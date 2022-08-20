/*
 *
 *  ic_local_optimize.cpp
 *  中间代码局部优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#include"ic_local_optimize.h"
#include"dag.h"

/*
删除无用的函数返回值
*/
#include<iostream>
void remove_useless_return(struct ic_flow_graph * intermediate_codes_flow_graph)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_func_flow_graph * called_func_flow_graph,* new_func_flow_graph;
    set<struct ic_func * > funcs;
    set<struct ic_data * > used_tmp_vars,uses;
    ic_pos pos;
    size_t offset;
    list<struct ic_data * > * new_f_params;
    struct ic_func * called_func,* new_func;
    struct ic_basic_block * new_bb;
    struct ic_data * ret_val,* new_f_param;
    map<struct ic_func * ,size_t > func_called_times;
    map<struct ic_func * ,size_t > non_used_ret_func_called_times;
    map<struct ic_data * ,struct ic_data * > old_new_datas_map;
    map<struct ic_label * ,struct ic_label * > old_new_labels_map;
    map<struct ic_basic_block * ,struct ic_basic_block * > old_new_bbs_map;
    struct ic_label * label,* new_label;
    struct ic_scope * new_func_scope;
    //先统计函数调用信息
    for(auto func_flow_graph:intermediate_codes_flow_graph->func_flow_graphs)
    {
        for(auto basic_block:func_flow_graph->basic_blocks)
        {
            offset=0;
            used_tmp_vars.clear();
            for(vector<quaternion_with_info>::reverse_iterator ic_with_info=basic_block->ic_sequence.rbegin();ic_with_info!=basic_block->ic_sequence.rend();ic_with_info++)
            {
                pos=ic_pos(basic_block,offset++);
                if((*ic_with_info).intermediate_code.op==ic_op::CALL && (*ic_with_info).intermediate_code.result.first==ic_operand::DATA)
                {
                    called_func=(struct ic_func *)(*ic_with_info).intermediate_code.arg1.second;
                    ret_val=(struct ic_data *)(*ic_with_info).intermediate_code.result.second;
                    if(called_func->type==func_type::PROGRAMER_DEFINED && 
                    called_func->return_type!=language_data_type::VOID)
                    {
                        if(func_called_times.find(called_func)==func_called_times.end())
                        {
                            func_called_times.insert(make_pair(called_func,0));
                        }
                        func_called_times.at(called_func)++;
                        if(ret_val->is_tmp_var() && 
                        used_tmp_vars.find(ret_val)==used_tmp_vars.end())
                        {
                            if(non_used_ret_func_called_times.find(called_func)==non_used_ret_func_called_times.end())
                            {
                                non_used_ret_func_called_times.insert(make_pair(called_func,0));
                            }
                            non_used_ret_func_called_times.at(called_func)++;
                        }
                    }
                }
                for(auto use:(*ic_with_info).uses)
                {
                    if(use->is_tmp_var() && used_tmp_vars.find(use)==used_tmp_vars.end())
                    {
                        used_tmp_vars.insert(use);
                    }
                }
            }
        }
    }
    //找出需要改成void返回类型的所有目标函数
    for(auto non_used_ret_func:non_used_ret_func_called_times)
    {
        if(func_called_times.at(non_used_ret_func.first)==non_used_ret_func.second)
        {
            funcs.insert(non_used_ret_func.first);
        }
    }
    //为找出的目标函数生成新的void返回类型函数
    for(auto func:funcs)
    {
        new_f_params=new list<struct ic_data * >;
        old_new_datas_map.clear();
        old_new_labels_map.clear();
        for(auto f_param:*func->f_params)
        {
            new_f_param=new struct ic_data(f_param->get_var_name(),f_param->get_data_type(),f_param->dimensions_len,f_param->is_const());
            new_f_params->push_back(new_f_param);
            old_new_datas_map.insert(make_pair(f_param,new_f_param));
        }
        new_func=symbol_table->new_func(func->name+".void",func_type::PROGRAMER_DEFINED,language_data_type::VOID,new_f_params);
        new_func_flow_graph=new struct ic_func_flow_graph(new_func);
        new_func_scope=new struct ic_scope(symbol_table->get_global_scope(),new_func);
        called_func_flow_graph=intermediate_codes_flow_graph->get_func_flow_graph(func);
        for(auto bb:called_func_flow_graph->basic_blocks)
        {
            for(auto ic_with_info:bb->ic_sequence)
            {
                for(auto data:ic_with_info.get_all_datas())
                {
                    if(old_new_datas_map.find(data)==old_new_datas_map.end())
                    {
                        if(data->is_tmp_var())
                        {
                            old_new_datas_map.insert(make_pair(data,symbol_table->new_tmp_var(data->get_data_type(),data->dimensions_len,data->get_value(),data->is_const())));
                        }
                        else if(!data->is_global() && !data->is_f_param() && !data->is_array_member())
                        {
                            old_new_datas_map.insert(make_pair(data,symbol_table->new_var(data->get_var_name(),data->get_data_type(),data->dimensions_len,data->get_value(),data->is_const(),new_func_scope)));
                        }
                    }
                }
                if(ic_with_info.intermediate_code.op==ic_op::LABEL_DEFINE)
                {
                    label=ic_with_info.get_related_label();
                    new_label=symbol_table->new_label();
                    new_func_flow_graph->label_basic_block_map.insert(make_pair(new_label,bb));
                    old_new_labels_map.insert(make_pair(label,new_label));
                }
            }
        }
        old_new_bbs_map.clear();
        for(auto bb:called_func_flow_graph->basic_blocks)
        {
            new_bb=new struct ic_basic_block(*bb,new_func_flow_graph,old_new_datas_map,old_new_labels_map);
            old_new_bbs_map.insert(make_pair(bb,new_bb));
            new_func_flow_graph->basic_blocks.push_back(new_bb);
            ret_val=nullptr;
            for(auto & ic_with_info:new_bb->ic_sequence)
            {
                switch(ic_with_info.intermediate_code.op)
                {
                    case ic_op::RET:
                        ret_val=(struct ic_data *)ic_with_info.intermediate_code.result.second;
                        if(!ret_val->is_tmp_var())
                        {
                            ret_val=nullptr;
                        }
                        ic_with_info.intermediate_code.result.first=ic_operand::NONE;
                        ic_with_info.intermediate_code.result.second=nullptr;
                        ic_with_info.uses.clear();
                        break;
                    case ic_op::FUNC_DEFINE:
                    case ic_op::END_FUNC_DEFINE:
                        ic_with_info.intermediate_code.result.second=(void *)new_func;
                        break;
                    default:
                        break;
                }
            }
            if(ret_val)
            {
                uses.clear();
                for(vector<quaternion_with_info>::reverse_iterator ic_with_info=new_bb->ic_sequence.rbegin();ic_with_info!=new_bb->ic_sequence.rend();ic_with_info++)
                {
                    if(uses.find(ret_val)!=uses.end())
                    {
                        break;
                    }
                    if((*ic_with_info).explicit_def==ret_val)
                    {
                        switch((*ic_with_info).intermediate_code.op)
                        {
                            case ic_op::ASSIGN:
                            case ic_op::ADD:
                            case ic_op::SUB:
                            case ic_op::MUL:
                            case ic_op::MOD:
                            case ic_op::DIV:
                                (*ic_with_info)=quaternion_with_info();
                                break;
                            case ic_op::CALL:
                                called_func=(struct ic_func * )((*ic_with_info).intermediate_code.arg1.second);
                                if(symbol_table->get_func_def_globals_and_array_f_params(called_func).empty())
                                {
                                    (*ic_with_info)=quaternion_with_info();
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    set_union(uses.begin(),uses.end(),(*ic_with_info).uses.begin(),(*ic_with_info).uses.end(),inserter(uses,uses.begin()));
                }
            }
        }
        for(auto f:intermediate_codes_flow_graph->func_flow_graphs)
        {
            for(auto b:f->basic_blocks)
            {
                for(auto & i:b->ic_sequence)
                {
                    if(i.intermediate_code.op==ic_op::CALL && 
                    i.intermediate_code.arg1.second==func)
                    {
                        symbol_table->replace_func_direct_calls(f->func,(struct ic_func *)i.intermediate_code.arg1.second,new_func);
                        i.intermediate_code.arg1.second=(void *)new_func;
                        i.intermediate_code.result.first=ic_operand::NONE;
                        i.intermediate_code.result.second=nullptr;
                    }
                }
            }
        }
        intermediate_codes_flow_graph->func_flow_graphs.push_front(new_func_flow_graph);
        for(auto & label_and_old_bb:new_func_flow_graph->label_basic_block_map)
        {
            label_and_old_bb.second=old_new_bbs_map.at(label_and_old_bb.second);
        }
        new_func_flow_graph->build_nexts_between_basic_blocks();
        new_func_flow_graph->build_array_and_offset_to_array_member_map();
        new_func_flow_graph->build_vars_def_and_use_pos_info();
    }
}

/*
函数内联

Parameters
----------
func:要优化的函数流图
*/
void function_inline(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_func * current_func,* called_func;
    set<struct ic_func * > funcs_called_directly_by_called_func;
    struct ic_func_flow_graph * called_func_flow_graph;
    set<struct ic_data * > called_func_def_globals_and_f_params,offsets;
    list<struct ic_data * > * f_params,* r_params;
    list<struct ic_data * >::iterator f_param_iterator,r_param_iterator;
    list<struct ic_basic_block * > copyed_basic_blocks;
    map<struct ic_basic_block *,struct ic_basic_block *> old_and_new_ic_basic_block_map;
    struct ic_basic_block * new_basic_block,* skip_until_basic_block;
    map<struct ic_data *,struct ic_data * > old_and_new_vars_map;
    map<struct ic_data *,struct ic_data * > changed_f_params_map;
    map<struct ic_label *,struct ic_label * > old_and_new_labels_map;
    struct ic_data * new_var,* result;
    struct ic_scope * inline_func_scope;
    struct ic_label * old_label,* new_label;
    vector<struct quaternion_with_info>::iterator another_ic_with_info;
    list<struct quaternion> new_basic_block_ic_sequence;
    size_t called_func_exit_num;
    skip_until_basic_block=nullptr;
    size_t inline_var_num;
again:
    for(auto basic_block:func->basic_blocks)
    {
        if(skip_until_basic_block)
        {
            if(basic_block!=skip_until_basic_block)
            {
                continue;
            }
            else
            {
                skip_until_basic_block=nullptr;
            }
        }
        for(vector<struct quaternion_with_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            if((*ic_with_info).intermediate_code.op==ic_op::CALL)
            {
                current_func=func->func;
                called_func=(struct ic_func *)(*ic_with_info).intermediate_code.arg1.second;
                funcs_called_directly_by_called_func=symbol_table->get_func_direct_calls(called_func);
                if(funcs_called_directly_by_called_func.find(called_func)==funcs_called_directly_by_called_func.end() && funcs_called_directly_by_called_func.find(current_func)==funcs_called_directly_by_called_func.end())
                {
                    //只有当被调用的函数不是递归函数，而且也不会直接调用当前的函数，并且是用户定义的函数的时候才能够进行函数内联
                    called_func_flow_graph=intermediate_codes_flow_graph->get_func_flow_graph(called_func);
                    if(!called_func_flow_graph)
                    {
                        goto next;
                    }
                    //目前由于全局寄存器分配还没有完成，所以暂时只对满足下列所有条件的函数进行内联：
                    //（1）函数形参中没有数组（这个限制是为了效率）
                    //（2）当前函数和被调用的函数中没有涉及到浮点数（这个限制是为了防止带有浮点数的函数过长）
                    //（3）要么所有的形参在函数中都不会被改变，要么被调用的函数只有一个基本块
                    //（4）要么函数调用的时候实参不能存在临时变量，要么被调用的函数只有一个基本块
                    //（5）如果函数调用有返回值，那么要么这个返回值不能是临时变量，要么被调用的函数只有一个出口（即该函数末尾）
                    //（6）函数调用的返回值不会用于IF_JMP和IF_NOT_JMP
                    r_params=(list<struct ic_data * > * )(*ic_with_info).intermediate_code.arg2.second;
                    // if(called_func_flow_graph->basic_blocks.size()>1)
                    // {
                    //     for(auto r_param:(*r_params))
                    //     {
                    //         if(r_param->is_tmp_var())
                    //         {
                    //             goto next;
                    //         }
                    //     }
                    // }
                    f_params=called_func->f_params;
                    for(auto f_param:(*f_params))
                    {
                        if(f_param->is_array_var())
                        {
                            goto next;
                        }
                    }
                    result=(struct ic_data *)(*ic_with_info).intermediate_code.result.second;
                    if((basic_block->ic_sequence.back().intermediate_code.op==ic_op::IF_JMP || basic_block->ic_sequence.back().intermediate_code.op==ic_op::IF_NOT_JMP) && 
                    basic_block->ic_sequence.back().intermediate_code.arg1.second==result)
                    {
                        goto next;
                    }
                    called_func_exit_num=called_func_flow_graph->get_exit_num();
                    // if(result && result->is_tmp_var() && called_func_exit_num>1)
                    // {
                    //     goto next;
                    // }
                    for(auto current_basic_block:func->basic_blocks)
                    {
                        for(auto current_ic_with_info:current_basic_block->ic_sequence)
                        {
                            for(auto current_ic_with_info_releated_var:current_ic_with_info.get_all_datas())
                            {
                                if(current_ic_with_info_releated_var->get_data_type()==language_data_type::FLOAT)
                                {
                                    goto next;
                                }
                            }
                        }
                    }
                    //获取那些会被改变的被调用函数形参
                    inline_func_scope=new ic_scope(current_func->scope);
                    inline_func_scope->set_scope_type(ic_scope_type::INLINE_FUNC);
                    inline_func_scope->func=called_func;
                    inline_var_num=0;
                    called_func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_f_params(called_func);
                    // for(auto called_func_def_global_or_f_param:called_func_def_globals_and_f_params)
                    // {
                    //     if(called_func_def_global_or_f_param->is_f_param())
                    //     {
                    //         // if(called_func_flow_graph->basic_blocks.size()>1)
                    //         // {
                    //         //     old_and_new_vars_map.clear();
                    //         //     goto next;
                    //         // }
                    //         // else
                    //         // {
                    //             old_and_new_vars_map.insert(make_pair(called_func_def_global_or_f_param,symbol_table->new_var(/*called_func_def_global_or_f_param->get_var_name()*/"<"+to_string(inline_var_num++)+">",called_func_def_global_or_f_param->get_data_type(),called_func_def_global_or_f_param->dimensions_len,called_func_def_global_or_f_param->get_value(),called_func_def_global_or_f_param->type==ic_data_type::CONST_FUNC_F_PARAM,inline_func_scope)));
                    //         // }
                    //     }
                    // }
                    for(auto f_param:*f_params)
                    {
                        old_and_new_vars_map.insert(make_pair(f_param,symbol_table->new_var(/*f_param->get_var_name()*/"<"+to_string(inline_var_num++)+">",f_param->get_data_type(),f_param->dimensions_len,f_param->get_value(),f_param->type==ic_data_type::CONST_FUNC_F_PARAM,inline_func_scope)));
                    }
                    //将要复制的流图中的变量和标签进行相应的替换，并更改其相应的作用域
                    for(auto called_basic_block:called_func_flow_graph->basic_blocks)
                    {
                        for(auto called_ic_with_info:called_basic_block->ic_sequence)
                        {
                            for(auto called_ic_with_info_releated_var:called_ic_with_info.get_all_datas())
                            {
                                if(called_ic_with_info_releated_var->get_data_type()==language_data_type::FLOAT)
                                {
                                    old_and_new_vars_map.clear();
                                    goto next;
                                }
                                if(old_and_new_vars_map.find(called_ic_with_info_releated_var)==old_and_new_vars_map.end())
                                {
                                    if(called_ic_with_info_releated_var->is_array_member())
                                    {
                                        offsets.insert(called_ic_with_info_releated_var->get_offset());
                                        continue;
                                    }
                                    else if(called_ic_with_info_releated_var->is_f_param() || called_ic_with_info_releated_var->is_global())
                                    {
                                        continue;
                                    }
                                    else if(called_ic_with_info_releated_var->is_const())
                                    {
                                        new_var=symbol_table->const_entry(called_ic_with_info_releated_var->get_data_type(),called_ic_with_info_releated_var->get_value());
                                    }
                                    else if(called_ic_with_info_releated_var->is_tmp_var())
                                    {
                                        new_var=symbol_table->new_tmp_var(called_ic_with_info_releated_var->get_data_type(),called_ic_with_info_releated_var->dimensions_len,called_ic_with_info_releated_var->get_value(),called_ic_with_info_releated_var->type==ic_data_type::CONST_TMP_VAR);
                                    }
                                    else
                                    {
                                        new_var=symbol_table->new_var(/*called_ic_with_info_releated_var->get_var_name()*/"<"+to_string(inline_var_num++)+">",called_ic_with_info_releated_var->get_data_type(),called_ic_with_info_releated_var->dimensions_len,called_ic_with_info_releated_var->get_value(),called_ic_with_info_releated_var->type==ic_data_type::LOCAL_CONST_VAR,inline_func_scope);
                                    }
                                    old_and_new_vars_map.insert(make_pair(called_ic_with_info_releated_var,new_var));
                                }
                            }
                            old_label=called_ic_with_info.get_related_label();
                            if(old_label && old_and_new_labels_map.find(old_label)==old_and_new_labels_map.end())
                            {
                                old_and_new_labels_map.insert(make_pair(old_label,symbol_table->new_label()));
                            }
                        }
                    }
                    f_param_iterator=f_params->begin();
                    r_param_iterator=r_params->begin();
                    while(f_param_iterator!=f_params->end() && r_param_iterator!=r_params->end())
                    {
                        //if(old_and_new_vars_map.find(*f_param_iterator)==old_and_new_vars_map.end())
                        if(called_func_def_globals_and_f_params.find(*f_param_iterator)==called_func_def_globals_and_f_params.end() && 
                        (offsets.find(*f_param_iterator)==offsets.end() || !(*r_param_iterator)->is_array_member()))
                        {
                            old_and_new_vars_map.at(*f_param_iterator)=*r_param_iterator;
                            //old_and_new_vars_map.insert(make_pair(*f_param_iterator,*r_param_iterator));
                        }
                        else
                        {
                            changed_f_params_map.insert(make_pair(old_and_new_vars_map.at(*f_param_iterator),*r_param_iterator));
                        }
                        f_param_iterator++;
                        r_param_iterator++;
                    }
                    //将被调用的函数的流图复制一份，并将里面需要替换的变量和标签全部进行替换
                    for(auto old_basic_block:called_func_flow_graph->basic_blocks)
                    {
                        new_basic_block=new ic_basic_block(*old_basic_block,func,old_and_new_vars_map,old_and_new_labels_map);
                        copyed_basic_blocks.push_back(new_basic_block);
                        old_and_new_ic_basic_block_map.insert(make_pair(old_basic_block,new_basic_block));
                    }
                    //再构建新拷贝出来的流图的跳转关系，并把拷贝出来的流图的所属函数流图进行更新
                    for(auto copyed_basic_block:copyed_basic_blocks)
                    {
                        // copyed_basic_block->set_sequential_next(old_and_new_ic_basic_block_map[copyed_basic_block->sequential_next]);
                        // copyed_basic_block->set_jump_next(old_and_new_ic_basic_block_map[copyed_basic_block->jump_next]);
                        if(old_and_new_ic_basic_block_map.find(copyed_basic_block->sequential_next)!=old_and_new_ic_basic_block_map.end())
                        {
                            copyed_basic_block->set_sequential_next(old_and_new_ic_basic_block_map.at(copyed_basic_block->sequential_next));
                        }
                        else
                        {
                            copyed_basic_block->set_sequential_next(nullptr);
                        }
                        if(old_and_new_ic_basic_block_map.find(copyed_basic_block->jump_next)!=old_and_new_ic_basic_block_map.end())
                        {
                            copyed_basic_block->set_jump_next(old_and_new_ic_basic_block_map.at(copyed_basic_block->jump_next));
                        }
                        else
                        {
                            copyed_basic_block->set_jump_next(nullptr);
                        }
                    }
                    //删除FUNC_DEFINE和END_FUNC_DEFINE中间代码
                    if(copyed_basic_blocks.front()->ic_sequence.front().intermediate_code.op==ic_op::FUNC_DEFINE)
                    {
                        copyed_basic_blocks.front()->ic_sequence.erase(copyed_basic_blocks.front()->ic_sequence.begin());
                    }
                    if(copyed_basic_blocks.back()->ic_sequence.back().intermediate_code.op==ic_op::END_FUNC_DEFINE)
                    {
                        copyed_basic_blocks.back()->ic_sequence.pop_back();
                    }
                    //将那些在内联函数中会被改变的函数形参进行处理
                    new_basic_block=copyed_basic_blocks.front();
                    for(auto changed_f_param:changed_f_params_map)
                    {
                        new_basic_block->add_ic_to_front(quaternion(ic_op::ASSIGN,ic_operand::DATA,(void *)changed_f_param.second,ic_operand::NONE,nullptr,ic_operand::DATA,(void *)changed_f_param.first));
                    }
                    //最后将复制完成的函数流图添加到当前函数流图中，再把所有的RET替换掉即可
                    another_ic_with_info=basic_block->ic_sequence.begin();
                    while(another_ic_with_info!=ic_with_info)
                    {
                        another_ic_with_info++;
                    }
                    another_ic_with_info=basic_block->ic_sequence.erase(another_ic_with_info);
                    if(called_func_exit_num>1)
                    {
                        new_basic_block=new ic_basic_block(func);
                        copyed_basic_blocks.push_back(new_basic_block);
                        new_label=symbol_table->new_label();
                        new_basic_block->add_ic(quaternion(ic_op::LABEL_DEFINE,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,(void *)new_label));
                        func->label_basic_block_map.insert(make_pair(new_label,new_basic_block));
                    }
                    else
                    {
                        new_basic_block=copyed_basic_blocks.back();
                    }
                    for(auto old_and_new_label:old_and_new_labels_map)
                    {
                        func->label_basic_block_map.insert(make_pair(old_and_new_label.second,old_and_new_ic_basic_block_map.at(called_func_flow_graph->label_basic_block_map.at(old_and_new_label.first))));
                    }
                    for(list<struct ic_basic_block * >::iterator copyed_basic_block=copyed_basic_blocks.begin();copyed_basic_block!=copyed_basic_blocks.end();copyed_basic_block++)
                    {
for_iterator:
                        for(vector<struct quaternion_with_info>::iterator ic_with_info_in_copyed_basic_block=(*copyed_basic_block)->ic_sequence.begin();ic_with_info_in_copyed_basic_block!=(*copyed_basic_block)->ic_sequence.end();ic_with_info_in_copyed_basic_block++)
                        {
                            if((*ic_with_info_in_copyed_basic_block).intermediate_code.op==ic_op::RET)
                            {
                                if((*ic_with_info_in_copyed_basic_block).intermediate_code.result.second && result)
                                {
                                    (*ic_with_info_in_copyed_basic_block)=quaternion_with_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,(*ic_with_info_in_copyed_basic_block).intermediate_code.result.second,ic_operand::NONE,nullptr,ic_operand::DATA,(void *)result));
                                }
                                else
                                {
                                    (*ic_with_info_in_copyed_basic_block)=quaternion_with_info(quaternion());
                                }
                                if(called_func_exit_num>1)
                                {
                                    if((*copyed_basic_block)!=copyed_basic_blocks.back())
                                    {
                                        (*copyed_basic_block)->add_ic(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,(void *)new_label));
                                        (*copyed_basic_block)->set_jump_next(new_basic_block);
                                        goto for_iterator;
                                    }
                                    else
                                    {
                                        (*copyed_basic_block)->set_sequential_next(new_basic_block);
                                    }
                                }
                            }
                        }
                    }
                    while(another_ic_with_info!=basic_block->ic_sequence.end())
                    {
                        new_basic_block_ic_sequence.push_back((*another_ic_with_info).intermediate_code);
                        another_ic_with_info=basic_block->ic_sequence.erase(another_ic_with_info);
                    }
                    for(auto ic_in_new_basic_block:new_basic_block_ic_sequence)
                    {
                        new_basic_block->add_ic(ic_in_new_basic_block);
                    }
                    new_basic_block->set_sequential_next(basic_block->sequential_next);
                    new_basic_block->set_jump_next(basic_block->jump_next);
                    basic_block->set_sequential_next(copyed_basic_blocks.front()->sequential_next);
                    basic_block->set_jump_next(copyed_basic_blocks.front()->jump_next);
                    for(auto new_ic_with_info:copyed_basic_blocks.front()->ic_sequence)
                    {
                        basic_block->add_ic(new_ic_with_info.intermediate_code);
                    }
                    delete copyed_basic_blocks.front();
                    copyed_basic_blocks.pop_front();
                    for(list<struct ic_basic_block * >::iterator bb=func->basic_blocks.begin();bb!=func->basic_blocks.end();bb++)
                    {
                        if((*bb)==basic_block)
                        {
                            func->basic_blocks.insert(++bb,copyed_basic_blocks.cbegin(),copyed_basic_blocks.cend());;
                            break;
                        }
                    }
                    if(called_func_exit_num>1 && !copyed_basic_blocks.empty())
                    {
                        copyed_basic_blocks.pop_back();
                    }
                    //清理现场，准备下一次的函数内联
                    if(copyed_basic_blocks.empty())
                    {
                        skip_until_basic_block=basic_block;
                    }
                    else
                    {
                        skip_until_basic_block=new_basic_block;
                    }
                    copyed_basic_blocks.clear();
                    old_and_new_ic_basic_block_map.clear();
                    changed_f_params_map.clear();
                    old_and_new_vars_map.clear();
                    old_and_new_labels_map.clear();
                    new_basic_block_ic_sequence.clear();
                    offsets.clear();
                    goto again;
                }
            }
next:
            ;
        }
    }
}

