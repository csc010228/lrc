/*
 *
 *  graph_coloring_register_manager.cpp
 *  图着色寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/20
 *
*/
#include "graph_coloring_register_manager.h"
#include "util.h"

//===================================== struct live_analysis =====================================//

void live_analysis::clear()
{
    basic_block_s_live_analysis_infos.clear();
}

//==========================================================================//



//===================================== struct virutal_reg_s_live_interval =====================================//

virutal_reg_s_live_interval::virutal_reg_s_live_interval():is_extending(false),score(0)
{

}

void virutal_reg_s_live_interval::start(virtual_target_code_pos start_pos,virtual_target_code_pos end_pos)
{
    is_extending=true;
    live_interval.push_front(make_pair(start_pos,end_pos));
}

void virutal_reg_s_live_interval::cut_off(virtual_target_code_pos pos)
{
    if(is_extending)
    {
        live_interval.front().first=pos;
    }
    else
    {
        start(pos,pos+1);
    }
    is_extending=false;
}

void virutal_reg_s_live_interval::extend(virtual_target_code_pos pos)
{
    live_interval.front().first=pos;
}

void virutal_reg_s_live_interval::add_use_pos(struct arm_basic_block * bb,virtual_target_code_pos pos)
{
    map_set_insert(use_poses,bb,pos);
}

void virutal_reg_s_live_interval::add_def_pos(struct arm_basic_block * bb,virtual_target_code_pos pos)
{
    map_set_insert(def_poses,bb,pos);
}

size_t virutal_reg_s_live_interval::get_score()
{
    if(score==0)
    {
        for(auto i:use_poses)
        {
            // if(score<i.first->loop_count)
            // {
            //     score=i.first->loop_count;
            // }
            if(score<i.first->loop_count*100)
            {
                score=i.first->loop_count*100+i.second.size();
                if(def_poses.find(i.first)!=def_poses.end())
                {
                    score+=def_poses.at(i.first).size();
                }
            }
        }
        for(auto i:def_poses)
        {
            // if(score<i.first->loop_count)
            // {
            //     score=i.first->loop_count;
            // }
            if(score<i.first->loop_count*100)
            {
                score=i.first->loop_count*100+i.second.size();
                if(use_poses.find(i.first)!=use_poses.end())
                {
                    score+=use_poses.at(i.first).size();
                }
            }
        }
    }
    return score;
}

//==========================================================================//



//===================================== struct live_intervals =====================================//

void live_intervals::clear()
{
    virtual_regs_s_live_intervals.clear();
}

void live_intervals::new_empty_virtual_code_segment(virtual_target_code_pos pos,size_t add_size)
{
    for(auto reg_s_live_interval:virtual_regs_s_live_intervals)
    {
        for(auto & live_interval_segment:reg_s_live_interval.second.live_interval)
        {
            if(live_interval_segment.first>=pos)
            {
                live_interval_segment.first+=add_size;
            }
            if(live_interval_segment.second>pos)
            {
                live_interval_segment.second+=add_size;
            }
        }
        // for(auto & use_bb_and_pos:reg_s_live_interval.second.use_poses)
        // {
        //     for(auto & use_pos:use_bb_and_pos.second)
        //     {
        //         if(use_pos>=pos)
        //         {
        //             use_pos+=pos;
        //         }
        //     }
        // }
        // for(auto & def_bb_and_pos:reg_s_live_interval.second.def_poses)
        // {
        //     for(auto & def_pos:def_bb_and_pos.second)
        //     {
        //         if(def_pos>=pos)
        //         {
        //             def_pos+=pos;
        //         }
        //     }
        // }
    }
}

//==========================================================================//



//===================================== struct coherent_diagram_node =====================================//

coherent_diagram_node::coherent_diagram_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type,struct virutal_reg_s_live_interval live_interval):reg(reg),degree(0),live_interval(live_interval),data_type(data_type)
{

}

void coherent_diagram_node::add_a_move_related_neighbour(struct coherent_diagram_node * node)
{
    //TO-DO
}

void coherent_diagram_node::add_a_collision_neighbour(struct coherent_diagram_node * node)
{
    if(collision_nodes.find(node)==collision_nodes.end())
    {
        collision_nodes.insert(node);
        degree++;
    }
}

size_t coherent_diagram_node::get_score()
{
    return collision_nodes.size();
}

//==========================================================================//



//===================================== struct coherent_diagram =====================================//

coherent_diagram::~coherent_diagram()
{
    clear();
}

void coherent_diagram::clear()
{
    for(auto node:nodes)
    {
        delete node.second;
    }
    nodes.clear();
}

struct coherent_diagram_node * coherent_diagram::new_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type)
{
    struct coherent_diagram_node * res=nullptr;
    if(nodes.find(reg)==nodes.end())
    {
        res=new coherent_diagram_node(reg,data_type,current_func_s_live_intervals.virtual_regs_s_live_intervals.at(reg));
        nodes.insert(make_pair(reg,res));
    }
    return res;
}

struct coherent_diagram_node * coherent_diagram::get_coherent_diagram_node(reg_index reg)
{
    return nodes.at(reg);
}

void coherent_diagram::add_move_related(reg_index reg_1,reg_index reg_2)
{
    //TO-DO
}

void coherent_diagram::add_collision(reg_index reg_1,reg_index reg_2)
{
    struct coherent_diagram_node * node_1,* node_2;
    node_1=get_coherent_diagram_node(reg_1);
    node_2=get_coherent_diagram_node(reg_2);
    node_1->add_a_collision_neighbour(node_2);
    node_2->add_a_collision_neighbour(node_1);
}

//==========================================================================//



//===================================== class Graph_coloring_register_manager =====================================//

Graph_coloring_register_manager::Graph_coloring_register_manager(set<struct reg> regs,struct flag_reg flag_reg):Global_register_manager(regs,flag_reg),current_processor(reg_processor::CPU)
{
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.processor==reg_processor::CPU && (reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP))
        {
            available_physical_cpu_regs.insert(reg.first);
        }
    }
    available_physical_cpu_reg_num=available_physical_cpu_regs.size();
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.processor==reg_processor::VFP && (reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP))
        {
            available_physical_vfp_regs.insert(reg.first);
        }
    }
    available_physical_vfp_reg_num=available_physical_vfp_regs.size();
}

Graph_coloring_register_manager::~Graph_coloring_register_manager()
{

}

