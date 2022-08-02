/*
 *
 *  virtual_arm_asm_optimizer.cpp
 *  arm架构下的虚拟目标代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/8/1
 *
*/

#include "virtual_arm_asm_optimizer.h"

Virtual_arm_asm_optimizer::Virtual_arm_asm_optimizer(bool optimize):Virtual_asm_optimizer(optimize),virtual_target_code_(nullptr)
{

}

Virtual_arm_asm_optimizer::~Virtual_arm_asm_optimizer()
{
    
}

void Virtual_arm_asm_optimizer::optimize_func_and_basic_block_s_enter_and_exit()
{
    /*
    这个优化做了之后有很多情况下性能不升反降???
    */
    bool optimize_func_enter_exit=true;
    Arm_instruction * instruction;
    Arm_pseudo_instruction * pseudo_instruction;
    Arm_cpu_instruction * cpu_instruction;
    Arm_cpu_data_process_instruction * cpu_data_process_instruction;
    list<Arm_asm_file_line * >::iterator opt_line;
    list<list<Arm_asm_file_line * >::iterator> sub_sp_inses,add_sp_inses,sp_inses_need_optimize;
    set<reg_index> destination_regs,source_regs,all_regs;
    set<struct arm_basic_block * > bbs,ret_func_bbs;
    struct arm_basic_block * enter_func_bbs;
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data,
    fp=(reg_index)notify(event(event_type::GET_FP_REG,nullptr)).int_data,
    lr=(reg_index)notify(event(event_type::GET_LR_REG,nullptr)).int_data;
    for(auto basic_block:virtual_target_code_->basic_blocks)
    {
        //当基本块满足下列条件的时候，我们认为该基本块出入口的移动sp为临时变量开辟和释放内存空间是可以省略的
        //该基本块中只有add sp,sp,...和sub sp,sp,...等指令涉及到了sp寄存器，其他的指令没有涉及到寄存器
        add_sp_inses.clear();
        sub_sp_inses.clear();
        for(list<Arm_asm_file_line * >::iterator line=basic_block->arm_sequence.begin();line!=basic_block->arm_sequence.end();line++)
        {
            if((*line)->is_instruction())
            {
                instruction=dynamic_cast<Arm_instruction *>(*line);
                if(instruction->is_cpu_instruction())
                {
                    cpu_instruction=dynamic_cast<Arm_cpu_instruction * >(instruction);
                    if(cpu_instruction->is_data_process_instruction())
                    {
                        cpu_data_process_instruction=dynamic_cast<Arm_cpu_data_process_instruction *>(cpu_instruction);
                        all_regs=cpu_data_process_instruction->get_all_regs();
                        if(all_regs.find(sp)!=all_regs.end())
                        {
                            destination_regs=cpu_data_process_instruction->get_all_destination_regs();
                            source_regs=cpu_data_process_instruction->get_all_source_regs();
                            if(destination_regs.size()==1 && 
                            source_regs.size()==1 && 
                            (*destination_regs.begin()==sp || *destination_regs.begin()==fp && cpu_data_process_instruction->get_op()==arm_op::ADD) && 
                            *source_regs.begin()==sp && 
                            (cpu_data_process_instruction->get_op()==arm_op::ADD || 
                            cpu_data_process_instruction->get_op()==arm_op::SUB))
                            {
                                if(cpu_data_process_instruction->get_op()==arm_op::ADD && *destination_regs.begin()==sp)
                                {
                                    add_sp_inses.push_back(line);
                                }
                                else if(cpu_data_process_instruction->get_op()==arm_op::SUB)
                                {
                                    sub_sp_inses.push_back(line);
                                }
                            }
                            else
                            {
                                optimize_func_enter_exit=false;
                                goto next;
                            }
                        }
                    }
                }
            }
            else if((*line)->is_pseudo_instruction())
            {
                pseudo_instruction=dynamic_cast<Arm_pseudo_instruction * >(*line);
                all_regs=pseudo_instruction->get_all_regs();
                if(all_regs.find(sp)!=all_regs.end())
                {
                    optimize_func_enter_exit=false;
                    goto next;
                }
            }
        }
        //如果一个基本块是函数入口的基本块，那么我们认为该基本块入口的移动sp为临时变量开辟内存空间是第3行sub sp,sp,...
        //否则的话，我们认为该基本块入口的移动sp为临时变量开辟内存空间是第1行sub sp,sp,...
        bbs=basic_block->get_precursors();
        if(bbs.empty() && sub_sp_inses.size()>=3)
        {
            sub_sp_inses.pop_front();
            sp_inses_need_optimize.push_back(sub_sp_inses.front());
            sub_sp_inses.pop_front();
        }
        opt_line=sub_sp_inses.front();
        (*opt_line)=new Arm_pseudo_instruction();
        //如果一个基本块是具有函数返回语句的基本块，那么我们认为该基本块入口的移动sp为临时变量释放内存空间是倒数第4行和倒数第1行add sp,sp,...
        //否则的话，我们认为该基本块入口的移动sp为临时变量释放内存空间是倒数第1行add sp,sp,...
        bbs=basic_block->get_successors();
        if(bbs.empty() && add_sp_inses.size()>=4)
        {
            opt_line=add_sp_inses.back();
            (*opt_line)=new Arm_pseudo_instruction();
            add_sp_inses.pop_back();
            add_sp_inses.pop_back();
            sp_inses_need_optimize.push_back(add_sp_inses.back());
            add_sp_inses.pop_back();
        }
        opt_line=add_sp_inses.back();
        (*opt_line)=new Arm_pseudo_instruction();
next:
        ;
    }
    if(optimize_func_enter_exit)
    {
        for(auto sp_ins_need_optimize:sp_inses_need_optimize)
        {
            (*sp_ins_need_optimize)=new Arm_pseudo_instruction();
        }
    }
}

void Virtual_arm_asm_optimizer::local_optimize()
{
    
}

void Virtual_arm_asm_optimizer::global_optimize()
{
    
}

void Virtual_arm_asm_optimizer::handle_OPTIMIZE(struct arm_func_flow_graph * virtual_target_code)
{
    virtual_target_code_=virtual_target_code;
    optimize_func_and_basic_block_s_enter_and_exit();
    local_optimize();
    global_optimize();
}

struct event Virtual_arm_asm_optimizer::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::OPTIMIZE:
            handle_OPTIMIZE((struct arm_func_flow_graph *)event.pointer_data);
            break;
        default:
            break;
    }
    return res;
}