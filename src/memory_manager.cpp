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
    current_func_stack_space_.clear();
    
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

struct event Memory_manager::handle_READY_TO_PUSH_LOCAL_VARS(struct ic_func * func)
{
    struct event res;
    size_t local_vars_total_byte_size=0;
    set<struct ic_data * > local_vars=func->get_local_vars();
    for(auto i:local_vars)
    {
        if(i->is_const() && !i->is_array_var())
        {
            //如果局部变量是常数数组的话，仍旧需要将其入栈
            continue;
        }
        local_vars_total_byte_size+=i->get_4_bytes_size();
        current_func_stack_space_.push_to_local_vars(i);
    }
    res.type=event_type::RESPONSE_INT;
    res.int_data=local_vars_total_byte_size*4;
    return res;
}

void Memory_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(list<reg_index> * regs)
{
    for(list<reg_index>::reverse_iterator i=regs->rbegin();i!=regs->rend();i++)
    {
        current_func_stack_space_.push_to_context_saved_cpu_regs(*i,notify(event(event_type::GET_REG_BYTE_SIZE,(int)(*i))).int_data);
    }
}

void Memory_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(list<reg_index> * regs)
{
    for(list<reg_index>::reverse_iterator i=regs->rbegin();i!=regs->rend();i++)
    {
        current_func_stack_space_.push_to_context_saved_vfp_regs(*i,notify(event(event_type::GET_REG_BYTE_SIZE,(int)(*i))).int_data);
    }
}

void Memory_manager::handle_READY_TO_PUSH_F_PARAM_CPU_REGS(list<struct ic_data * > * f_params)
{
    size_t int_f_params_num=0;
    list<struct ic_data * > int_f_params_in_regs;

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
    }
}

