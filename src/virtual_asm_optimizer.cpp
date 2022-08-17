/*
 *
 *  virtual_asm_optimizer.cpp
 *  虚拟目标代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/8/1
 *
*/
#include "virtual_asm_optimizer.h"
#include "graph_coloring_register_manager.h"

//减少常量寄存器
void Graph_coloring_register_manager::reduce_const_regs()
{
    reg_index reg;
    virtual_target_code_pos pos;
    list<Arm_asm_file_line *>::iterator line_it;
    Arm_asm_file_line * line;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    int const_int_data;
    bool tag;
    struct operand2 op2;
    for(auto virtual_regs_s_live_interval:current_func_s_live_intervals.virtual_regs_s_live_intervals)
    {
        reg=virtual_regs_s_live_interval.first;
        if(virtual_regs_info_.is_physical_reg(reg) || 
        virtual_regs_info_.reg_indexs.at(reg).data_type!=virtual_related_data_type::CONST || 
        virtual_regs_info_.reg_indexs.at(reg).processor!=reg_processor::CPU || 
        !operand2::is_legal_immed_8r(virtual_regs_info_.reg_indexs.at(reg).related_const.int_data))
        {
            continue;
        }
        const_int_data=virtual_regs_info_.reg_indexs.at(reg).related_const.int_data;
        for(auto live_interval_segment:virtual_regs_s_live_interval.second.live_interval)
        {
            pos=live_interval_segment.second;
            tag=true;
            while(pos>live_interval_segment.first)
            {
                if(virtual_regs_s_live_interval.second.is_use_pos(pos))
                {
                    line=*get_virtual_target_code(pos);
                    cpu_data_process_ins=to_Arm_cpu_data_process_instruction(line);
                    if(cpu_data_process_ins && 
                    cpu_data_process_ins->get_op()==arm_op::MOV && 
                    cpu_data_process_ins->get_all_source_regs().size()==1 && 
                    (*cpu_data_process_ins->get_all_source_regs().begin())==reg)
                    {
                        cpu_data_process_ins->change_operand2_to_immed_8r(const_int_data);
                    }
                    else
                    {
                        tag=false;
                    }
                }
                else if(virtual_regs_s_live_interval.second.is_def_pos(pos))
                {
                    tag=false;
                }
                pos--;
            }
            if(tag && virtual_regs_s_live_interval.second.is_def_pos(pos))
            {
                line_it=get_virtual_target_code(pos);
                delete (*line_it);
                (*line_it)=new Arm_pseudo_instruction();
            }
        }
    }
}