set<reg_index> Graph_coloring_register_manager::get_virtual_traget_instruction_s_all_source_regs(Arm_asm_file_line * line)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    set<reg_index> res;
    Arm_instruction * ins;
    Arm_pseudo_instruction * pseudo_ins;
    Arm_cpu_instruction * cpu_ins;
    Arm_cpu_branch_instruction * cpu_branch_ins;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins;
    Arm_cpu_multiple_registers_load_and_store_instruction * cpu_multiple_registers_load_and_store_ins;
    Arm_vfp_instruction * vfp_ins;
    Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_ins;
    Arm_vfp_multiple_registers_load_and_store_instruction * vfp_multiple_registers_load_and_store_ins;
    list<reg_index> regs;
    struct ic_func * func;
    struct operand2 op2;
    struct flexoffset fleoff;
    string func_name;
    if(line->is_instruction())
    {
        ins=dynamic_cast<Arm_instruction *>(line);
        if(ins->is_cpu_instruction())
        {
            cpu_ins=dynamic_cast<Arm_cpu_instruction *>(ins);
            if(cpu_ins->is_branch_instruction())
            {
                cpu_branch_ins=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_ins);
                func_name=cpu_branch_ins->get_label().name_;
                regs=get_func_s_regs_holding_f_params_by_func_name(func_name);
                for(auto reg:regs)
                {
                    res.insert(reg);
                }
            }
            else if(cpu_ins->is_data_process_instruction())
            {
                cpu_data_process_ins=dynamic_cast<Arm_cpu_data_process_instruction *>(cpu_ins);
                if(cpu_data_process_ins->get_data_type()==arm_data_process_instruction_data_type::OPERNAD2)
                {
                    op2=cpu_data_process_ins->get_operand2();
                    if(op2.type==operand2_type::RM_SHIFT)
                    {
                        res.insert(op2.Rm_shift.Rm);
                        if(op2.Rm_shift.shift_op==operand2_shift_op::ASR_RS || 
                        op2.Rm_shift.shift_op==operand2_shift_op::LSL_RS ||
                        op2.Rm_shift.shift_op==operand2_shift_op::LSR_RS ||
                        op2.Rm_shift.shift_op==operand2_shift_op::ROR_RS)
                        {
                            res.insert(op2.Rm_shift.Rs);
                        }
                    }
                }
            }
            else if(cpu_ins->is_single_register_load_and_store_instruction())
            {
                cpu_single_register_load_and_store_ins=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(cpu_ins);
                fleoff=cpu_single_register_load_and_store_ins->get_flexoffset();
                if(fleoff.type==flexoffset_type::RM_SHIFT)
                {
                    res.insert(fleoff.Rm_shift.Rm);
                }
                if(cpu_single_register_load_and_store_ins->is_store())
                {
                    for(auto reg:ins->get_destination_registers().registers_)
                    {
                        res.insert(reg);
                    }
                }
            }
            else if(cpu_ins->is_multiple_registers_load_and_store_instruction())
            {
                cpu_multiple_registers_load_and_store_ins=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(cpu_ins);
                if(cpu_multiple_registers_load_and_store_ins->is_store())
                {
                    for(auto reg:ins->get_destination_registers().registers_)
                    {
                        res.insert(reg);
                    }
                }
            }
        }
        else if(ins->is_vfp_instruction())
        {
            vfp_ins=dynamic_cast<Arm_vfp_instruction * >(ins);
            if(vfp_ins->is_single_register_load_and_store_instruction())
            {
                vfp_single_register_load_and_store_ins=dynamic_cast<Arm_vfp_single_register_load_and_store_instruction *>(vfp_ins);
                if(vfp_single_register_load_and_store_ins->is_store())
                {
                    for(auto reg:ins->get_destination_registers().registers_)
                    {
                        res.insert(reg);
                    }
                }
            }
            else if(vfp_ins->is_multiple_registers_load_and_store_instruction())
            {
                vfp_multiple_registers_load_and_store_ins=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(vfp_ins);
                if(vfp_multiple_registers_load_and_store_ins->is_store())
                {
                    for(auto reg:ins->get_destination_registers().registers_)
                    {
                        res.insert(reg);
                    }
                }
            }
        }
        for(auto reg:ins->get_source_registers().registers_)
        {
            res.insert(reg);
        }
    }
    return res;
}

set<reg_index> Graph_coloring_register_manager::get_virtual_traget_instruction_s_all_destination_regs(Arm_asm_file_line * line)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    set<reg_index> res;
    Arm_instruction * ins;
    Arm_cpu_instruction * cpu_ins;
    Arm_cpu_branch_instruction * cpu_branch_ins;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins;
    Arm_cpu_multiple_registers_load_and_store_instruction * cpu_multiple_registers_load_and_store_ins;
    Arm_vfp_instruction * vfp_ins;
    Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_ins;
    Arm_vfp_multiple_registers_load_and_store_instruction * vfp_multiple_registers_load_and_store_ins;
    Arm_pseudo_instruction * pseudo_ins;
    list<reg_index> registers;
    string func_name;
    if(line->is_instruction())
    {
        ins=dynamic_cast<Arm_instruction *>(line);
        if(ins->is_cpu_instruction())
        {
            cpu_ins=dynamic_cast<Arm_cpu_instruction *>(ins);
            if(cpu_ins->is_branch_instruction())
            {
                cpu_branch_ins=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_ins);
                func_name=cpu_branch_ins->get_label().name_;
                if(notify(event(event_type::IS_AN_ABI_FUNC,(void *)(&func_name))).bool_data || symbol_table->is_a_defined_or_library_func(func_name))
                {
                    for(auto reg_info:regs_info_.reg_indexs)
                    {
                        if(reg_info.second.attr==reg_attr::ARGUMENT && reg_info.second.processor==current_processor)
                        {
                            res.insert(reg_info.first);
                        }
                    }
                }
            }
            else if(cpu_ins->is_single_register_load_and_store_instruction())
            {
                cpu_single_register_load_and_store_ins=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(cpu_ins);
                if(cpu_single_register_load_and_store_ins->is_store())
                {
                    return res;
                }
            }
            else if(cpu_ins->is_multiple_registers_load_and_store_instruction())
            {
                cpu_multiple_registers_load_and_store_ins=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(cpu_ins);
                if(cpu_multiple_registers_load_and_store_ins->is_store())
                {
                    return res;
                }
            }
        }
        else if(ins->is_vfp_instruction())
        {
            vfp_ins=dynamic_cast<Arm_vfp_instruction *>(ins);
            if(vfp_ins->is_single_register_load_and_store_instruction())
            {
                vfp_single_register_load_and_store_ins=dynamic_cast<Arm_vfp_single_register_load_and_store_instruction *>(vfp_ins);
                if(vfp_single_register_load_and_store_ins->is_store())
                {
                    return res;
                }
            }
            else if(vfp_ins->is_multiple_registers_load_and_store_instruction())
            {
                vfp_multiple_registers_load_and_store_ins=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(vfp_ins);
                if(vfp_multiple_registers_load_and_store_ins->is_store())
                {
                    return res;
                }
            }
        }
        for(auto reg:ins->get_destination_registers().registers_)
        {
            res.insert(reg);
        }
    }
    else if(line->is_pseudo_instruction())
    {
        pseudo_ins=dynamic_cast<Arm_pseudo_instruction *>(line);
        if(pseudo_ins->get_op()==arm_pseudo_op::ADR || 
        pseudo_ins->get_op()==arm_pseudo_op::ADRL ||
        pseudo_ins->get_op()==arm_pseudo_op::LDR || 
        pseudo_ins->get_op()==arm_pseudo_op::VLDR)
        {
            res.insert(pseudo_ins->get_reg());
        }
    }
    return res;
}
#include<iostream>
using namespace std;
set<reg_index> Graph_coloring_register_manager::get_virtual_traget_instruction_s_all_regs(Arm_asm_file_line * line)
{
    set<reg_index> source_regs,destination_regs,res;
    source_regs=get_virtual_traget_instruction_s_all_source_regs(line);
    destination_regs=get_virtual_traget_instruction_s_all_destination_regs(line);
    set_union(source_regs.begin(),source_regs.end(),destination_regs.begin(),destination_regs.end(),inserter(res,res.begin()));
    return res;
}

