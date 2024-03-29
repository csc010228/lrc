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
Arm_asm_optimizer::Arm_asm_optimizer(bool optimize):Asm_optimizer(optimize)
{

}

/*
析构函数
*/
Arm_asm_optimizer::~Arm_asm_optimizer()
{

}

/*
利用arm数据处理指令能够更改标志位进行优化

Parameters
----------
basic_block:要优化的基本块
*/
void Arm_asm_optimizer::data_process_instructions_change_flags(struct arm_basic_block * basic_block)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    Arm_instruction * instruction;
    Arm_cpu_instruction * cpu_instruction;
    Arm_cpu_data_process_instruction * cpu_data_process_instruction;
    Arm_cpu_branch_instruction * cpu_branch_instruction;
    struct operand2 op2;
    struct arm_registers source_regs,destination_regs;
    reg_index reg;
    string label_name;
    map<reg_index,list<Arm_asm_file_line * >::reverse_iterator> compare_zero_regs;
    for(list<Arm_asm_file_line * >::reverse_iterator line=basic_block->arm_sequence.rbegin();line!=basic_block->arm_sequence.rend();line++)
    {
        if((*line)->is_instruction())
        {
            instruction=dynamic_cast<Arm_instruction *>(*line);
            if(instruction->is_cpu_instruction())
            {
                cpu_instruction=dynamic_cast<Arm_cpu_instruction * >(instruction);
                if(cpu_instruction->is_branch_instruction())
                {
                    cpu_branch_instruction=dynamic_cast<Arm_cpu_branch_instruction *>(cpu_instruction);
                    label_name=cpu_branch_instruction->get_label().name_;
                    if(symbol_table->func_entry(label_name)!=nullptr || notify(event(event_type::IS_AN_ABI_FUNC,(void *)(&label_name))).bool_data)
                    {
                        //函数调用的时候不能保证标志位寄存器是不变的
                        break;
                    }
                }
                else if(cpu_instruction->is_data_process_instruction())
                {
                    cpu_data_process_instruction=dynamic_cast<Arm_cpu_data_process_instruction *>(cpu_instruction);
                    op2=cpu_data_process_instruction->get_operand2();
                    switch(cpu_data_process_instruction->get_op())
                    {
                        case arm_op::CMP:
                        case arm_op::CMN:
                        case arm_op::TST:
                        case arm_op::TEQ:
                            source_regs=cpu_data_process_instruction->get_source_registers();
                            if(op2.type==operand2_type::IMMED_8R && op2.immed_8r==0 && source_regs.get_regs_num()==1)
                            {
                                compare_zero_regs.insert(make_pair(source_regs.get_only_member(),line));
                            }
                            break;
                        default:
                            destination_regs=cpu_data_process_instruction->get_destination_registers();
                            if(destination_regs.get_regs_num()==1)
                            {
                                reg=destination_regs.get_only_member();
                                if(compare_zero_regs.find(reg)!=compare_zero_regs.end())
                                {
                                    if(cpu_data_process_instruction->get_cond()==arm_condition::NONE)
                                    {
                                        cpu_data_process_instruction->set_update_flags(true);
                                        (*compare_zero_regs.at(reg))=new Arm_pseudo_instruction();
                                    }
                                    compare_zero_regs.erase(reg);
                                }
                            }
                            break;
                    }
                }
                else if(cpu_instruction->is_multiple_registers_load_and_store_instruction() || cpu_instruction->is_single_register_load_and_store_instruction())
                {
                    destination_regs=cpu_instruction->get_destination_registers();
                    for(auto des_reg:destination_regs.registers_)
                    {
                        if(compare_zero_regs.find(des_reg)!=compare_zero_regs.end())
                        {
                            compare_zero_regs.erase(des_reg);
                        }
                    }
                }
            }      
        }
    }
}

/*
删除无用mov指令

Parameters
----------
basic_bloc:要优化的基本块
*/
void Arm_asm_optimizer::remove_useless_mov(struct arm_basic_block * basic_block)
{
    Arm_instruction * ins;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    struct operand2 op2;
    for(list<Arm_asm_file_line * >::iterator line=basic_block->arm_sequence.begin();line!=basic_block->arm_sequence.end();line++)
    {
        if((*line)->is_instruction())
        {
            ins=dynamic_cast<Arm_instruction * >(*line);
            if(ins->get_op()==arm_op::MOV && ins->get_cond()==arm_condition::NONE)
            {
                cpu_data_process_ins=dynamic_cast<Arm_cpu_data_process_instruction * >(ins);
                if(!cpu_data_process_ins->is_update_flags() && cpu_data_process_ins->get_destination_registers().registers_.size()==1)
                {
                    op2=cpu_data_process_ins->get_operand2();
                    if(op2.type==operand2_type::RM_SHIFT && op2.Rm_shift.shift_op==operand2_shift_op::NONE && op2.Rm_shift.Rm==(*(cpu_data_process_ins->get_destination_registers().registers_.begin())))
                    {
                        delete (*line);
                        (*line)=new Arm_pseudo_instruction();
                    }
                }
            }
        }
    }
}

