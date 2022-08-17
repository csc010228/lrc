/*
 *
 *  union_optimize.cpp
 *  联合优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#include"ic_optimizer.h"
#include"data_flow_analyse.h"

/*
多线程优化
*/
void thread_optimize(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    string f_param_name;
    size_t thread_part_func_num=0,new_f_param_num,offset;
    struct ic_func * thread_part_func;
    struct ic_func_flow_graph * thread_part_func_flow_graph;
    struct ic_scope * thread_part_func_s_scope;
    struct ic_data * new_var,* var,* belong_array,* arg1,* arg2;
    struct ic_basic_block * new_basic_block;
    struct ic_label * old_label,* out_label;
    struct quaternion new_ic;
    struct quaternion_with_info exit_loop_judge;
    list<struct ic_data * >::iterator f_param,r_param;
    map<struct ic_data *,set<struct ic_data * > > def_array_members,use_array_members;
    map<struct ic_data *,struct ic_data * > old_and_new_vars_map;
    map<struct ic_label *,struct ic_label * > old_and_new_labels_map;
    map<struct ic_basic_block *,struct ic_basic_block * > old_and_new_ic_basic_block_map;
    map<struct ic_label * ,struct ic_basic_block * > new_label_basic_block_map;
    list<struct ic_data * > * thread_part_func_s_f_params,* thread_part_func_s_r_params;
    list<struct ic_basic_block * > copyed_basic_blocks,new_basic_blocks;
    map<struct loop_info *,struct quaternion_with_info> exit_loop_judges;
    map<struct loop_info *,struct ic_data * > loop_vars;
    map<struct loop_info *,struct ic_data * > loop_limits;
    set<struct loop_info * > loops_cannot_analyse;
    set<ic_pos> def_in_loop_1,def_in_loop_2;
    stack<struct loop_info * > loops;
    struct loop_info * loop;
    ic_pos pos;
    bool tag=false;
    if(func->func->type!=func_type::PROGRAMER_DEFINED)
    {
        return;
    }
    for(auto loop_info:func->loops_info)
    {
        //暂时只对最外层的循环进行多线程优化
        if(loop_info.second->father_loop!=nullptr)
        {
            continue;
        }
        //满足下列所有条件的循环才能进行自动多线程：
        //（1）循环中没有return
        //（2）循环中没有函数调用
        //（3）循环中定义的数据不会被循环外用到
        //（4）循环中定义的数组元素是互不干扰的
        //（5）只有一处IF_JMP或者IF_NOT_JMP可以跳出循环
        //寻找循环以及它的所有子循环的退出判断语句，以及它们的循环变量，还有循环变量的上下限
        loops.push(loop_info.second);
        while(!loops.empty())
        {
            loop=loops.top();
            loops.pop();
            for(auto child:loop->children_loops)
            {
                loops.push(child);
            }
            for(auto bb_in_loop:loop->all_basic_blocks)
            {
                for(auto ic_with_info_in_loop:bb_in_loop->ic_sequence)
                {
                    switch(ic_with_info_in_loop.intermediate_code.op)
                    {
                        case ic_op::JMP:
                        case ic_op::IF_JMP:
                        case ic_op::IF_NOT_JMP:
                            old_label=ic_with_info_in_loop.get_related_label();
                            if(loop->all_basic_blocks.find(func->label_basic_block_map.at(old_label))==loop->all_basic_blocks.end())
                            {
                                if(ic_with_info_in_loop.intermediate_code.op!=ic_op::JMP && 
                                exit_loop_judges.find(loop)==exit_loop_judges.end())
                                {
                                    exit_loop_judges.insert(make_pair(loop,func->get_ic_with_info(*(*(ic_with_info_in_loop.ud_chain.begin())).second.begin())));
                                }
                                else if(loops_cannot_analyse.find(loop)==loops_cannot_analyse.end())
                                {
                                    loops_cannot_analyse.insert(loop);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        for(auto loop_cannot_analyse:loops_cannot_analyse)
        {
            if(exit_loop_judges.find(loop_cannot_analyse)!=exit_loop_judges.end())
            {
                exit_loop_judges.erase(loop_cannot_analyse);
            }
        }
        if(exit_loop_judges.find(loop_info.second)==exit_loop_judges.end())
        {
            goto out;
        }
        for(auto loop_and_exit_loop_judge:exit_loop_judges)
        {
            exit_loop_judge=loop_and_exit_loop_judge.second;
            arg1=(struct ic_data *)exit_loop_judge.intermediate_code.arg1.second;
            arg2=(struct ic_data *)exit_loop_judge.intermediate_code.arg2.second;
            def_in_loop_1.clear();
            def_in_loop_2.clear();
            if(exit_loop_judge.ud_chain.find(arg1)!=exit_loop_judge.ud_chain.end())
            {
                for(auto def_pos:exit_loop_judge.ud_chain.at(arg1))
                {
                    if(loop_and_exit_loop_judge.first->all_basic_blocks.find(def_pos.basic_block)!=loop_and_exit_loop_judge.first->all_basic_blocks.end())
                    {
                        def_in_loop_1.insert(def_pos);
                    }
                }
            }
            if(exit_loop_judge.ud_chain.find(arg2)!=exit_loop_judge.ud_chain.end())
            {
                for(auto def_pos:exit_loop_judge.ud_chain.at(arg2))
                {
                    if(loop_and_exit_loop_judge.first->all_basic_blocks.find(def_pos.basic_block)!=loop_and_exit_loop_judge.first->all_basic_blocks.end())
                    {
                        def_in_loop_2.insert(def_pos);
                    }
                }
            }
            if(def_in_loop_1.empty() && !def_in_loop_2.empty())
            {
                data_exchange(arg1,arg2);
            }
            else if(def_in_loop_2.empty() && !def_in_loop_1.empty())
            {
                ;
            }
            else if(loop_and_exit_loop_judge.first==loop_info.second)
            {
                goto out;
            }
            loop_vars.insert(make_pair(loop_and_exit_loop_judge.first,arg1));
            loop_limits.insert(make_pair(loop_and_exit_loop_judge.first,arg2));
        }
        if(loop_vars.find(loop_info.second)==loop_vars.end() || loop_limits.find(loop_info.second)==loop_limits.end())
        {
            goto out;
        }
        //查看循环中有哪些使用的数据是循环外部定义的
        //查看循环中有哪些定义的数据会传递到循环外部
        new_f_param_num=0;
        out_label=nullptr;
        for(auto bb_in_loop:loop_info.second->all_basic_blocks)
        {
            offset=0;
            for(auto ic_with_info_in_loop:bb_in_loop->ic_sequence)
            {
                pos=ic_pos(bb_in_loop,offset);
                switch(ic_with_info_in_loop.intermediate_code.op)
                {
                    case ic_op::VAR_DEFINE:
                        if(((struct ic_data *)ic_with_info_in_loop.intermediate_code.result.second)->is_array_var())
                        {
                            goto out;
                        }
                        break;
                    case ic_op::CALL:
                    case ic_op::RET:
                        goto out;
                        break;
                    default:
                        break;
                }
                for(auto ud_chain_node:ic_with_info_in_loop.ud_chain)
                {
                    if(old_and_new_vars_map.find(ud_chain_node.first)!=old_and_new_vars_map.end() || 
                    ud_chain_node.first->is_global())
                    {
                        continue;
                    }
                    if(ud_chain_node.first->is_array_member())
                    {
                        belong_array=ud_chain_node.first->get_belong_array();
                        map_set_insert(use_array_members,belong_array,ud_chain_node.first);
                        continue;
                    }
                    for(auto def_pos:ud_chain_node.second)
                    {
                        if(loop_info.second->all_basic_blocks.find(def_pos.basic_block)==loop_info.second->all_basic_blocks.end())
                        {
                            if(ud_chain_node.first->is_tmp_var())
                            {
                                f_param_name="%"+to_string(new_f_param_num++);
                            }
                            else
                            {
                                f_param_name=ud_chain_node.first->get_var_name();
                            }
                            old_and_new_vars_map.insert(make_pair(ud_chain_node.first,new struct ic_data(f_param_name,ud_chain_node.first->get_data_type(),ud_chain_node.first->dimensions_len,false)));
                            break;
                        }
                    }
                }
                for(auto du_chain_node:ic_with_info_in_loop.du_chain)
                {
                    if(du_chain_node.first->is_array_var())
                    {
                        if(!du_chain_node.first->is_global())
                        {
                            old_and_new_vars_map.insert(make_pair(du_chain_node.first,new struct ic_data(du_chain_node.first->get_var_name(),du_chain_node.first->get_data_type(),du_chain_node.first->dimensions_len,false)));
                        }
                        continue;
                    }
                    if(du_chain_node.first->is_array_member())
                    {
                        belong_array=du_chain_node.first->get_belong_array();
                        map_set_insert(def_array_members,belong_array,du_chain_node.first);
                        continue;
                    }
                    if(du_chain_node.first->is_global())
                    {
                        goto out;
                    }
                    for(auto use_pos:du_chain_node.second)
                    {
                        if(loop_info.second->all_basic_blocks.find(use_pos.basic_block)==loop_info.second->all_basic_blocks.end())
                        {
                            goto out;
                        }
                    }
                }
                offset++;
            }
        }
        for(auto def_array_and_member:def_array_members)
        {
            if(use_array_members.find(def_array_and_member.first)!=use_array_members.end())
            {
                goto out;
            }
        }
        //把符合条件的循环抽象成一个thread函数
        thread_part_func_s_f_params=new list<struct ic_data * >;
        for(auto new_f_param:old_and_new_vars_map)
        {
            thread_part_func_s_f_params->push_back(new_f_param.second);
        }
        thread_part_func=symbol_table->new_func(func->func->name+".thread."+to_string(thread_part_func_num++),func_type::THREAD_PART,language_data_type::VOID,thread_part_func_s_f_params);
        thread_part_func_s_scope=new ic_scope(symbol_table->get_global_scope(),thread_part_func);
        //生成调用该thread函数的语句
        thread_part_func_s_r_params=new list<struct ic_data * >;
        for(auto new_r_param:old_and_new_vars_map)
        {
            thread_part_func_s_r_params->push_back(new_r_param.first);
        }
        new_ic=quaternion(ic_op::CALL,ic_operand::FUNC,(void *)thread_part_func,ic_operand::DATAS,(void *)thread_part_func_s_r_params,ic_operand::NONE,nullptr);
        if(loop_limits.find(loop_info.second)!=loop_limits.end())
        {
            f_param=thread_part_func_s_f_params->begin();
            r_param=thread_part_func_s_r_params->begin();
            while(f_param!=thread_part_func_s_f_params->end() && r_param!=thread_part_func_s_r_params->end())
            {
                if((*r_param)==loop_limits.at(loop_info.second))
                {
                    thread_part_func_s_f_params->push_front(*f_param);
                    thread_part_func_s_f_params->erase(f_param);
                    thread_part_func_s_r_params->push_front(*r_param);
                    thread_part_func_s_r_params->erase(r_param);
                    break;
                }
                f_param++;
                r_param++;
            }
        }
        if(loop_vars.find(loop_info.second)!=loop_vars.end())
        {
            f_param=thread_part_func_s_f_params->begin();
            r_param=thread_part_func_s_r_params->begin();
            while(f_param!=thread_part_func_s_f_params->end() && r_param!=thread_part_func_s_r_params->end())
            {
                if((*r_param)==loop_vars.at(loop_info.second))
                {
                    thread_part_func_s_f_params->push_front(*f_param);
                    thread_part_func_s_f_params->erase(f_param);
                    thread_part_func_s_r_params->push_front(*r_param);
                    thread_part_func_s_r_params->erase(r_param);
                    break;
                }
                f_param++;
                r_param++;
            }
        }
        //将要循环中的变量进行相应的替换，并更改其相应的作用域
        for(auto bb_in_loop:loop_info.second->all_basic_blocks)
        {
            for(auto ic_with_info_in_loop:bb_in_loop->ic_sequence)
            {
                for(auto var:ic_with_info_in_loop.get_all_datas())
                {
                    if(old_and_new_vars_map.find(var)==old_and_new_vars_map.end())
                    {
                        if(var->is_global() || var->is_array_member())
                        {
                            continue;
                        }
                        else if(var->is_const())
                        {
                            new_var=symbol_table->const_entry(var->get_data_type(),var->get_value());
                        }
                        else if(var->is_tmp_var())
                        {
                            new_var=symbol_table->new_tmp_var(var->get_data_type(),var->dimensions_len,var->get_value(),var->type==ic_data_type::CONST_TMP_VAR);
                        }
                        else
                        {
                            new_var=symbol_table->new_var(var->get_var_name(),var->get_data_type(),var->dimensions_len,var->get_value(),var->type==ic_data_type::LOCAL_CONST_VAR,thread_part_func_s_scope);
                        }
                        old_and_new_vars_map.insert(make_pair(var,new_var));
                    }
                }
                old_label=ic_with_info_in_loop.get_related_label();
                if(old_label && old_and_new_labels_map.find(old_label)==old_and_new_labels_map.end())
                {
                    old_and_new_labels_map.insert(make_pair(old_label,symbol_table->new_label()));
                    if(loop_info.second->all_basic_blocks.find(func->label_basic_block_map.at(old_label))==loop_info.second->all_basic_blocks.end())
                    {
                        out_label=old_label;
                    }
                }
            }
        }
        //将循环中的所有基本块复制一份
        for(auto old_basic_block:loop_info.second->all_basic_blocks)
        {
            new_basic_block=new ic_basic_block(*old_basic_block,func,old_and_new_vars_map,old_and_new_labels_map);
            copyed_basic_blocks.push_back(new_basic_block);
            old_and_new_ic_basic_block_map.insert(make_pair(old_basic_block,new_basic_block));
        }
        //将复制完成的循环基本块流图放入新的函数
        thread_part_func_flow_graph=new ic_func_flow_graph(thread_part_func);
        intermediate_codes_flow_graph->func_flow_graphs.push_front(thread_part_func_flow_graph);
        thread_part_func_flow_graph->basic_blocks=copyed_basic_blocks;
        for(auto old_and_new_label:old_and_new_labels_map)
        {
            if(old_and_new_ic_basic_block_map.find(func->label_basic_block_map.at(old_and_new_label.first))!=old_and_new_ic_basic_block_map.end())
            {
                thread_part_func_flow_graph->label_basic_block_map.insert(make_pair(old_and_new_label.second,old_and_new_ic_basic_block_map.at(func->label_basic_block_map.at(old_and_new_label.first))));
            }
        }
        //添加FUNC_DEFINE，RET和END_FUNC_DEFINE语句
        new_basic_block=new ic_basic_block(thread_part_func_flow_graph);
        new_basic_block->add_ic(quaternion(ic_op::FUNC_DEFINE,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::FUNC,thread_part_func));
        thread_part_func_flow_graph->basic_blocks.push_front(new_basic_block);
        new_basic_block=new ic_basic_block(thread_part_func_flow_graph);
        thread_part_func_flow_graph->label_basic_block_map.insert(make_pair(old_and_new_labels_map.at(out_label),new_basic_block));
        new_basic_block->add_ic(quaternion(ic_op::LABEL_DEFINE,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,(void *)old_and_new_labels_map.at(out_label)));
        new_basic_block->add_ic(quaternion(ic_op::RET,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
        new_basic_block->add_ic(quaternion(ic_op::END_FUNC_DEFINE,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::FUNC,thread_part_func));
        thread_part_func_flow_graph->basic_blocks.push_back(new_basic_block);
        //对新构建的thread函数的数据流分析
        Data_flow_analyzer::data_flow_analysis_for_a_func(thread_part_func_flow_graph,true);
        //将旧的函数中的循环替换成一条函数调用
        tag=false;
        for(auto bb:func->basic_blocks)
        {
            if(loop_info.second->all_basic_blocks.find(bb)==loop_info.second->all_basic_blocks.end())
            {
                new_basic_blocks.push_back(bb);
            }
            else if(!tag)
            {
                tag=true;
                new_basic_block=new ic_basic_block(func);
                new_basic_block->add_ic(new_ic);
                new_basic_blocks.push_back(new_basic_block);
            }
        }
        func->basic_blocks=new_basic_blocks;
        for(auto label_to_bb:func->label_basic_block_map)
        {
            if(loop_info.second->all_basic_blocks.find(label_to_bb.second)==loop_info.second->all_basic_blocks.end())
            {
                new_label_basic_block_map.insert(label_to_bb);
            }
        }
        func->label_basic_block_map=new_label_basic_block_map;
out:
        //清理现场
        old_and_new_vars_map.clear();
        old_and_new_labels_map.clear();
        old_and_new_ic_basic_block_map.clear();
        copyed_basic_blocks.clear();
        def_array_members.clear();
        use_array_members.clear();
        new_basic_blocks.clear();
        new_label_basic_block_map.clear();
        exit_loop_judges.clear();
        loops_cannot_analyse.clear();
        loop_vars.clear();
        loop_limits.clear();
        while(!loops.empty())
        {
            loops.pop();
        }
    }
    if(tag)
    {
        //重新对旧的函数进行数据流分析
        func->build_nexts_between_basic_blocks();
        Data_flow_analyzer::data_flow_analysis_for_a_func(func,true);
    }
}
