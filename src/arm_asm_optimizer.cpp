/*
 *
 *  arm_asm_optimizer.cpp
 *  arm汇编代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/4/24
 *
*/
#include "arm_asm_optimizer.h"

/*
局部优化

Parameters
----------
arm_flow_graph:要优化的arm汇编代码流图
*/
void Arm_asm_optimizer::local_optimize(struct arm_flow_graph & arm_flow_graph)
{

}

/*
优化函数的出入口代码
把不需要保存到栈的上下文寄存器删去

Parameters
----------
func:要优化的arm函数
*/
void Arm_asm_optimizer::optimize_func_enter_and_exit(struct arm_func_flow_graph * arm_func)
{
    /*Arm_asm_file_line * enter_push=nullptr,* enter_vpush=nullptr,* exit_pop=nullptr,* exit_vpop=nullptr;
    Arm_cpu_multiple_registers_load_and_store_instruction * push_or_pop;
    Arm_vfp_data_process_instruction * vpush_or_vpop;
    set<Arm_asm_file_line * > exit_pops,exit_pops;
    Arm_instruction * instruction;
    Arm_pseudo_instruction * pseudo_instruction;
    set<reg_index> arm_func_regs_used;
    bool tag=false;
    for(auto i:arm_func->basic_blocks)
    {
        for(auto j:i->arm_sequence)
        {
            if(j->type_==arm_asm_file_line_type::INSTRUCTION)
            {
                instruction=(Arm_instruction *)j;
                for(auto k:instruction->destination_registers_.registers_)
                {
                    arm_func_regs_used.insert(k);
                }
                for(auto k:instruction->source_registers_.registers_)
                {
                    arm_func_regs_used.insert(k);
                }
                switch(instruction->op_)
                {
                    case arm_op::PUSH:
                        if(enter_push==nullptr)
                        {
                            push_or_pop=(Arm_cpu_multiple_registers_load_and_store_instruction *)j;
                            tag=true;
                        }
                        break;
                    case arm_op::POP:
                        exit_pop=j;
                        push_or_pop=(Arm_cpu_multiple_registers_load_and_store_instruction *)j;
                        
                        break;
                    case arm_op::VPUSH:
                        if(enter_push==nullptr && tag=true)
                        {
                            vpush_or_vpop=(Arm_vfp_multiple_registers_load_and_store_instruction *)j;
                            tag=false;
                        }
                        break;
                    case arm_op::VPOP:
                        exit_vpop=j;
                        break;
                    default:
                        break;
                }
            }
            else if(j->type_==arm_asm_file_line_type::PSEUDO_INSTRUCTION)
            {
                pseudo_instruction=(Arm_pseudo_instruction *)j;
                if(pseudo_instruction->op_==arm_pseudo_op::ADR || pseudo_instruction->op_==arm_pseudo_op::ADRL || pseudo_instruction->op_==arm_pseudo_op::LDR || pseudo_instruction->op_==arm_pseudo_op::VLDR)
                {
                    arm_func_regs_used.insert(pseudo_instruction->reg_);
                }
            }
        }
    }*/

}

/*
全局优化

Parameters
----------
arm_flow_graph:要优化的arm汇编代码流图
*/
void Arm_asm_optimizer::global_optimize(struct arm_flow_graph & arm_flow_graph)
{
    for(auto i:arm_flow_graph.func_flow_graphs)
    {
        //优化函数出入口代码
        optimize_func_enter_and_exit(i);
    }
}

/*
进行arm汇编优化

Parameters
----------
arm_flow_graph:要优化的arm汇编代码流图
*/
void Arm_asm_optimizer::optimize(struct arm_flow_graph & arm_flow_graph)
{
    //局部优化
    local_optimize(arm_flow_graph);
    //全局优化
    global_optimize(arm_flow_graph);
}