/*
局部优化
*/
void Arm_asm_optimizer::local_optimize()
{
    for(auto func:arm_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            //利用arm数据处理指令能够更改标志位进行优化
            data_process_instructions_change_flags(basic_block);
        }
    }
    for(auto func:arm_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            //将删除无用的mov
            remove_useless_mov(basic_block);
        }
    }
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
    set<Arm_cpu_data_process_instruction * > add_sps;
    Arm_cpu_data_process_instruction * cpu_data_process_instruction,* assign_fp=nullptr,* sub_sp=nullptr,* add_sp=nullptr;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_instruction;
    Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_instruction;
    Arm_instruction * instruction;
    Arm_pseudo_instruction * pseudo_instruction;
    set<reg_index> used_regs;
    list<reg_index> used_cpu_temp_regs,used_vfp_temp_regs,push_cpu_regs,pop_cpu_regs;
    int fp_sp_deviation=0;
    reg_index pc=(reg_index)notify(event(event_type::GET_PC_REG,nullptr)).int_data,sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data,lr=(reg_index)notify(event(event_type::GET_LR_REG,nullptr)).int_data,fp=(reg_index)notify(event(event_type::GET_FP_REG,nullptr)).int_data;
    bool tag=false;
    size_t push_cpu_regs_bytes=0,used_vfp_temp_regs_bytes=0,f_params_push_regs_bytes=0;
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
                            add_sps.insert(add_sp);
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
                    case arm_op::ASR:
                    case arm_op::LSL:
                    case arm_op::LSR:
                    case arm_op::ROR:
                    case arm_op::RRX:
                    case arm_op::CMP:
                    case arm_op::CMN:
                    case arm_op::TST:
                    case arm_op::TEQ:
                    case arm_op::MOV:
                    case arm_op::MVN:
                        cpu_data_process_instruction=dynamic_cast<Arm_cpu_data_process_instruction *>(instruction);
                        if(sub_sp==nullptr && instruction->get_op()==arm_op::SUB && cpu_data_process_instruction->get_destination_registers().get_only_member()==sp && cpu_data_process_instruction->get_source_registers().get_only_member()==sp)
                        {
                            sub_sp=cpu_data_process_instruction;
                            goto next;
                        }
                        if(assign_fp==nullptr && instruction->get_op()==arm_op::ADD && cpu_data_process_instruction->get_destination_registers().get_only_member()==fp && cpu_data_process_instruction->get_source_registers().get_only_member()==sp)
                        {
                            assign_fp=cpu_data_process_instruction;
                            goto next;
                        }
                        if(instruction->get_op()==arm_op::ADD && cpu_data_process_instruction->get_destination_registers().get_only_member()==sp && cpu_data_process_instruction->get_source_registers().get_only_member()==sp)
                        {
                            add_sp=cpu_data_process_instruction;
                            goto next;
                        }
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
    pop_cpu_regs=used_cpu_temp_regs;
    push_cpu_regs.push_back(fp);
    pop_cpu_regs.push_back(fp);
    push_cpu_regs.push_back(lr);
    pop_cpu_regs.push_back(pc);
    //这里要求所有的函数都是8bytes栈对齐的
    for(auto reg:push_cpu_regs)
    {
        push_cpu_regs_bytes+=notify(event(event_type::GET_REG_BYTE_SIZE,(int)reg)).int_data;
    }
    for(auto reg:used_vfp_temp_regs)
    {
        used_vfp_temp_regs_bytes+=notify(event(event_type::GET_REG_BYTE_SIZE,(int)reg)).int_data;
    }
    if((push_cpu_regs_bytes+used_vfp_temp_regs_bytes)%8!=0)
    {
        sub_sp->change_operand2_to_immed_8r(8-((push_cpu_regs_bytes+used_vfp_temp_regs_bytes)%8));
    }
    else
    {
        sub_sp->change_operand2_to_immed_8r(0);
    }
    for(auto add_sp_for_padding:add_sps)
    {
        add_sp_for_padding->change_operand2_to_immed_8r(sub_sp->get_operand2().immed_8r);
    }
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
                delete *arm_asm;
                fp_sp_deviation+=push_cpu_regs_bytes;
                fp_sp_deviation+=used_vfp_temp_regs_bytes;
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

extern map<enum arm_condition,string> condition_output_map;
extern map<enum arm_condition,enum arm_condition> condition_antonym_map;

