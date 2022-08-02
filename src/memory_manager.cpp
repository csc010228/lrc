/*
 *
 *  memory_manager.cpp
 *  内存管理器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#include "memory_manager.h"

//构造函数
Memory_manager::Memory_manager(string memory_info)
{
    is_init_successful_=true;
}

//析构函数
Memory_manager::~Memory_manager()
{

}

//检查是否构造成功
bool Memory_manager::is_init_successful()
{
    return is_init_successful_;
}

void Memory_manager::handle_FUNC_DEFINE(struct ic_func * func)
{
    size_t int_f_parmas_num=0,float_f_parmas_num=0;
    list<struct ic_data * > f_params_in_stack;
    
    //如果此时函数的参数的整型部分大小超过了4*32bits，浮点部分大小超过了16*32bits，那么超出的参数部分已经按照从右往左的顺序放入栈中了
    //默认函数参数都是4bytes
    for(auto i:(*func->f_params))
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            int_f_parmas_num+=i->get_4_bytes_size();
            if(int_f_parmas_num>4)
            {
                f_params_in_stack.push_front(i);
            }
        }
        else if(i->get_data_type()==language_data_type::FLOAT)
        {
            float_f_parmas_num+=i->get_4_bytes_size();
            if(float_f_parmas_num>16)
            {
                f_params_in_stack.push_front(i);
            }
        }
    }
    for(auto i:f_params_in_stack)
    {
        current_func_stack_space_.push_to_f_params_passed_by_stack_as_callee(i);
    }
}

struct event Memory_manager::handle_READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK(struct ic_func * func)
{
    size_t total_byte_size=0,float_f_params_num=0,int_f_params_num=0;
    list<struct ic_data * > local_vars=func->get_local_vars(),* f_params=func->f_params,float_f_params_in_regs,int_f_params_in_regs;
    //把通过CPU寄存器传递的函数参数分配栈空间
    //这里假设每一个参数都是4bytes大小的
    for(auto i:*f_params)
    {
        //包括数组，因为在函数参数传递的时候，数组是使用地址的形式来进行传递的
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            if(int_f_params_num>=4)
            {
                break;
            }
            int_f_params_in_regs.push_front(i);
            int_f_params_num++;
        }
    }
    for(auto i:int_f_params_in_regs)
    {
        current_func_stack_space_.push_to_f_params_passed_by_cpu_regs_as_callee(i);
        total_byte_size+=(i->get_4_bytes_size());
    }
    //把通过VFP寄存器传递的函数参数分配栈空间
    //这里假设每一个参数都是4bytes大小的
    for(auto i:*f_params)
    {
        if(i->get_data_type()==language_data_type::FLOAT && !i->is_array_var())
        {
            if(float_f_params_num>=16)
            {
                break;
            }
            float_f_params_in_regs.push_front(i);
            float_f_params_num++;
        }
    }
    for(auto i:float_f_params_in_regs)
    {
        current_func_stack_space_.push_to_f_params_passed_by_vfp_regs_as_callee(i);
        total_byte_size+=(i->get_4_bytes_size());
    }
    //把那些跨越了基本块的临时变量也作为局部变量进行处理
    set<struct ic_data * > * temp_vars_over_basic_blocks=(set<struct ic_data * > * )notify(event(event_type::GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC,nullptr)).pointer_data;
    for(auto temp_var_over_basic_blocks:(*temp_vars_over_basic_blocks))
    {
        local_vars.push_back(temp_var_over_basic_blocks);
    }
    for(auto local_var:local_vars)
    {
        if(local_var->is_const() && !local_var->is_array_var())
        {
            //如果局部变量是常数数组的话，仍旧需要将其入栈
            continue;
        }
        total_byte_size+=(local_var->get_4_bytes_size());
        current_func_stack_space_.push_to_local_vars(local_var);
    }
    total_byte_size*=4;
    //栈8bytes对齐
    if(total_byte_size%8!=0)
    {
        current_func_stack_space_.set_padding_bytes_after_local_vars(8-(total_byte_size%8));
    }
    total_byte_size+=current_func_stack_space_.padding_bytes_after_local_vars;
    return event(event_type::RESPONSE_INT,(int)total_byte_size);
}

struct event Memory_manager::handle_READY_TO_PUSH_TEMP_VARS(set<struct ic_data * > * temp_vars)
{
    size_t temp_vars_total_byte_size=0;
    for(auto temp_var:(*temp_vars))
    {
        if(temp_var->is_tmp_var())
        {
            temp_vars_total_byte_size+=(temp_var->get_4_bytes_size());
            current_func_stack_space_.push_to_temp_vars(temp_var);
        }
    }
    temp_vars_total_byte_size*=4;
    //栈8bytes对齐
    if(temp_vars_total_byte_size%8!=0)
    {
        current_func_stack_space_.set_padding_bytes_after_temp_vars(8-(temp_vars_total_byte_size%8));
    }
    temp_vars_total_byte_size+=current_func_stack_space_.padding_bytes_after_temp_vars;
    return event(event_type::RESPONSE_INT,(int)temp_vars_total_byte_size);
}

struct event Memory_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(struct ic_func * func)
{
    list<reg_index> * regs=(list<reg_index> *)notify(event(event_type::GET_ALL_CONTEXT_SAVED_CPU_REGS,(void *)func)).pointer_data;
    for(list<reg_index>::reverse_iterator i=regs->rbegin();i!=regs->rend();i++)
    {
        current_func_stack_space_.push_to_context_saved_cpu_regs(*i,notify(event(event_type::GET_REG_BYTE_SIZE,(int)(*i))).int_data);
    }
    return event(event_type::RESPONSE_POINTER,(void *)regs);
}

struct event Memory_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(struct ic_func * func)
{
    list<reg_index> * regs=(list<reg_index> *)notify(event(event_type::GET_ALL_CONTEXT_SAVED_VFP_REGS,(void *)func)).pointer_data;
    for(list<reg_index>::reverse_iterator i=regs->rbegin();i!=regs->rend();i++)
    {
        current_func_stack_space_.push_to_context_saved_vfp_regs(*i,notify(event(event_type::GET_REG_BYTE_SIZE,(int)(*i))).int_data);
    }
    return event(event_type::RESPONSE_POINTER,(void *)regs);
}

struct event Memory_manager::handle_GET_CURRENT_FUNC_STACK_SIZE()
{
    return event(event_type::RESPONSE_INT,(int)(current_func_stack_space_.stack_pointer-current_func_stack_space_.frame_pointer));
}

struct event Memory_manager::handle_CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC()
{
    if((current_func_stack_space_.stack_pointer-current_func_stack_space_.context_saved_cpu_regs_as_callee.front().first)%8!=0)
    {
        current_func_stack_space_.set_padding_bytes_after_context_saved_regs(8-((current_func_stack_space_.stack_pointer-current_func_stack_space_.context_saved_cpu_regs_as_callee.front().first)%8));
    }
    return event(event_type::RESPONSE_INT,(int)current_func_stack_space_.padding_bytes_after_context_saved_regs);
}

struct event Memory_manager::handle_GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC()
{
    return event(event_type::RESPONSE_INT,(int)current_func_stack_space_.padding_bytes_after_context_saved_regs);
}

struct event Memory_manager::handle_READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS()
{
    struct event res;
    list<reg_index> * regs=new list<reg_index>;
    reg_index lr=(reg_index)notify(event(event_type::GET_LR_REG,nullptr)).int_data,pc=(reg_index)notify(event(event_type::GET_PC_REG,nullptr)).int_data;
    for(auto i:current_func_stack_space_.context_saved_cpu_regs_as_callee)
    {
        if(i.second==lr)
        {
            regs->push_front(pc);
        }
        else
        {
            regs->push_front(i.second);
        }
    }
    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)regs;
    return res;
}

struct event Memory_manager::handle_READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS()
{
    struct event res;
    list<reg_index> * regs=new list<reg_index>;

    for(auto i:current_func_stack_space_.context_saved_vfp_regs_as_callee)
    {
        regs->push_front(i.second);
    }

    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)regs;
    return res;
}

struct event Memory_manager::handle_READY_TO_POP_WHEN_RET()
{
    size_t stack_offset;
    if(!current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
    }
    else if(!current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first;
    }
    else if(!current_func_stack_space_.local_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.local_vars.front().first;
    }
    else if(!current_func_stack_space_.temp_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
    }
    else
    {
        stack_offset=current_func_stack_space_.padding_bytes_after_local_vars+current_func_stack_space_.padding_bytes_after_temp_vars;
    }
    return event(event_type::RESPONSE_INT,(int)(stack_offset));
}

struct event Memory_manager::handle_GET_VAR_STACK_POS_FROM_SP(struct ic_data * var)
{
    struct event res;
    res.type=event_type::RESPONSE_INT;

    for(auto i:current_func_stack_space_.temp_vars)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.stack_pointer-i.first-var->get_byte_size();
            return res;
        }
    }

    for(auto i:current_func_stack_space_.local_vars)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.stack_pointer-i.first-var->get_byte_size();
            return res;
        }
    }

    for(auto i:current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.stack_pointer-i.first-var->get_byte_size();
            return res;
        }
    }

    for(auto i:current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.stack_pointer-i.first-var->get_byte_size();
            return res;
        }
    }

    for(auto i:current_func_stack_space_.f_params_passed_by_stack_as_callee)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.stack_pointer-i.first-var->get_byte_size();
            return res;
        }
    }
    return res;
}

struct event Memory_manager::handle_GET_VAR_STACK_POS_FROM_FP(struct ic_data * var)
{
    //要求var必须是通过栈传递的参数
    struct event res(event_type::RESPONSE_INT,0);
    for(auto i:current_func_stack_space_.f_params_passed_by_stack_as_callee)
    {
        if(var==i.second)
        {
            res.int_data=current_func_stack_space_.frame_pointer-i.first-var->get_byte_size();
            break;
        }
    }
    return res;
}

void Memory_manager::handle_CLEAR_CURRENT_BASIC_BLOCK_STACK()
{
    size_t stack_offset=0;
    //把此时所有的临时变量全部出栈
    if(current_func_stack_space_.r_params_passed_by_stack_as_caller.empty() && !current_func_stack_space_.temp_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.stack_pointer=current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.temp_vars.clear();
        current_func_stack_space_.padding_bytes_after_temp_vars=0;
    }
    else if(current_func_stack_space_.padding_bytes_after_temp_vars!=0)
    {
        current_func_stack_space_.stack_pointer-=current_func_stack_space_.padding_bytes_after_temp_vars;
        stack_offset=current_func_stack_space_.padding_bytes_after_temp_vars;
        current_func_stack_space_.padding_bytes_after_temp_vars=0;
    }
    notify(event(event_type::POP_STACK_WHEN_EXIT_BASIC_BLOCK,(int)stack_offset));
}

void Memory_manager::handle_PUSH_VAR_TO_STACK(struct ic_data * var)
{
    if(var->is_tmp_var())
    {
        current_func_stack_space_.push_to_temp_vars(var);
    }
    else
    {
        current_func_stack_space_.push_to_local_vars(var);
    }
}

void Memory_manager::handle_PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC(struct ic_data * argument)
{
    current_func_stack_space_.push_to_r_params_passed_by_stack_as_caller(argument);
}

void Memory_manager::handle_PADDING_WHEN_CALL_FUNC(int padding_bytes)
{
    current_func_stack_space_.set_padding_bytes_before_r_params_passed_by_stack_as_caller(padding_bytes);
}

struct event Memory_manager::handle_RET_FROM_CALLED_FUNC()
{
    size_t stack_offset=0;
    if(!current_func_stack_space_.r_params_passed_by_stack_as_caller.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.r_params_passed_by_stack_as_caller.front().first;
        current_func_stack_space_.stack_pointer=current_func_stack_space_.r_params_passed_by_stack_as_caller.front().first;
        current_func_stack_space_.r_params_passed_by_stack_as_caller.clear();
    }
    stack_offset+=current_func_stack_space_.padding_bytes_before_r_params_passed_by_stack_as_caller;
    current_func_stack_space_.stack_pointer-=current_func_stack_space_.padding_bytes_before_r_params_passed_by_stack_as_caller;
    current_func_stack_space_.padding_bytes_before_r_params_passed_by_stack_as_caller=0;
    return event(event_type::RESPONSE_INT,(int)stack_offset);
}

struct event Memory_manager::handle_RET_FROM_CALLED_ABI_FUNC()
{
    return handle_RET_FROM_CALLED_FUNC();
}

struct event Memory_manager::handle_CHECK_TEMP_VAR_IN_STACK(struct ic_data * var)
{
    struct event res;
    res.type=event_type::RESPONSE_BOOL;
    res.bool_data=false;
    for(auto i:current_func_stack_space_.temp_vars)
    {
        if(i.second==var)
        {
            res.bool_data=true;
        }
    }
    return res;
}

void Memory_manager::handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG(basic_block);
}

void Memory_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block)
{
    static struct ic_basic_block * pre_basic_block=nullptr;
    size_t stack_offset=0;
    if(pre_basic_block==basic_block)
    {
        return;
    }
    pre_basic_block=basic_block;
    //把此时所有的临时变量全部出栈
    /*if(current_func_stack_space_.r_params_passed_by_stack_as_caller.empty() && !current_func_stack_space_.temp_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.stack_pointer=current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.temp_vars.clear();
        current_func_stack_space_.padding_bytes_after_temp_vars=0;
    }
    else if(current_func_stack_space_.padding_bytes_after_temp_vars!=0)
    {
        current_func_stack_space_.stack_pointer-=current_func_stack_space_.padding_bytes_after_temp_vars;
        stack_offset=current_func_stack_space_.padding_bytes_after_temp_vars;
        current_func_stack_space_.padding_bytes_after_temp_vars=0;
    }
    notify(event(event_type::POP_STACK_WHEN_EXIT_BASIC_BLOCK,(int)stack_offset));*/
    handle_CLEAR_CURRENT_BASIC_BLOCK_STACK();
}