//分裂寄存器，把有些寄存器进行分裂
void Graph_coloring_register_manager::fission_regs()
{
    reg_index new_reg,old_reg;
    map<reg_index,reg_index> old_new_regs;
    struct arm_basic_block * bb;
    virtual_target_code_pos live_pos,farest_pos;
    Arm_asm_file_line * line;
    map<reg_index,set<reg_index> > debug_info;
    list<pair<virtual_target_code_pos,virtual_target_code_pos> > fission_reg_s_live_interval;
    bool tag;
    for(auto virtual_regs_s_live_interval:current_func_s_live_intervals.virtual_regs_s_live_intervals)
    {
        old_reg=virtual_regs_s_live_interval.first;
        if(virtual_regs_info_.is_physical_reg(old_reg) || virtual_regs_s_live_interval.second.live_interval.size()==1)
        {
            continue;
        }
        fission_reg_s_live_interval.clear();
        farest_pos=0;
        for(auto live_interval_segment:virtual_regs_s_live_interval.second.live_interval)
        {
            if(fission_reg_s_live_interval.empty())
            {
                fission_reg_s_live_interval.push_back(live_interval_segment);
            }
            else
            {
                // if(fission_reg_s_live_interval.back().second==live_interval_segment.first || 
                // !virtual_regs_s_live_interval.second.is_def_pos(live_interval_segment.first) || 
                // (get_pos_s_basic_block(live_interval_segment.first)==get_pos_s_basic_block(fission_reg_s_live_interval.back().second)))
                // {
                //     fission_reg_s_live_interval.back().second=live_interval_segment.second;
                // }
                // else
                // {
                //     fission_reg_s_live_interval.push_back(live_interval_segment);
                // }
                if(live_interval_segment.first>=farest_pos && virtual_regs_s_live_interval.second.is_def_pos(live_interval_segment.first) && fission_reg_s_live_interval.back().second!=live_interval_segment.first)
                {
                    fission_reg_s_live_interval.push_back(live_interval_segment);
                }
                else
                {
                    fission_reg_s_live_interval.back().second=live_interval_segment.second;
                }
            }
            bb=get_pos_s_basic_block(live_interval_segment.first);
            tag=false;
            for(auto basic_block:virtual_target_code->basic_blocks)
            {
                if(bb!=basic_block && !tag)
                {
                    continue;
                }
                tag=true;
                if(get_basic_block_s_end_pos(bb)>=live_interval_segment.second)
                {
                    break;
                }
                for(auto successor:basic_block->get_successors())
                {
                    if(farest_pos<get_basic_block_s_start_pos(successor))
                    {
                        farest_pos=get_basic_block_s_start_pos(successor);
                    }
                }
            }
        }
        fission_reg_s_live_interval.pop_front();
        for(auto new_reg_s_live_interval:fission_reg_s_live_interval)
        {
            switch(virtual_regs_info_.reg_indexs.at(old_reg).data_type)
            {
                case virtual_related_data_type::CONST:
                    new_reg=virtual_regs_info_.new_temp_for_const(virtual_regs_info_.reg_indexs.at(old_reg).related_const,virtual_regs_info_.get_reg_s_processor(old_reg));
                    break;
                case virtual_related_data_type::VAR_ADDR:
                    new_reg=virtual_regs_info_.new_temp_for_var_addr(virtual_regs_info_.reg_indexs.at(old_reg).related_var,virtual_regs_info_.get_reg_s_processor(old_reg));
                    break;
                case virtual_related_data_type::VAR_VALUE:
                    new_reg=virtual_regs_info_.new_temp_for_var_value(virtual_regs_info_.reg_indexs.at(old_reg).related_var,virtual_regs_info_.get_reg_s_processor(old_reg));
                    break;
                default:
                    break;
            }


            map_set_insert(debug_info,old_reg,new_reg);


            old_new_regs.clear();
            old_new_regs.insert(make_pair(old_reg,new_reg));
            for(auto def_basic_block_and_poses:virtual_regs_s_live_interval.second.def_poses)
            {
                for(auto def_pos:def_basic_block_and_poses.second)
                {
                    if(def_pos>=new_reg_s_live_interval.first && def_pos<=new_reg_s_live_interval.second)
                    {
                        line=*get_virtual_target_code(def_pos);
                        line->replace_regs(old_new_regs);
                    }
                }
            }
            for(auto use_basic_block_and_poses:virtual_regs_s_live_interval.second.use_poses)
            {
                for(auto use_pos:use_basic_block_and_poses.second)
                {
                    if(use_pos>=new_reg_s_live_interval.first && use_pos<=new_reg_s_live_interval.second)
                    {
                        line=*get_virtual_target_code(use_pos);
                        line->replace_regs(old_new_regs);
                    }
                }
            }
        }
    }
    // cout<<endl<<"FISSION REGS:"<<endl;
    // for(auto i:debug_info)
    // {
    //     cout<<"VR"<<i.first<<"->{";
    //     for(auto j:i.second)
    //     {
    //         cout<<"VR"<<j<<",";
    //     }
    //     cout<<"}"<<endl;
    // }
}

void Graph_coloring_register_manager::peephole_optimization()
{
    static reg_index sp=regs_info_.reg_names.at("sp");
    reg_index reg;
    list<Arm_asm_file_line * >::iterator next_line;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins,* new_cpu_single_register_load_and_store_ins;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator line=bb->arm_sequence.begin();line!=bb->arm_sequence.end();line++)
        {
            cpu_data_process_ins=to_Arm_cpu_data_process_instruction(*line);
            if(cpu_data_process_ins && 
            cpu_data_process_ins->get_op()==arm_op::ADD && 
            cpu_data_process_ins->get_all_destination_regs().size()==1 && 
            cpu_data_process_ins->get_all_source_regs().size()==1 && 
            *(cpu_data_process_ins->get_all_source_regs().begin())==sp && 
            cpu_data_process_ins->get_operand2().type==operand2_type::IMMED_8R && 
            flexoffset::is_legal_expr(cpu_data_process_ins->get_operand2().immed_8r))
            {
                reg=*(cpu_data_process_ins->get_all_destination_regs().begin());
                next_line=line;
                advance(next_line,1);
                if(next_line!=bb->arm_sequence.end())
                {
                    cpu_single_register_load_and_store_ins=to_Arm_cpu_single_register_load_and_store_instruction(*next_line);
                    if(cpu_single_register_load_and_store_ins && 
                    cpu_single_register_load_and_store_ins->get_single_register_load_and_store_type()==arm_single_register_load_and_store_type::ZERO_OFFSET && 
                    reg==(cpu_single_register_load_and_store_ins->get_source_registers().registers_.front()))
                    {
                        new_cpu_single_register_load_and_store_ins=new Arm_cpu_single_register_load_and_store_instruction(cpu_single_register_load_and_store_ins->get_op(),cpu_single_register_load_and_store_ins->get_cond(),cpu_single_register_load_and_store_ins->get_data_type(),cpu_single_register_load_and_store_ins->get_destination_registers().registers_.front(),sp,flexoffset(cpu_data_process_ins->get_operand2().immed_8r));
                        delete (*next_line);
                        (*next_line)=new_cpu_single_register_load_and_store_ins;
                    }
                }
            }
        }
    }
}