void Graph_coloring_register_manager::live_analyze()
{
    set<reg_index> registers,old_out;
    set<struct arm_basic_block * > successors;
    bool tag=true;
    virtual_target_code_pos pos=1;
    //首先计算每一个基本块的live_def和live_use
    basic_block_s_interval.clear();
    for(auto bb:virtual_target_code->basic_blocks)
    {
        basic_block_s_interval.insert(make_pair(bb,make_pair(pos,0)));
        current_func_s_live_analysis.basic_block_s_live_analysis_infos.insert(make_pair(bb,basic_block_s_live_analysis()));
        for(auto virtual_target_instruction:bb->arm_sequence)
        {
            registers=get_virtual_traget_instruction_s_all_source_regs(virtual_target_instruction);
            for(auto reg:registers)
            {
                if(virtual_regs_info_.is_physical_reg(reg) && !regs_info_.is_ARGUMENT_REG(reg))
                {
                    continue;
                }
                if(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_def.find(reg)==current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_def.end())
                {
                    current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_use.insert(reg);
                }
            }
            registers=get_virtual_traget_instruction_s_all_destination_regs(virtual_target_instruction);
            for(auto reg:registers)
            {
                if(virtual_regs_info_.is_physical_reg(reg) && !regs_info_.is_ARGUMENT_REG(reg))
                {
                    continue;
                }
                current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_def.insert(reg);
            }
            pos++;
        }
        basic_block_s_interval.at(bb).second=pos;
    }
    //再迭代计算每一个基本块的live_in和live_out
    for(auto bb:virtual_target_code->basic_blocks)
    {
        current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in=current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_use;
    }
    while(tag)
    {
        tag=false;
        for(auto bb:virtual_target_code->basic_blocks)
        {
            old_out=current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out;
            current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.clear();
            successors=bb->get_successors();
            for(auto successor:successors)
            {
                set_union(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(successor).live_in.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(successor).live_in.end(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.end(),inserter(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out,current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.begin()));
            }
            current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in.clear();
            set_difference(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out.end(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_def.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_def.end(),inserter(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in,current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in.begin()));
            set_union(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_use.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_use.end(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in.begin(),current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in.end(),inserter(current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in,current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_in.begin()));
            if(old_out!=current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(bb).live_out)
            {
                tag=true;
            }
        }
    }
    // cout<<"live_analysis:"<<endl;
    // for(auto i:virtual_target_code->basic_blocks)
    // {
    //     cout<<"basic_block "<<i<<endl;
    //     cout<<"live def:";
    //     for(auto j:current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(i).live_def)
    //     {
    //         cout<<(virtual_regs_info_.is_physical_reg(j)?"r":"VR")<<j<<",";
    //     }
    //     cout<<endl;
    //     cout<<"live use:";
    //     for(auto j:current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(i).live_use)
    //     {
    //         cout<<(virtual_regs_info_.is_physical_reg(j)?"r":"VR")<<j<<",";
    //     }
    //     cout<<endl;
    //     cout<<"live in:";
    //     for(auto j:current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(i).live_in)
    //     {
    //         cout<<(virtual_regs_info_.is_physical_reg(j)?"r":"VR")<<j<<",";
    //     }
    //     cout<<endl;
    //     cout<<"live out:";
    //     for(auto j:current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(i).live_out)
    //     {
    //         cout<<(virtual_regs_info_.is_physical_reg(j)?"r":"VR")<<j<<",";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
}

void Graph_coloring_register_manager::build_live_intervals()
{
    //先进行活跃虚拟寄存器分析
    live_analyze();
    reg_index return_reg;
    bool has_return_value=true;
    set<reg_index> regs,live_out;
    list<reg_index> * f_param_regs;
    set<struct arm_basic_block * > successors;
    virtual_target_code_pos bb_start,bb_end;
    //虚拟目标代码的编号从1开始
    virtual_target_code_pos current_pos=virtual_target_code->get_line_num(),block_s_max_pos;
    //逆序遍历基本块
    for(auto bb=virtual_target_code->basic_blocks.rbegin();bb!=virtual_target_code->basic_blocks.rend();bb++)
    {
        block_s_max_pos=current_pos;
        //对基本块的live_out里面的元素进行操作
        live_out=current_func_s_live_analysis.basic_block_s_live_analysis_infos.at(*bb).live_out;
        for(auto reg:live_out)
        {
            if(current_func_s_live_intervals.get_reg_s_live_interval(reg).is_extending)
            {
                //延长range
                current_func_s_live_intervals.get_reg_s_live_interval(reg).extend(block_s_max_pos);
            }
            else
            {
                //新增range
                current_func_s_live_intervals.get_reg_s_live_interval(reg).start(block_s_max_pos,block_s_max_pos+1);
            }
        }
        //对那些不在该基本块的live_out中，而且有着未截断的range到达该基本块的末尾的虚拟寄存器进行处理
        for(auto & reg_and_live_interval:current_func_s_live_intervals.virtual_regs_s_live_intervals)
        {
            if(reg_and_live_interval.second.is_extending && live_out.find(reg_and_live_interval.first)==live_out.end())
            {
                reg_and_live_interval.second.cut_off(block_s_max_pos);
            }
        }
        //逆序遍历该基本块中的指令，对这些指令进行处理
        for(auto line=(*bb)->arm_sequence.rbegin();line!=(*bb)->arm_sequence.rend();line++)
        {
            regs=get_virtual_traget_instruction_s_all_destination_regs(*line);
            //处理目的寄存器
            for(auto reg:regs)
            {
                if(virtual_regs_info_.is_physical_reg(reg) && !regs_info_.is_ARGUMENT_REG(reg))
                {
                    continue;
                }
                current_func_s_live_intervals.get_reg_s_live_interval(reg).add_def_pos(*bb,current_pos);
                //截断
                current_func_s_live_intervals.get_reg_s_live_interval(reg).cut_off(current_pos);   
            }
            //处理源寄存器
            regs=get_virtual_traget_instruction_s_all_source_regs(*line);
            for(auto reg:regs)
            {
                if(virtual_regs_info_.is_physical_reg(reg) && !regs_info_.is_ARGUMENT_REG(reg))
                {
                    continue;
                }
                current_func_s_live_intervals.get_reg_s_live_interval(reg).add_use_pos(*bb,current_pos);
                if(current_func_s_live_intervals.get_reg_s_live_interval(reg).is_extending)
                {
                    //延长range
                    current_func_s_live_intervals.get_reg_s_live_interval(reg).extend(current_pos-1);
                }
                else
                {
                    //新增range
                    current_func_s_live_intervals.get_reg_s_live_interval(reg).start(current_pos-1,current_pos);
                }
            }
            current_pos--;
        }
    }
    //在最后，需要对那些存放函数参数的寄存器进行特殊处理
    switch(current_processor)
    {
        case reg_processor::CPU:
            f_param_regs=(list<reg_index> *)notify(event(event_type::GET_CPU_REGS_HOLDING_F_PARAMS_WHEN_ENTERING_FUNC,(void *)virtual_target_code->function->func)).pointer_data;
            break;
        case reg_processor::VFP:
            f_param_regs=(list<reg_index> *)notify(event(event_type::GET_VFP_REGS_HOLDING_F_PARAMS_WHEN_ENTERING_FUNC,(void *)virtual_target_code->function->func)).pointer_data;
            break;
        default:
            break;
    }
    for(auto reg:*f_param_regs)
    {
        if(current_func_s_live_intervals.virtual_regs_s_live_intervals.find(reg)!=current_func_s_live_intervals.virtual_regs_s_live_intervals.end() && !current_func_s_live_intervals.virtual_regs_s_live_intervals.at(reg).live_interval.empty())
        {
            current_func_s_live_intervals.virtual_regs_s_live_intervals.at(reg).live_interval.front().first=1;
        }
    }
    delete f_param_regs;
    //同时需要对存放函数返回值的寄存器进行特殊处理
    switch(virtual_target_code->function->func->return_type)
    {
        case language_data_type::INT:
            return_reg=regs_info_.reg_names.at("r0");
            break;
        case language_data_type::FLOAT:
            return_reg=regs_info_.reg_names.at("s0");
            break;
        default:
            has_return_value=false;
            break;
    }
    if(has_return_value)
    {
        for(auto bb:virtual_target_code->basic_blocks)
        {
            //对那些具有函数返回语句的基本块进行处理
            //暂时可以认为那些没有后继的基本块就是具有函数返回语句的基本块（实际上并不是）
            successors=bb->get_successors();
            if(successors.empty())
            {
                bb_start=basic_block_s_interval.at(bb).first;
                bb_end=basic_block_s_interval.at(bb).second;
                list<pair<virtual_target_code_pos,virtual_target_code_pos> > & return_reg_s_live_interval=current_func_s_live_intervals.virtual_regs_s_live_intervals.at(return_reg).live_interval;
                for(list<pair<virtual_target_code_pos,virtual_target_code_pos> >::reverse_iterator li=return_reg_s_live_interval.rbegin();li!=return_reg_s_live_interval.rend();li++)
                {
                    if((*li).first>=bb_start && (*li).first<=bb_end && (*li).second>=bb_start && (*li).second<=bb_end)
                    {
                        li->second=bb_end;
                        break;
                    }
                }
            }
        }
    }
    // cout<<"live interval:"<<endl;
    // for(auto i:current_func_s_live_intervals.virtual_regs_s_live_intervals)
    // {
    //     cout<<(virtual_regs_info_.is_physical_reg(i.first)?"r":"VR")<<i.first<<":";
    //     for(auto j:i.second.live_interval)
    //     {
    //         cout<<"("<<j.first<<"-"<<j.second<<")->";
    //     }
    //     cout<<endl;
    // }
    // cout<<endl;
}

void Graph_coloring_register_manager::build_coherent_diagram()
{
    //首先构建live_intervals
    build_live_intervals();
    current_func_s_coherent_diagram=coherent_diagram(current_func_s_live_intervals);
    //已经检查过的寄存器
    map<reg_index,set<reg_index> > regs_checked_1,regs_checked_2;
    virtual_target_code_pos min_start,max_end;
    enum coherent_diagram_node_s_data_type data_type;
    //先建立相干图中的所有点
    for(auto live_interval_info:current_func_s_live_intervals.virtual_regs_s_live_intervals)
    {
        if(!virtual_regs_info_.is_physical_reg(live_interval_info.first))
        {
            switch(virtual_regs_info_.reg_indexs.at(live_interval_info.first).data_type)
            {
                case virtual_related_data_type::CONST:
                    data_type=coherent_diagram_node_s_data_type::CONST;
                    break;
                case virtual_related_data_type::VAR_ADDR:
                    data_type=coherent_diagram_node_s_data_type::ADDR;
                    break;
                case virtual_related_data_type::VAR_VALUE:
                    if(virtual_regs_info_.reg_indexs.at(live_interval_info.first).related_var->is_array_member())
                    {
                        data_type=coherent_diagram_node_s_data_type::ARRAY_MEMBER_VALUE;
                    }
                    else
                    {
                        data_type=coherent_diagram_node_s_data_type::VALUE;
                    }
                    break;
                default:
                    break;
            }
        }
        current_func_s_coherent_diagram.new_node(live_interval_info.first,data_type);
    }
    //再开始建立相干图中的点之间的关系
    //目前只构建了冲突，还没有构建移动关联
    for(auto live_interval_info_1:current_func_s_live_intervals.virtual_regs_s_live_intervals)
    {
        for(auto live_interval_info_2:current_func_s_live_intervals.virtual_regs_s_live_intervals)
        {
            if(live_interval_info_1.first==live_interval_info_2.first || map_set_find(regs_checked_1,live_interval_info_1.first,live_interval_info_2.first) || map_set_find(regs_checked_2,live_interval_info_1.first,live_interval_info_2.first))
            {
                continue;
            }
            map_set_insert(regs_checked_1,live_interval_info_1.first,live_interval_info_2.first);
            map_set_insert(regs_checked_2,live_interval_info_2.first,live_interval_info_1.first);
            for(auto segment_of_live_interval_1:live_interval_info_1.second.live_interval)
            {
                for(auto segment_of_live_interval_2:live_interval_info_2.second.live_interval)
                {
                    if(segment_of_live_interval_1.first<segment_of_live_interval_2.first)
                    {
                        min_start=segment_of_live_interval_1.first;
                    }
                    else
                    {
                        min_start=segment_of_live_interval_2.first;
                    }
                    if(segment_of_live_interval_1.second>segment_of_live_interval_2.second)
                    {
                        max_end=segment_of_live_interval_1.second;
                    }
                    else
                    {
                        max_end=segment_of_live_interval_2.second;
                    }
                    if((max_end-min_start)<((segment_of_live_interval_1.second-segment_of_live_interval_1.first)+(segment_of_live_interval_2.second-segment_of_live_interval_2.first)))
                    {
                        //找到了冲突
                        current_func_s_coherent_diagram.add_collision(live_interval_info_1.first,live_interval_info_2.first);
                        goto next;
                    }
                }
            }
next:
            ;
        }
    }
    // cout<<"coherent_diagram:"<<endl;
    // for(auto i:current_func_s_coherent_diagram.nodes)
    // {
    //     cout<<"("<<(virtual_regs_info_.is_physical_reg(i.first)?"r":"VR")<<i.first<<","<<i.second->degree<<");";
    // }
    // cout<<endl;
    // cout<<endl;
}

void Graph_coloring_register_manager::mk_worklists()
{
    for(auto reg_s_node:current_func_s_coherent_diagram.nodes)
    {
        if(virtual_regs_info_.is_physical_reg(reg_s_node.first))
        {
            precolored.insert(reg_s_node.second);
        }
        else
        {
            initial.insert(reg_s_node.second);
        }
    }
    for(auto node:initial)
    {
        if(node->degree>=available_physical_cpu_reg_num)
        {
            spill_worklist.insert(node);
        }
        else
        {
            simplify_worklist.insert(node);
        }
    }
}

void Graph_coloring_register_manager::decrement_degree(struct coherent_diagram_node * node)
{
    node->degree--;
    if(node->degree==available_physical_cpu_reg_num && spill_worklist.find(node)!=spill_worklist.end())
    {
        spill_worklist.erase(node);
        simplify_worklist.insert(node);
    }
}

bool sort_func(struct coherent_diagram_node * node_1,struct coherent_diagram_node * node_2)
{
    //return node_1->get_score()>node_2->get_score();
    //return node_1->live_interval.get_score()<node_2->live_interval.get_score();
    if(node_1==node_2)
    {
        return false;
    }
    if(node_1->live_interval.get_score()!=node_2->live_interval.get_score())
    {
        return node_1->live_interval.get_score()<node_2->live_interval.get_score();
    }
    //return  node_1->get_score()>node_2->get_score();
    if(node_1->get_score()!=node_2->get_score())
    {
       return  node_1->get_score()>node_2->get_score();
    }
    // if(node_1->data_type==coherent_diagram_node_s_data_type::CONST || node_1->data_type==coherent_diagram_node_s_data_type::ADDR)
    // {
    //     return false;
    // }
    // if(node_2->data_type==coherent_diagram_node_s_data_type::CONST || node_2->data_type==coherent_diagram_node_s_data_type::ADDR)
    // {
    //     return true;
    // }
    // if(node_1->data_type==coherent_diagram_node_s_data_type::ARRAY_MEMBER_VALUE)
    // {
    //     return true;
    // }
    // if(node_2->data_type==coherent_diagram_node_s_data_type::ARRAY_MEMBER_VALUE)
    // {
    //     return false;
    // }
    // return false;
    return node_2->data_type==coherent_diagram_node_s_data_type::CONST || node_2->data_type==coherent_diagram_node_s_data_type::ADDR || node_1->data_type==coherent_diagram_node_s_data_type::ARRAY_MEMBER_VALUE;
}

void Graph_coloring_register_manager::simplify()
{
    vector<struct coherent_diagram_node * > temp;
    while(!simplify_worklist.empty())
    {
        //对simplify_worklist排序，把最不想溢出的节点尽量放在最前面
        for(auto node:simplify_worklist)
        {
            temp.push_back(node);
        }
        sort(temp.begin(),temp.end(),sort_func);
        for(auto node:temp)
        {
            simplify_worklist.erase(node);
            select_stack.push(node);
            for(auto neighbour:node->collision_nodes)
            {
                decrement_degree(neighbour);
            }
        }
    }
}

void Graph_coloring_register_manager::coalesce()
{
    
}

void Graph_coloring_register_manager::freeze()
{
    
}

void Graph_coloring_register_manager::select_spill()
{
    set<struct coherent_diagram_node * > temp;
    while(!spill_worklist.empty())
    {
        temp=spill_worklist;
        for(auto node:temp)
        {
            spill_worklist.erase(node);
            simplify_worklist.insert(node);
        }
    }
}

void Graph_coloring_register_manager::assign_colors()
{
    struct coherent_diagram_node * node;
    set<reg_index> available_colors;
    while(!select_stack.empty())
    {
        node=select_stack.top();
        select_stack.pop();
        available_colors=available_physical_cpu_regs;
        for(auto neighbour:node->collision_nodes)
        {
            if(precolored.find(neighbour)!=precolored.end())
            {
                available_colors.erase(neighbour->reg);
            }
            else if(colored_nodes.find(neighbour)!=colored_nodes.end())
            {
                available_colors.erase(colored_nodes.at(neighbour));
            }
        }
        if(available_colors.empty())
        {
            spilled_nodes.insert(node);
        }
        else
        {
            colored_nodes.insert(make_pair(node,*(available_colors.begin())));
        }
    }
}

void Graph_coloring_register_manager::set_registers_when_spilling_var_value_s_reg(struct arm_basic_block * bb,list<Arm_asm_file_line * >::iterator ins_pos,struct ic_data * var)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    set<struct ic_data * > func_def_globals_and_f_params;
    struct ic_func * func;
    Arm_instruction * inst;
    Arm_cpu_instruction * cpu_inst;
    Arm_cpu_branch_instruction * cpu_branch_inst;
    set<reg_index> target_regs,old_out,temp,effective_regs;
    set<struct arm_basic_block * > precursors;
    map<struct arm_basic_block *,set<reg_index> > gens,kills,ins,outs;
    struct ic_data * belong_array=nullptr,* offset=nullptr;
    bool tag=true,first_set_intersection;
    string func_name;
    if(var->is_array_member())
    {
        belong_array=var->get_belong_array();
        offset=var->get_offset();
    }
    //首先遍历一遍所有的虚拟寄存器，将其全部设置为无效，同时找出所有和该变量相关的所有的地址以及变量所在的寄存器
    for(auto reg_info:virtual_regs_info_.reg_indexs)
    {
        switch(reg_info.second.data_type)
        {
            case virtual_related_data_type::VAR_ADDR:
                if(var==reg_info.second.related_var || (offset && offset==reg_info.second.related_var))
                {
                    target_regs.insert(reg_info.first);
                }
                break;
            case virtual_related_data_type::VAR_VALUE:
                if((belong_array && belong_array==reg_info.second.related_var) || (offset && offset==reg_info.second.related_var))
                {
                    target_regs.insert(reg_info.first);
                }
                break;
            default:
                break;
        }
        virtual_regs_info_.set_reg_ineffective(reg_info.first);
    }
    //对这些目标寄存器进行无效生命周期的分析
    //首先计算gens和kills
    for(auto basic_block:virtual_target_code->basic_blocks)
    {
        gens.insert(make_pair(basic_block,set<reg_index>()));
        kills.insert(make_pair(basic_block,set<reg_index>()));
        ins.insert(make_pair(basic_block,set<reg_index>()));
        outs.insert(make_pair(basic_block,set<reg_index>()));
        for(auto line:basic_block->arm_sequence)
        {
            temp=get_virtual_traget_instruction_s_all_regs(line);
            for(auto target_reg:target_regs)
            {
                if(temp.find(target_reg)!=temp.end())
                {
                    if(gens.at(basic_block).find(target_reg)!=gens.at(basic_block).end())
                    {
                        gens.at(basic_block).erase(target_reg);
                    }
                    kills.at(basic_block).insert(target_reg);
                }
            }
            if(line->is_instruction())
            {
                inst=dynamic_cast<Arm_instruction * >(line);
                if(inst->is_cpu_instruction())
                {
                    cpu_inst=dynamic_cast<Arm_cpu_instruction * >(inst);
                    if(cpu_inst->is_branch_instruction())
                    {
                        cpu_branch_inst=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_inst);
                        func_name=cpu_branch_inst->get_label().name_;
                        if(symbol_table->is_a_defined_or_library_func(func_name))
                        {
                            func=symbol_table->func_entry(func_name);
                            func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_array_f_params(func);
                            for(auto target_reg:target_regs)
                            {
                                if(virtual_regs_info_.reg_indexs.at(target_reg).data_type==virtual_related_data_type::VAR_VALUE && 
                                virtual_regs_info_.reg_indexs.at(target_reg).related_var->is_global() && 
                                !virtual_regs_info_.reg_indexs.at(target_reg).related_var->is_array_var() && 
                                func_def_globals_and_f_params.find(virtual_regs_info_.reg_indexs.at(target_reg).related_var)!=func_def_globals_and_f_params.end())
                                {
                                    gens.at(basic_block).insert(target_reg);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //再迭代计算ins和outs
    while(tag)
    {
        tag=false;
        for(auto basic_block:virtual_target_code->basic_blocks)
        {
            precursors=basic_block->get_precursors();
            if(precursors.empty())
            {
                ins.at(basic_block)=target_regs;
            }
            else
            {
                ins.at(basic_block).clear();
                for(auto precursor:precursors)
                {
                    set_union(outs.at(precursor).begin(),outs.at(precursor).end(),ins.at(basic_block).begin(),ins.at(basic_block).end(),inserter(ins.at(basic_block),ins.at(basic_block).begin()));
                }
            }
            old_out=outs.at(basic_block);
            outs.at(basic_block).clear();
            set_difference(ins.at(basic_block).begin(),ins.at(basic_block).end(),kills.at(basic_block).begin(),kills.at(basic_block).end(),inserter(outs.at(basic_block),outs.at(basic_block).begin()));
            set_union(outs.at(basic_block).begin(),outs.at(basic_block).end(),gens.at(basic_block).begin(),gens.at(basic_block).end(),inserter(outs.at(basic_block),outs.at(basic_block).begin()));
            if(old_out!=outs.at(basic_block))
            {
                tag=true;
            }
        }
    }
    //再从当前位置所在的基本块的开始处计算到当前位置的寄存器信息
    set_difference(target_regs.begin(),target_regs.end(),ins.at(bb).begin(),ins.at(bb).end(),inserter(effective_regs,effective_regs.begin()));
    for(list<Arm_asm_file_line * >::iterator pos=bb->arm_sequence.begin();pos!=ins_pos;pos++)
    {
        temp=get_virtual_traget_instruction_s_all_regs(*pos);
        set_intersection(temp.begin(),temp.end(),target_regs.begin(),target_regs.end(),inserter(effective_regs,effective_regs.begin()));
        if((*pos)->is_instruction())
        {
            inst=dynamic_cast<Arm_instruction * >(*pos);
            if(inst->is_cpu_instruction())
            {
                cpu_inst=dynamic_cast<Arm_cpu_instruction * >(inst);
                if(cpu_inst->is_branch_instruction())
                {
                    cpu_branch_inst=dynamic_cast<Arm_cpu_branch_instruction * >(cpu_inst);
                    func_name=cpu_branch_inst->get_label().name_;
                    if(symbol_table->is_a_defined_or_library_func(func_name))
                    {
                        func=symbol_table->func_entry(func_name);
                        func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_array_f_params(func);
                        temp=effective_regs;
                        for(auto effective_reg:temp)
                        {
                            if(virtual_regs_info_.reg_indexs.at(effective_reg).data_type==virtual_related_data_type::VAR_VALUE && 
                            virtual_regs_info_.reg_indexs.at(effective_reg).related_var->is_global() && 
                            !virtual_regs_info_.reg_indexs.at(effective_reg).related_var->is_array_var() && 
                            func_def_globals_and_f_params.find(virtual_regs_info_.reg_indexs.at(effective_reg).related_var)!=func_def_globals_and_f_params.end())
                            {
                                effective_regs.erase(effective_reg);
                            }
                        }
                    }
                }
            }
        }
    }
    //最后设置有效的寄存器信息即可
    for(auto effective_reg:effective_regs)
    {
        virtual_regs_info_.set_reg_effective(effective_reg);
        switch(virtual_regs_info_.reg_indexs.at(effective_reg).data_type)
        {
            case virtual_related_data_type::CONST:
                virtual_regs_info_.const_value_to_reg_map.at(virtual_regs_info_.reg_indexs.at(effective_reg).processor).at(virtual_regs_info_.reg_indexs.at(effective_reg).related_const)=effective_reg;
                break;
            case virtual_related_data_type::VAR_ADDR:
                virtual_regs_info_.var_addr_to_reg_map.at(virtual_regs_info_.reg_indexs.at(effective_reg).processor).at(virtual_regs_info_.reg_indexs.at(effective_reg).related_var)=effective_reg;
                break;
            case virtual_related_data_type::VAR_VALUE:
                virtual_regs_info_.var_value_to_reg_map.at(virtual_regs_info_.reg_indexs.at(effective_reg).processor).at(virtual_regs_info_.reg_indexs.at(effective_reg).related_var)=effective_reg;
                break;
            default:
                break;
        }
    }
}

list<Arm_asm_file_line * > Graph_coloring_register_manager::generate_instructions_for_loading_var_value(struct ic_data * var,reg_index reg)
{
    list<Arm_asm_file_line * > res,* temp;
    pair<struct ic_data *,reg_index> * event_data=new pair<struct ic_data *,reg_index>(var,reg);
    notify(event(event_type::START_GENERATING_SPILLING_VIRTUAL_CODES,nullptr));
    notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data));
    delete event_data;
    temp=(list<Arm_asm_file_line * > *)notify(event(event_type::GET_SPILLING_VIRTUAL_CODES,nullptr)).pointer_data;
    res=*temp;
    delete temp;
    return res;
}

list<Arm_asm_file_line * > Graph_coloring_register_manager::generate_instructions_for_storing_var_value(struct ic_data * var,reg_index reg)
{
    list<Arm_asm_file_line * > res,* temp;
    pair<struct ic_data *,reg_index> * event_data=new pair<struct ic_data *,reg_index>(var,reg);
    notify(event(event_type::START_GENERATING_SPILLING_VIRTUAL_CODES,nullptr));
    notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
    delete event_data;
    temp=(list<Arm_asm_file_line * > *)notify(event(event_type::GET_SPILLING_VIRTUAL_CODES,nullptr)).pointer_data;
    res=*temp;
    delete temp;
    return res;
}

list<Arm_asm_file_line * > Graph_coloring_register_manager::generate_instructions_for_writing_var_addr_to_new_reg(struct ic_data * var,reg_index reg)
{
    list<Arm_asm_file_line * > res,* temp;
    pair<struct ic_data *,reg_index> * event_data=new pair<struct ic_data *,reg_index>(var,reg);
    notify(event(event_type::START_GENERATING_SPILLING_VIRTUAL_CODES,nullptr));
    notify(event(event_type::WRITE_ADDR_TO_REG,(void *)event_data));
    delete event_data;
    temp=(list<Arm_asm_file_line * > *)notify(event(event_type::GET_SPILLING_VIRTUAL_CODES,nullptr)).pointer_data;
    res=*temp;
    delete temp;
    return res;
}

list<Arm_asm_file_line * > Graph_coloring_register_manager::generate_instructions_for_writing_const_value_to_new_reg(OAA const_value,reg_index reg)
{
    list<Arm_asm_file_line * > res,* temp;
    pair<OAA,reg_index> * event_data=new pair<OAA,reg_index>(const_value,reg);
    notify(event(event_type::START_GENERATING_SPILLING_VIRTUAL_CODES,nullptr));
    notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data));
    delete event_data;
    temp=(list<Arm_asm_file_line * > *)notify(event(event_type::GET_SPILLING_VIRTUAL_CODES,nullptr)).pointer_data;
    res=*temp;
    delete temp;
    return res;
}

void Graph_coloring_register_manager::rewrite_program()
{
    struct
    {
        set<reg_index> array_member_regs;
        set<reg_index> var_regs;
        set<reg_index> not_f_param_in_regs_array_regs;
        set<reg_index> f_param_in_regs_array_regs;
        set<reg_index> addr_regs;
        set<reg_index> const_regs;
    } spilled_regs;
    list<Arm_asm_file_line * > instructions;
    set<reg_index> destination_regs,source_regs,regs;
    map<reg_index,reg_index> replace_regs_map;
    OAA const_value;
    struct ic_data * var;
    reg_index new_reg;
    virtual_target_code_pos current_pos;
    list<Arm_asm_file_line * >::iterator new_ins_pos;
    struct runtime_stack_space * stack_space;
    map<reg_index,reg_index> old_new_regs;
    map<struct ic_data *,bool> f_param_in_regs_array_has_been_spilled;
    set<struct ic_data * > * f_params_in_regs=(set<struct ic_data * > *)notify(event(event_type::GET_FUNC_S_F_PARAMS_IN_REGS,(void *)(virtual_target_code->function->func))).pointer_data;
    map<reg_index,set<reg_index> > map_for_debug;
    //找出要溢出的虚拟寄存器，并对这些要溢出的虚拟寄存器决定溢出的处理顺序
    for(auto node:spilled_nodes)
    {
        switch(virtual_regs_info_.reg_indexs.at(node->reg).data_type)
        {
            case virtual_related_data_type::CONST:
                spilled_regs.const_regs.insert(node->reg);
                break;
            case virtual_related_data_type::VAR_ADDR:
                spilled_regs.addr_regs.insert(node->reg);
                break;
            case virtual_related_data_type::VAR_VALUE:
                if(virtual_regs_info_.reg_indexs.at(node->reg).related_var->is_array_member())
                {
                    spilled_regs.array_member_regs.insert(node->reg);
                }
                else if(virtual_regs_info_.reg_indexs.at(node->reg).related_var->is_array_var())
                {
                    if(f_params_in_regs->find(virtual_regs_info_.reg_indexs.at(node->reg).related_var)!=f_params_in_regs->end())
                    {
                        spilled_regs.f_param_in_regs_array_regs.insert(node->reg);
                    }
                    else
                    {
                        spilled_regs.not_f_param_in_regs_array_regs.insert(node->reg);
                    }
                }
                else
                {
                    spilled_regs.var_regs.insert(node->reg);
                }
                break;
            default:
                break;
        }
    }
    delete f_params_in_regs;
    //溢出处理的顺序如下（不是强制的，只是这样处理会更好）：
    //数组元素
    //溢出数组元素所在的寄存器的时候，需要在定义数组元素之后将该数组元素的值写回内存，在使用数组元素之前将数组元素的值写入虚拟寄存器
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            old_new_regs.clear();
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            destination_regs=get_virtual_traget_instruction_s_all_destination_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.array_member_regs.begin(),spilled_regs.array_member_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    old_new_regs.insert(make_pair(reg,new_reg));
                    map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_loading_var_value(var,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            regs.clear();
            set_intersection(destination_regs.begin(),destination_regs.end(),spilled_regs.array_member_regs.begin(),spilled_regs.array_member_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    //new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    if(old_new_regs.find(reg)!=old_new_regs.end())
                    {
                        new_reg=old_new_regs.at(reg);
                    }
                    else
                    {
                        new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                        map_set_insert(map_for_debug,reg,new_reg);
                    }
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_storing_var_value(var,new_reg);
                    new_ins_pos=ins_pos;
                    advance(new_ins_pos,1);
                    bb->arm_sequence.insert(new_ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos,instructions.size());
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            current_pos++;
        }
    }
    //变量
    //溢出变量所在的寄存器的时候，需要在定义变量之后将该变量的值写回内存，在使用变量之前将变量的值写入虚拟寄存器
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            old_new_regs.clear();
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            destination_regs=get_virtual_traget_instruction_s_all_destination_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.var_regs.begin(),spilled_regs.var_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    old_new_regs.insert(make_pair(reg,new_reg));
                    map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_loading_var_value(var,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            regs.clear();
            set_intersection(destination_regs.begin(),destination_regs.end(),spilled_regs.var_regs.begin(),spilled_regs.var_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    if(old_new_regs.find(reg)!=old_new_regs.end())
                    {
                        new_reg=old_new_regs.at(reg);
                    }
                    else
                    {
                        new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                        map_set_insert(map_for_debug,reg,new_reg);
                    }
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_storing_var_value(var,new_reg);
                    new_ins_pos=ins_pos;
                    advance(new_ins_pos,1);
                    bb->arm_sequence.insert(new_ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos,instructions.size());
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            current_pos++;
        }
    }
    //存放在寄存器中传递的函数数组形参的值
    //溢出这类数组变量的值所在的寄存器的时候，只需要在第一次使用该数组之后将其写回内存，每次在该虚拟寄存器使用之前把地址写入新的虚拟寄存器即可
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            old_new_regs.clear();
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            destination_regs=get_virtual_traget_instruction_s_all_destination_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.f_param_in_regs_array_regs.begin(),spilled_regs.f_param_in_regs_array_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    old_new_regs.insert(make_pair(reg,new_reg));
                    map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_loading_var_value(var,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            regs.clear();
            set_intersection(destination_regs.begin(),destination_regs.end(),spilled_regs.f_param_in_regs_array_regs.begin(),spilled_regs.f_param_in_regs_array_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    if(f_param_in_regs_array_has_been_spilled.find(var)!=f_param_in_regs_array_has_been_spilled.end() && f_param_in_regs_array_has_been_spilled.at(var)==true)
                    {
                        continue;
                    }
                    f_param_in_regs_array_has_been_spilled.insert(make_pair(var,true));
                    set_registers_when_spilling_var_value_s_reg(bb,ins_pos,var);
                    //new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    if(old_new_regs.find(reg)!=old_new_regs.end())
                    {
                        new_reg=old_new_regs.at(reg);
                    }
                    else
                    {
                        new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                        map_set_insert(map_for_debug,reg,new_reg);
                    }
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_storing_var_value(var,new_reg);
                    new_ins_pos=ins_pos;
                    advance(new_ins_pos,1);
                    bb->arm_sequence.insert(new_ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos,instructions.size());
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            current_pos++;
        }
    }
    //除了存放在寄存器中传递的函数数组形参之外的其他数组变量的值
    //溢出这类数组变量的值所在的寄存器的时候，只需要在该虚拟寄存器使用之前把地址写入新的虚拟寄存器即可
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.not_f_param_in_regs_array_regs.begin(),spilled_regs.not_f_param_in_regs_array_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                virtual_regs_info_.set_all_regs_ineffective();
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    new_reg=virtual_regs_info_.new_temp_for_var_value(var,virtual_regs_info_.get_reg_s_processor(reg));
                    map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_loading_var_value(var,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            current_pos++;
        }
    }
    //地址
    //溢出地址所在的寄存器的时候，只需要在该虚拟寄存器使用之前把地址写入新的虚拟寄存器即可
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.addr_regs.begin(),spilled_regs.addr_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                virtual_regs_info_.set_all_regs_ineffective();
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    var=virtual_regs_info_.reg_indexs.at(reg).related_var;
                    new_reg=virtual_regs_info_.new_temp_for_var_addr(var,virtual_regs_info_.get_reg_s_processor(reg));
                    map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_writing_var_addr_to_new_reg(var,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            current_pos++;
        }
    }
    //常数
    //溢出常数所在的寄存器的时候，只需要在该虚拟寄存器使用之前把常数写入新的虚拟寄存器即可
    current_pos=1;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator ins_pos=bb->arm_sequence.begin();ins_pos!=bb->arm_sequence.end();ins_pos++)
        {
            source_regs=get_virtual_traget_instruction_s_all_source_regs(*ins_pos);
            regs.clear();
            set_intersection(source_regs.begin(),source_regs.end(),spilled_regs.const_regs.begin(),spilled_regs.const_regs.end(),inserter(regs,regs.begin()));
            if(!regs.empty())
            {
                replace_regs_map.clear();
                for(auto reg:regs)
                {
                    const_value=virtual_regs_info_.reg_indexs.at(reg).related_const;
                    new_reg=virtual_regs_info_.new_temp_for_const(const_value,virtual_regs_info_.get_reg_s_processor(reg));
                    // map_set_insert(map_for_debug,reg,new_reg);
                    replace_regs_map.insert(make_pair(reg,new_reg));
                    instructions=generate_instructions_for_writing_const_value_to_new_reg(const_value,new_reg);
                    bb->arm_sequence.insert(ins_pos,instructions.cbegin(),instructions.cend());
                    current_func_s_live_intervals.new_empty_virtual_code_segment(current_pos-1,instructions.size());
                    current_pos+=instructions.size();
                }
                (*ins_pos)->replace_regs(replace_regs_map);
            }
            notify(event(event_type::SET_STACK_SPACE,(void *)(&ins_pos)));
            current_pos++;
        }
    }
    // cout<<"array_member_regs :"<<spilled_regs.array_member_regs.size()<<endl;
    // cout<<"var_regs :"<<spilled_regs.var_regs.size()<<endl;
    // cout<<"not_f_param_in_regs_array_regs :"<<spilled_regs.not_f_param_in_regs_array_regs.size()<<endl;
    // cout<<"f_param_in_regs_array_regs :"<<spilled_regs.f_param_in_regs_array_regs.size()<<endl;
    // cout<<"addr_regs :"<<spilled_regs.addr_regs.size()<<endl;
    // cout<<"const_regs :"<<spilled_regs.const_regs.size()<<endl;
    // cout<<endl<<endl;
    // cout<<"SPILL "<<map_for_debug.size()<<":"<<endl;
    // for(auto i:map_for_debug)
    // {
    //     cout<<(virtual_regs_info_.is_physical_reg(i.first)?"r":"VR")<<i.first<<"->{";
    //     for(auto j:i.second)
    //     {
    //         cout<<(virtual_regs_info_.is_physical_reg(j)?"r":"VR")<<j<<",";
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl;
}

void Graph_coloring_register_manager::graph_coloring_register_distribute()
{
    // cout<<"=========================================================="<<endl;
    // //debug输出虚拟寄存器信息
    // cout<<"VIRTUAL REGS INFO:"<<endl;
    // for(auto i:virtual_regs_info_.reg_indexs)
    // {
    //     cout<<(virtual_regs_info_.is_physical_reg(i.first)?"r":"VR")<<i.first<<":";
    //     switch(i.second.data_type)
    //     {
    //         case virtual_related_data_type::CONST:
    //             cout<<i.second.related_const.int_data<<endl;
    //             break;
    //         case virtual_related_data_type::VAR_ADDR:
    //             if(i.second.related_var->is_array_member())
    //             {
    //                 cout<<i.second.related_var->get_belong_array()->get_var_name()<<"[";
    //                 if(i.second.related_var->get_offset()->is_const())
    //                 {
    //                     cout<<i.second.related_var->get_offset()->get_value().int_data;
    //                 }
    //                 else if(i.second.related_var->get_offset()->is_tmp_var())
    //                 {
    //                     cout<<"@"<<i.second.related_var->get_offset()->tmp_index;
    //                 }
    //                 else
    //                 {
    //                     cout<<i.second.related_var->get_offset()->get_var_name();
    //                 }
    //                 cout<<"]";
    //             }
    //             else if(i.second.related_var->is_tmp_var())
    //             {
    //                 cout<<"@"<<i.second.related_var->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<i.second.related_var->get_var_name();
    //             }
    //             cout<<"\t\t\t\t\t(addr)"<<endl;
    //             break;
    //         case virtual_related_data_type::VAR_VALUE:
    //             if(i.second.related_var->is_array_member())
    //             {
    //                 cout<<i.second.related_var->get_belong_array()->get_var_name()<<"[";
    //                 if(i.second.related_var->get_offset()->is_const())
    //                 {
    //                     cout<<i.second.related_var->get_offset()->get_value().int_data;
    //                 }
    //                 else if(i.second.related_var->get_offset()->is_tmp_var())
    //                 {
    //                     cout<<"@"<<i.second.related_var->get_offset()->tmp_index;
    //                 }
    //                 else
    //                 {
    //                     cout<<i.second.related_var->get_offset()->get_var_name();
    //                 }
    //                 cout<<"]";
    //             }
    //             else if(i.second.related_var->is_tmp_var())
    //             {
    //                 cout<<"@"<<i.second.related_var->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<i.second.related_var->get_var_name();
    //             }
    //             cout<<"\t\t\t\t\t(value)"<<endl;
    //             break;
    //         default:
    //             break;
    //     }
    // }
    // cout<<endl;
    // cout<<"VIRTUAL CODE:"<<endl;
    // size_t count=1;
    // for(auto i:virtual_target_code->basic_blocks)
    // {
    //     for(auto j:i->arm_sequence)
    //     {
    //         cout<<count++<<":"<<j->to_string()<<endl;
    //     }
    // }
    // cout<<endl;
    //首先构建虚拟寄存器之间的相干图
    build_coherent_diagram();
    //根据相干图初始化各个worklist
    mk_worklists();
    while(!simplify_worklist.empty() || !worklist_moves.empty() || !freeze_worklist.empty() || !spill_worklist.empty())
    {
        if(!simplify_worklist.empty())
        {
            simplify();
        }
        else if(!worklist_moves.empty())
        {
            coalesce();
        }
        else if(!freeze_worklist.empty())
        {
            freeze();
        }
        else if(!spill_worklist.empty())
        {
            select_spill();
        }
    }
    //图着色
    assign_colors();
    //如果还有节点溢出，那么重写虚拟目标代码，并且再次进行本算法
    if(!spilled_nodes.empty())
    {
        rewrite_program();
        clear_info();
        graph_coloring_register_distribute();
    }
}

void Graph_coloring_register_manager::virtual_target_code_to_physical_target_code()
{
    map<reg_index,reg_index> regs_map;
    for(auto colored_node:colored_nodes)
    {
        regs_map.insert(make_pair(colored_node.first->reg,colored_node.second));
    }
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(auto line:bb->arm_sequence)
        {
            line->replace_regs(regs_map);
        }
    }
}