struct event Memory_manager::handle_IS_F_PARAM_PASSED_BY_STACK(struct ic_data * var)
{
    for(auto i:current_func_stack_space_.f_params_passed_by_stack_as_callee)
    {
        if(var==i.second)
        {
            return event(event_type::RESPONSE_BOOL,true);
        }
    }
    return event(event_type::RESPONSE_BOOL,false);
}

void Memory_manager::handle_RECORD_STACK_SPACE_CHANGED_HERE(list<Arm_asm_file_line * >::iterator * current_instruction_pos)
{
    stack_space_changed_poses.push_back(make_pair(*current_instruction_pos,current_func_stack_space_));
}

void Memory_manager::handle_SET_STACK_SPACE(list<Arm_asm_file_line * >::iterator * current_instruction_pos)
{
    pair<list<Arm_asm_file_line * >::iterator,struct runtime_stack_space > stack_space_changed_pos=stack_space_changed_poses.front();
    if((*current_instruction_pos)==stack_space_changed_pos.first)
    {
        current_func_stack_space_=stack_space_changed_pos.second;
        stack_space_changed_poses.pop_front();
        stack_space_changed_poses.push_back(stack_space_changed_pos);
    }
}

void Memory_manager::handle_END_FUNC()
{
    current_func_stack_space_.clear();
    stack_space_changed_poses.clear();
}