void Graph_coloring_register_manager::remove_useless_regs()
{
    static reg_index sp=regs_info_.reg_names.at("sp"),
    fp=regs_info_.reg_names.at("fp");
    reg_index destination_reg;
    set<reg_index> source_regs,all_source_regs;
    Arm_asm_file_line * new_line;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(auto line:bb->arm_sequence)
        {
            source_regs=line->get_all_source_regs();
            set_union(source_regs.begin(),source_regs.end(),all_source_regs.begin(),all_source_regs.end(),inserter(all_source_regs,all_source_regs.begin()));
        }
    }
    for(auto bb:virtual_target_code->basic_blocks)
    {
        for(list<Arm_asm_file_line * >::iterator line=bb->arm_sequence.begin();line!=bb->arm_sequence.end();line++)
        {
            cpu_data_process_ins=to_Arm_cpu_data_process_instruction(*line);
            if(cpu_data_process_ins && 
            cpu_data_process_ins->get_op()==arm_op::ADD && 
            cpu_data_process_ins->get_cond()==arm_condition::NONE && 
            !cpu_data_process_ins->is_update_flags())
            {
                destination_reg=*((*line)->get_all_destination_regs().begin());
                source_regs=(*line)->get_all_source_regs();
                if(!virtual_regs_info_.is_physical_reg(destination_reg) && 
                (source_regs.find(sp)!=source_regs.end() || source_regs.find(fp)!=source_regs.end()) && 
                all_source_regs.find(destination_reg)==all_source_regs.end())
                {
                    new_line=new Arm_pseudo_instruction();
                    delete (*line);
                    (*line)=new_line;
                }
            }
        }
    }
}

void Graph_coloring_register_manager::optimize_func_and_basic_block_s_enter_and_exit()
{
    /*
    这个优化做了之后有很多情况下性能不升反降???
    */
    static reg_index sp=regs_info_.reg_names.at("sp"),
    fp=regs_info_.reg_names.at("fp"),
    lr=regs_info_.reg_names.at("lr");
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
    for(auto basic_block:virtual_target_code->basic_blocks)
    {
        //当基本块满足下列条件的时候，我们认为该基本块出入口的移动sp为临时变量开辟和释放内存空间是可以省略的
        //该基本块中只有add sp,sp,...和sub sp,sp,...等指令涉及到了sp寄存器，其他的指令没有涉及到寄存器
        add_sp_inses.clear();
        sub_sp_inses.clear();
        for(list<Arm_asm_file_line * >::iterator line=basic_block->arm_sequence.begin();line!=basic_block->arm_sequence.end();line++)
        {
            cpu_data_process_instruction=to_Arm_cpu_data_process_instruction(*line);
            if(cpu_data_process_instruction)
            {
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
                        if(cpu_data_process_instruction->get_op()==arm_op::ADD)
                        {
                            if(*destination_regs.begin()==sp)
                            {
                                add_sp_inses.push_back(line);
                                continue;
                            }
                            else if(*destination_regs.begin()==fp)
                            {
                                continue;
                            }
                        }
                        else if(cpu_data_process_instruction->get_op()==arm_op::SUB)
                        {
                            sub_sp_inses.push_back(line);
                            continue;
                        }
                    }
                }
            }
            all_regs=(*line)->get_all_regs();
            if(all_regs.find(sp)!=all_regs.end())
            {
                optimize_func_enter_exit=false;
                goto next;
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

void Graph_coloring_register_manager::local_optimize()
{
    
}

void Graph_coloring_register_manager::global_optimize()
{
    remove_useless_regs();
}

void Graph_coloring_register_manager::optimize_for_less_spill_regs()
{
    reduce_const_regs();
    fission_regs();
    peephole_optimization();
    remove_useless_regs();
}

void Graph_coloring_register_manager::optimize_before_rewrite_program()
{
    peephole_optimization();
    remove_useless_regs();
}

void Graph_coloring_register_manager::optimize_virtual_code()
{
    optimize_func_and_basic_block_s_enter_and_exit();
    local_optimize();
    global_optimize();
}