void Memory_manager::handle_READY_TO_PUSH_F_PARAM_VFP_REGS(list<struct ic_data * > * f_params)
{
    size_t float_f_params_num=0;
    list<struct ic_data * > float_f_params_in_regs;

    //这里假设每一个参数都是4bytes大小的
    for(auto i:*f_params)
    {
        if(i->get_data_type()==language_data_type::FLOAT)
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
    }
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

struct event Memory_manager::handle_READY_TO_POP_TEMP_VARS()
{
    struct event res;
    size_t stack_offset=0;

    /*if(current_func_stack_space_.f_params_passed_by_stack_as_caller.empty() && !current_func_stack_space_.temp_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
    }*/

    if(!current_func_stack_space_.temp_vars.empty())
    {
        if(current_func_stack_space_.f_params_passed_by_stack_as_caller.empty())
        {
            stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
        }
        else
        {
            stack_offset=current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first-current_func_stack_space_.temp_vars.front().first;
        }
    }

    res.type=event_type::RESPONSE_INT;
    res.int_data=stack_offset;
    return res;
}

struct event Memory_manager::handle_READY_TO_POP_LOCAL_VARS()
{
    struct event res;
    size_t stack_offset=0;

    if(!current_func_stack_space_.local_vars.empty())
    {
        if(!current_func_stack_space_.temp_vars.empty())
        {
            stack_offset=current_func_stack_space_.temp_vars.front().first-current_func_stack_space_.local_vars.front().first;
        }
        else if(!current_func_stack_space_.f_params_passed_by_stack_as_caller.empty())
        {
            stack_offset=current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first-current_func_stack_space_.local_vars.front().first;
        }
        else
        {
            stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.local_vars.front().first;
        }
    }

    res.type=event_type::RESPONSE_INT;
    res.int_data=stack_offset;
    return res;
}

struct event Memory_manager::handle_READY_TO_POP_F_PARAM_CPU_REGS()
{
    struct event res;
    size_t stack_offset=0;

    if(!current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.empty())
    {
        if(!current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.empty())
        {
            stack_offset=current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
        }
        else if(!current_func_stack_space_.local_vars.empty())
        {
            stack_offset=current_func_stack_space_.local_vars.front().first-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
        }
        else if(!current_func_stack_space_.temp_vars.empty())
        {
            stack_offset=current_func_stack_space_.temp_vars.front().first-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
        }
        else if(!current_func_stack_space_.f_params_passed_by_stack_as_caller.empty())
        {
            stack_offset=current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
        }
        else
        {
            stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.f_params_passed_by_cpu_regs_as_callee.front().first;
        }
    }

    res.type=event_type::RESPONSE_INT;
    res.int_data=stack_offset;
    return res;
}

struct event Memory_manager::handle_READY_TO_POP_F_PARAM_VFP_REGS()
{
    struct event res;
    size_t stack_offset=0;

    if(!current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.empty())
    {
        if(!current_func_stack_space_.local_vars.empty())
        {
            stack_offset=current_func_stack_space_.local_vars.front().first-current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first;
        }
        else if(!current_func_stack_space_.temp_vars.empty())
        {
            stack_offset=current_func_stack_space_.temp_vars.front().first-current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first;
        }
        else if(!current_func_stack_space_.f_params_passed_by_stack_as_caller.empty())
        {
            stack_offset=current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first-current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first;
        }
        else
        {
            stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.f_params_passed_by_vfp_regs_as_callee.front().first;
        }
    }

    res.type=event_type::RESPONSE_INT;
    res.int_data=stack_offset;
    return res;
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
            break;
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
    current_func_stack_space_.push_to_f_params_passed_by_stack_as_caller(argument);
}

struct event Memory_manager::handle_RET_FROM_CALLED_FUNC()
{
    struct event res;
    res.type=event_type::RESPONSE_INT;
    res.int_data=0;
    if(!current_func_stack_space_.f_params_passed_by_stack_as_caller.empty())
    {
        res.int_data=current_func_stack_space_.stack_pointer-current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first;
        current_func_stack_space_.stack_pointer=current_func_stack_space_.f_params_passed_by_stack_as_caller.front().first;
        current_func_stack_space_.f_params_passed_by_stack_as_caller.clear();
    }
    return res;
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

void Memory_manager::handle_END_BASIC_BLOCK()
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG();
}

void Memory_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG()
{
    size_t stack_offset=0;

    //把此时所有的临时变量全部出栈
    if(current_func_stack_space_.f_params_passed_by_stack_as_caller.empty() && !current_func_stack_space_.temp_vars.empty())
    {
        stack_offset=current_func_stack_space_.stack_pointer-current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.stack_pointer=current_func_stack_space_.temp_vars.front().first;
        current_func_stack_space_.temp_vars.clear();
    }
    
    if(stack_offset>0)
    {
        notify(event(event_type::POP_STACK,(int)stack_offset));
    }
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
        case event_type::READY_TO_PUSH_LOCAL_VARS:
            response=handle_READY_TO_PUSH_LOCAL_VARS((struct ic_func *)event.pointer_data);
            break;
        case event_type::READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS:
            response=handle_READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS();
            break;
        case event_type::READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS:
            response=handle_READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS();
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS:
            handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(((list<reg_index> *)event.pointer_data));
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS:
            handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(((list<reg_index> *)event.pointer_data));
            break;
        case event_type::READY_TO_PUSH_F_PARAM_CPU_REGS:
            handle_READY_TO_PUSH_F_PARAM_CPU_REGS((((struct ic_func *)event.pointer_data)->f_params));
            break;
        case event_type::READY_TO_PUSH_F_PARAM_VFP_REGS:
            handle_READY_TO_PUSH_F_PARAM_VFP_REGS((((struct ic_func *)event.pointer_data)->f_params));
            break;
        case event_type::READY_TO_POP_TEMP_VARS:
            response=handle_READY_TO_POP_TEMP_VARS();
            break;
        case event_type::READY_TO_POP_LOCAL_VARS:
            response=handle_READY_TO_POP_LOCAL_VARS();
            break;
        case event_type::READY_TO_POP_F_PARAM_CPU_REGS:
            response=handle_READY_TO_POP_F_PARAM_CPU_REGS();
            break;
        case event_type::READY_TO_POP_F_PARAM_VFP_REGS:
            response=handle_READY_TO_POP_F_PARAM_VFP_REGS();
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
        case event_type::RET_FROM_CALLED_FUNC:
            response=handle_RET_FROM_CALLED_FUNC();
            break;
        case event_type::CHECK_TEMP_VAR_IN_STACK:
            response=handle_CHECK_TEMP_VAR_IN_STACK((struct ic_data *)event.pointer_data);
            break;
        case event_type::END_BASIC_BLOCK:
            handle_END_BASIC_BLOCK();
            break;
        case event_type::END_BASIC_BLOCK_WITHOUT_FLAG:
            handle_END_BASIC_BLOCK_WITHOUT_FLAG();
            break;
        case event_type::IS_F_PARAM_PASSED_BY_STACK:
            response=handle_IS_F_PARAM_PASSED_BY_STACK((struct ic_data *)event.pointer_data);
            break;
        default:
            break;
    }
    return response;
}