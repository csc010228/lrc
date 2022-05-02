/*
 *
 *  arm_asm_optimizer.cpp
 *  arm汇编代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/4/24
 *
*/
#include "arm_asm_optimizer.h"
#include <algorithm>

/*
构造函数

Parameters
----------
arm_instruction_generatorz:该优化器所属的汇编代码生成器
*/
Arm_asm_optimizer::Arm_asm_optimizer()
{

}

/*
析构函数
*/
Arm_asm_optimizer::~Arm_asm_optimizer()
{

}

/*
局部优化
*/
void Arm_asm_optimizer::local_optimize()
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
    Arm_cpu_multiple_registers_load_and_store_instruction * enter_push=nullptr,* exit_pop=nullptr;
    Arm_vfp_multiple_registers_load_and_store_instruction * enter_vpush=nullptr,* exit_vpop=nullptr;
    set<Arm_cpu_multiple_registers_load_and_store_instruction * > exit_pops;
    set<Arm_vfp_multiple_registers_load_and_store_instruction * > exit_vpops;
    Arm_cpu_data_process_instruction * cpu_data_process_instruction,* assign_fp=nullptr;;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_instruction;
    Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_instruction;
    Arm_instruction * instruction;
    Arm_pseudo_instruction * pseudo_instruction;
    set<reg_index> used_regs;
    list<reg_index> used_cpu_temp_regs,used_vfp_temp_regs,push_cpu_regs,pop_cpu_regs;
    int fp_sp_deviation=0;
    reg_index pc=(reg_index)notify(event(event_type::GET_PC_REG,nullptr)).int_data,sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data,lr=(reg_index)notify(event(event_type::GET_LR_REG,nullptr)).int_data,fp=(reg_index)notify(event(event_type::GET_FP_REG,nullptr)).int_data;
    bool tag=false;
    //统计整个函数中用到的所有的寄存器
    for(auto basic_block:arm_func->basic_blocks)
    {
        for(auto arm_asm:basic_block->arm_sequence)
        {
            if(arm_asm->is_instruction())
            {
                instruction=dynamic_cast<Arm_instruction *>(arm_asm);
                switch(instruction->get_op())
                {
                    case arm_op::PUSH:
                        if(enter_push==nullptr)
                        {
                            enter_push=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(instruction);
                            goto next;
                        }
                        break;
                    case arm_op::POP:
                        exit_pop=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(instruction);
                        if(find(exit_pop->get_destination_registers().registers_.begin(), exit_pop->get_destination_registers().registers_.end(),pc)!=exit_pop->get_destination_registers().registers_.end())
                        {
                            exit_pops.insert(exit_pop);
                            exit_vpops.insert(exit_vpop);
                            goto next;
                        }
                        else
                        {
                            for(auto reg:exit_vpop->get_destination_registers().registers_)
                            {
                                used_regs.insert(reg);
                            }
                        }
                        break;
                    case arm_op::VPUSH:
                        if(enter_vpush==nullptr)
                        {
                            enter_vpush=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(instruction);
                            goto next;
                        }
                        break;
                    case arm_op::VPOP:
                        exit_vpop=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(instruction);
                        goto next;
                        break;
                    case arm_op::ADD:
                    case arm_op::SUB:
                    case arm_op::RSB:
                    case arm_op::ADC:
                    case arm_op::SBC:
                    case arm_op::RSC:
                    case arm_op::AND:
                    case arm_op::ORR:
                    case arm_op::EOR:
                    case arm_op::BIC:
                    case arm_op::CMP:
                    case arm_op::CMN:
                    case arm_op::TST:
                    case arm_op::TEQ:
                    case arm_op::MOV:
                    case arm_op::MVN:
                        if(assign_fp==nullptr && instruction->get_op()==arm_op::ADD)
                        {
                            assign_fp=dynamic_cast<Arm_cpu_data_process_instruction *>(instruction);
                            goto next;
                        }
                        cpu_data_process_instruction=dynamic_cast<Arm_cpu_data_process_instruction *>(instruction);
                        if(cpu_data_process_instruction->get_operand2().type==operand2_type::RM_SHIFT)
                        {
                            used_regs.insert(cpu_data_process_instruction->get_operand2().Rm_shift.Rm);
                            if(cpu_data_process_instruction->get_operand2().Rm_shift.shift_op==operand2_shift_op::ASR_RS || cpu_data_process_instruction->get_operand2().Rm_shift.shift_op==operand2_shift_op::LSL_RS || cpu_data_process_instruction->get_operand2().Rm_shift.shift_op==operand2_shift_op::LSR_RS || cpu_data_process_instruction->get_operand2().Rm_shift.shift_op==operand2_shift_op::ROR_RS)
                            {
                                used_regs.insert(cpu_data_process_instruction->get_operand2().Rm_shift.Rs);
                            }
                        }
                        break;
                    case arm_op::LDR:
                    case arm_op::STR:
                        cpu_single_register_load_and_store_instruction=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(instruction);
                        if(cpu_single_register_load_and_store_instruction->get_flexoffset().type==flexoffset_type::RM_SHIFT)
                        {
                            used_regs.insert(cpu_single_register_load_and_store_instruction->get_flexoffset().Rm_shift.Rm);
                        }
                        break;
                    case arm_op::VLDR:
                    case arm_op::VSTR:
                        vfp_single_register_load_and_store_instruction=dynamic_cast<Arm_vfp_single_register_load_and_store_instruction *>(instruction);
                        if(vfp_single_register_load_and_store_instruction->get_flexoffset().type==flexoffset_type::RM_SHIFT)
                        {
                            used_regs.insert(vfp_single_register_load_and_store_instruction->get_flexoffset().Rm_shift.Rm);
                        }
                        break;
                    default:
                        break;
                }
                for(auto reg:instruction->get_destination_registers().registers_)
                {
                    used_regs.insert(reg);
                }
                for(auto reg:instruction->get_source_registers().registers_)
                {
                    used_regs.insert(reg);
                }
next:
                ;
            }
            else if(arm_asm->is_pseudo_instruction())
            {
                pseudo_instruction=dynamic_cast<Arm_pseudo_instruction *>(arm_asm);
                if(pseudo_instruction->get_op()==arm_pseudo_op::ADR || pseudo_instruction->get_op()==arm_pseudo_op::ADRL || pseudo_instruction->get_op()==arm_pseudo_op::LDR || pseudo_instruction->get_op()==arm_pseudo_op::VLDR)
                {
                    used_regs.insert(pseudo_instruction->get_reg());
                }
            }
        }
    }
    //在函数的出入口处简化保存上下文的代码
    for(auto reg:used_regs)
    {
        if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data && notify(event(event_type::IS_TEMP_REG,(int)reg)).bool_data)
        {
            used_cpu_temp_regs.push_back(reg);
        }
        else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data && notify(event(event_type::IS_TEMP_REG,(int)reg)).bool_data)
        {
            used_vfp_temp_regs.push_back(reg);
        }
    }
    push_cpu_regs=used_cpu_temp_regs;
    push_cpu_regs.push_back(fp);
    push_cpu_regs.push_back(lr);
    pop_cpu_regs=used_cpu_temp_regs;
    pop_cpu_regs.push_back(fp);
    pop_cpu_regs.push_back(pc);
    for(auto basic_block:arm_func->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator arm_asm=basic_block->arm_sequence.begin();arm_asm!=basic_block->arm_sequence.end();arm_asm++)
        {
            if(enter_push==*arm_asm)
            {
                delete *arm_asm;
                if(push_cpu_regs.empty())
                {
                    *arm_asm=new Arm_pseudo_instruction();
                }
                else
                {
                    *arm_asm=new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(push_cpu_regs));
                }
            }
            else if(enter_vpush==*arm_asm)
            {
                delete *arm_asm;
                if(used_vfp_temp_regs.empty())
                {
                    *arm_asm=new Arm_pseudo_instruction();
                }
                else
                {
                    *arm_asm=new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(used_vfp_temp_regs));
                }
            }
            else if(assign_fp==*arm_asm)
            {
                for(auto reg:push_cpu_regs)
                {
                    fp_sp_deviation+=notify(event(event_type::GET_REG_BYTE_SIZE,(int)reg)).int_data;
                }
                for(auto reg:used_vfp_temp_regs)
                {
                    fp_sp_deviation+=notify(event(event_type::GET_REG_BYTE_SIZE,(int)reg)).int_data;
                }
                if(fp_sp_deviation==0)
                {
                    *arm_asm=new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NONE,false,fp,operand2(sp));
                }
                else
                {
                    *arm_asm=new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,fp,sp,operand2(fp_sp_deviation));
                }
            }
            else if(exit_pops.find((Arm_cpu_multiple_registers_load_and_store_instruction *)*arm_asm)!=exit_pops.end())
            {
                exit_pops.erase((Arm_cpu_multiple_registers_load_and_store_instruction *)*arm_asm);
                delete *arm_asm;
                if(pop_cpu_regs.empty())
                {
                    *arm_asm=new Arm_pseudo_instruction();
                }
                else
                {
                    *arm_asm=new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::POP,arm_condition::NONE,arm_registers(pop_cpu_regs));
                }
            }
            else if(exit_vpops.find((Arm_vfp_multiple_registers_load_and_store_instruction *)*arm_asm)!=exit_vpops.end())
            {
                exit_vpops.erase((Arm_vfp_multiple_registers_load_and_store_instruction *)*arm_asm);
                delete *arm_asm;
                if(used_vfp_temp_regs.empty())
                {
                    *arm_asm=new Arm_pseudo_instruction();
                }
                else
                {
                    *arm_asm=new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPOP,arm_condition::NONE,arm_registers(used_vfp_temp_regs));
                }
            }
        }
    }
}

/*
全局优化
*/
void Arm_asm_optimizer::global_optimize()
{
    for(auto func:arm_flow_graph_->func_flow_graphs)
    {
        //优化函数出入口代码
        optimize_func_enter_and_exit(func);
    }
}

/*
进行arm汇编优化

Parameters
----------
arm_flow_graph:要优化的arm汇编代码流图
*/
void Arm_asm_optimizer::handle_OPTIMIZE(struct arm_flow_graph * arm_flow_graph)
{
    arm_flow_graph_=arm_flow_graph;
    //局部优化
    local_optimize();
    //全局优化
    global_optimize();
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
struct event Arm_asm_optimizer::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::OPTIMIZE:
            handle_OPTIMIZE((struct arm_flow_graph *)event.pointer_data);
            break;
        default:
            break;
    }
    return res;
}