/*
利用arm的条件执行进行优化
*/
void Arm_asm_optimizer::conditional_execute(struct arm_func_flow_graph * func)
{
    list<struct arm_basic_block * >::iterator pre,next;
    Arm_instruction * ins;
    Arm_pseudo_instruction * pse_ins;
    Arm_cpu_instruction * cpu_ins;
    Arm_cpu_branch_instruction * cpu_branch_ins;
    size_t ins_num;
    string cond_string;
    enum arm_condition cond;
    for(list<struct arm_basic_block * >::iterator current=func->basic_blocks.begin();current!=func->basic_blocks.end();current++)
    {
        if(current!=func->basic_blocks.begin())
        {
            next=current;
            advance(next,1);
            if(next!=func->basic_blocks.end())
            {
                pre=current;
                advance(pre,-1);
                if((*pre)->sequential_next!=(*current) || (*pre)->jump_next!=(*next) || (*current)->get_successors().size()==2)
                {
                    continue;
                }
                ins_num=0;
                cpu_branch_ins=nullptr;
                for(auto line:(*current)->arm_sequence)
                {
                    if(line->is_pseudo_instruction())
                    {
                        pse_ins=dynamic_cast<Arm_pseudo_instruction * >(line);
                        if(pse_ins->get_op()==arm_pseudo_op::NOP)
                        {
                            continue;
                        }
                        else
                        {
                            goto next_bb;
                        }
                    }
                    else if(line->is_directive())
                    {
                        goto next_bb;
                    }
                    else if(line->is_instruction())
                    {
                        if(cpu_branch_ins)
                        {
                            goto next_bb;
                        }
                        ins=dynamic_cast<Arm_instruction * >(line);
                        if(ins->is_cpu_instruction())
                        {
                            cpu_ins=dynamic_cast<Arm_cpu_instruction * >(line);
                            if(cpu_ins->is_branch_instruction())
                            {
                                cpu_branch_ins=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_ins);
                            }
                        }
                        if(ins->get_cond()==arm_condition::NONE)
                        {
                            ins_num++;
                        }
                        else
                        {
                            goto next_bb;
                        }
                    }
                }
                if(ins_num>2)
                {
                    break;
                }
                cond=arm_condition::NONE;
                for(list<Arm_asm_file_line * >::reverse_iterator line=(*pre)->arm_sequence.rbegin();line!=(*pre)->arm_sequence.rend();line++)
                {
                    if((*line)->is_instruction())
                    {
                        ins=dynamic_cast<Arm_instruction * >(*line);
                        if(ins->is_cpu_instruction())
                        {
                            cpu_ins=dynamic_cast<Arm_cpu_instruction * >(ins);
                            if(cpu_ins->is_branch_instruction())
                            {
                                cpu_branch_ins=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_ins);
                                cond=condition_antonym_map.at(cpu_branch_ins->get_cond());
                                // if(cond==arm_condition::NONE)
                                // {
                                //     goto next_bb;
                                // }
                                delete (*line);
                                (*line)=new Arm_pseudo_instruction();
                                break;
                            }
                        }
                    }
                }
                // if(cond==arm_condition::NONE)
                // {
                //     goto next_bb;
                // }
                for(auto line:(*current)->arm_sequence)
                {
                    if(line->is_instruction())
                    {
                        ins=dynamic_cast<Arm_instruction * >(line);
                        ins->set_cond(cond);
                    }
                }
                cond_string=condition_output_map.at(cond);
                switch(ins_num)
                {
                    case 1:
                        (*current)->arm_sequence.push_front(new Arm_pseudo_instruction(cond_string));
                        break;
                    case 2:
                        (*current)->arm_sequence.push_front(new Arm_pseudo_instruction(cond_string,"t"));
                        break;
                    case 3:
                        (*current)->arm_sequence.push_front(new Arm_pseudo_instruction(cond_string,"t","t"));
                        break;
                    case 4:
                        (*current)->arm_sequence.push_front(new Arm_pseudo_instruction(cond_string,"t","t","t"));
                        break;
                    default:
                        break;
                }
            }
        }
next_bb:
        ;
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
        if(optimize_)
        {
            //使用arm的条件执行机制
            conditional_execute(func);
        }
    }
}

/*
进行arm汇编优化

Parameters
----------
arm_flow_graph:要优化的arm汇编代码流图
*/
void Arm_asm_optimizer::handle_OPTIMIZE_ASM(struct arm_flow_graph * arm_flow_graph)
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
        case event_type::OPTIMIZE_ASM:
            handle_OPTIMIZE_ASM((struct arm_flow_graph *)event.pointer_data);
            break;
        default:
            break;
    }
    return res;
}