/*
窥孔优化

Parameters
----------
basic_block:要优化的基本块
*/
void peephole_optimization(struct ic_basic_block * basic_block)
{
    set<struct ic_data * > used_vars;
    struct ic_data * use_var,* def_var;
    vector<struct quaternion_with_info>::reverse_iterator temp;
    map<struct ic_data *,vector<struct quaternion_with_info>::reverse_iterator> assigns_optimize_inses;
    for(vector<struct quaternion_with_info>::reverse_iterator ic_with_info=basic_block->ic_sequence.rbegin();ic_with_info!=basic_block->ic_sequence.rend();ic_with_info++)
    {
        //在还没有做任何优化之前，临时变量的作用域不会出基本块，而且仅会被赋值一次
        if((*ic_with_info).intermediate_code.op==ic_op::ASSIGN)
        {
            use_var=(struct ic_data *)((*ic_with_info).intermediate_code.arg1.second);
            def_var=(*ic_with_info).explicit_def;
            if(use_var->is_tmp_var() && use_var->get_data_type()==def_var->get_data_type() && used_vars.find(use_var)==used_vars.end() && (!def_var->is_array_member() || def_var->get_offset()!=use_var))
            {
                temp=ic_with_info;
                advance(temp,1);
                if((*temp).explicit_def && (*temp).explicit_def==use_var)
                {
                    (*temp).replace_datas(use_var,def_var,false,true);
                    (*ic_with_info)=quaternion_with_info();
                }
                advance(temp,-1);
            }
        }
        set_union((*ic_with_info).uses.begin(),(*ic_with_info).uses.end(),used_vars.begin(),used_vars.end(),inserter(used_vars,used_vars.begin()));
    }
}

/*
DAG相关优化

Parameters
----------
basic_block:要优化的基本块
*/
void DAG_optimize(struct ic_basic_block * basic_block)
{
    DAG * dag;
    list<struct quaternion> basic_block_ic_sequence;
    //根据基本块建立DAG
    //在建立DAG的过程中完成：
    //强度削弱
    //常量合并
    //复制传播
    //局部公共子表达式删除
    dag=new DAG(basic_block);
    //对建立完成的DAG进行优化
    dag->optimize();
    //将DAG重新转换成基本块的中间代码
    basic_block_ic_sequence=dag->to_basic_block();
    delete dag;
    basic_block->clear_ic_sequence();
    for(auto ic:basic_block_ic_sequence)
    {
        basic_block->add_ic(ic);
    }
    //同时查看基本块的跳转情况是否需要改变
    if(basic_block_ic_sequence.back().op==ic_op::JMP && basic_block->jump_next && basic_block->sequential_next)
    {
        basic_block->sequential_next=nullptr;
    }
}