/*
事件处理函数(由中介者进行调用)

Parameters
----------
event:要处理的事件信息

Return
------
处理完事件之后返回响应信息
*/
struct event Memory_manager::handler(struct event event)
{
    struct event response;
    switch(event.type)
    {
        case event_type::FUNC_DEFINE:
            handle_FUNC_DEFINE((struct ic_func *)event.pointer_data);
            break;
        case event_type::READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK:
            response=handle_READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK((struct ic_func *)event.pointer_data);
            break;
        case event_type::READY_TO_PUSH_TEMP_VARS:
            response=handle_READY_TO_PUSH_TEMP_VARS((set<struct ic_data * > *)event.pointer_data);
            break;
        case event_type::READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS:
            response=handle_READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS();
            break;
        case event_type::READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS:
            response=handle_READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS();
            break;
        case event_type::GET_CURRENT_FUNC_STACK_SIZE:
            response=handle_GET_CURRENT_FUNC_STACK_SIZE();
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS:
            response=handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(((struct ic_func *)event.pointer_data));
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS:
            response=handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(((struct ic_func *)event.pointer_data));
            break;
        case event_type::CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC:
            response=handle_CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC();
            break;
        case event_type::GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC:
            response=handle_GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC();
            break;
        case event_type::READY_TO_POP_WHEN_RET:
            response=handle_READY_TO_POP_WHEN_RET();
            break;
        case event_type::CLEAR_CURRENT_BASIC_BLOCK_STACK:
            handle_CLEAR_CURRENT_BASIC_BLOCK_STACK();
            break;
        case event_type::GET_VAR_STACK_POS_FROM_SP:
            response=handle_GET_VAR_STACK_POS_FROM_SP((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_VAR_STACK_POS_FROM_FP:
            response=handle_GET_VAR_STACK_POS_FROM_FP((struct ic_data *)event.pointer_data);
            break;
        case event_type::PUSH_VAR_TO_STACK:
            handle_PUSH_VAR_TO_STACK((struct ic_data *)event.pointer_data);
            break;
        case event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC:
            handle_PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC((struct ic_data *)event.pointer_data);
            break;
        case event_type::PADDING_WHEN_CALL_FUNC:
            handle_PADDING_WHEN_CALL_FUNC(event.int_data);
            break;
        case event_type::RET_FROM_CALLED_FUNC:
            response=handle_RET_FROM_CALLED_FUNC();
            break;
        case event_type::RET_FROM_CALLED_ABI_FUNC:
            response=handle_RET_FROM_CALLED_ABI_FUNC();
            break;
        case event_type::CHECK_TEMP_VAR_IN_STACK:
            response=handle_CHECK_TEMP_VAR_IN_STACK((struct ic_data *)event.pointer_data);
            break;
        case event_type::END_BASIC_BLOCK:
            handle_END_BASIC_BLOCK((struct ic_basic_block *)event.pointer_data);
            break;
        case event_type::END_BASIC_BLOCK_WITHOUT_FLAG:
            handle_END_BASIC_BLOCK_WITHOUT_FLAG((struct ic_basic_block *)event.pointer_data);
            break;
        case event_type::IS_F_PARAM_PASSED_BY_STACK:
            response=handle_IS_F_PARAM_PASSED_BY_STACK((struct ic_data *)event.pointer_data);
            break;
        case event_type::RECORD_STACK_SPACE_CHANGED_HERE:
            handle_RECORD_STACK_SPACE_CHANGED_HERE((list<Arm_asm_file_line * >::iterator *)event.pointer_data);
            break;
        case event_type::SET_STACK_SPACE:
            handle_SET_STACK_SPACE((list<Arm_asm_file_line * >::iterator *)event.pointer_data);
            break;
        case event_type::END_FUNC:
            handle_END_FUNC();
            break;
        default:
            break;
    }
    return response;
}