void Graph_coloring_register_manager::clear_info()
{
    current_func_s_live_analysis.clear();
    current_func_s_live_intervals.clear();
    current_func_s_coherent_diagram.clear();
    initial.clear();
    precolored.clear();
    simplify_worklist.clear();
    freeze_worklist.clear();
    spill_worklist.clear();
    spilled_nodes.clear();
    coalesced_nodes.clear();
    colored_nodes.clear();
    while(!select_stack.empty())
    {
        select_stack.pop();
    }
    worklist_moves.clear();
}

void Graph_coloring_register_manager::handle_END_FUNC()
{
    //获取当前函数的虚拟目标代码
    virtual_target_code=(struct arm_func_flow_graph *)notify(event(event_type::GET_VIRTUAL_TRAGET_CODE_OF_CURRENT_FUNC,nullptr)).pointer_data;
    virtual_target_code->build_nexts_between_basic_blocks();
    virtual_target_code->build_loop_info();
    //cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<virtual_target_code->function->func->name<<endl<<endl;
    //把物理寄存器堆清空
    regs_info_.clear();
    //对其进行图着色寄存器分配
    current_processor=reg_processor::CPU;
    graph_coloring_register_distribute();
    // cout<<endl<<endl<<endl<<"COLORED NODES:"<<endl;
    // for(auto i:colored_nodes)
    // {
    //     cout<<i.first->reg<<"-->"<<i.second<<endl;
    // }
    //对虚拟目标代码进行优化
    notify(event(event_type::OPTIMIZE,(void *)virtual_target_code));
    //根据着色的结果从虚拟目标代码生成实际目标代码
    virtual_target_code_to_physical_target_code();
    //把虚拟寄存器堆清空
    virtual_regs_info_.clear();
    //最后把所有的用于虚拟寄存器分配的信息清空
    clear_info();
}