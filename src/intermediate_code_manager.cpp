/*
 *
 *  intermediate_code_manager.cpp
 *  中间代码管理器
 *  Creator:Sichao Chen
 *  Create time:2022/4/19
 *
*/
#include "intermediate_code_manager.h"

/*
构造函数
*/
Intermediate_code_manager::Intermediate_code_manager():intermediate_codes_flow_graph_(nullptr)
{

}

// /*
// 析构函数
// */
// Intermediate_code_manager::~Intermediate_code_manager()
// {

// }

/*
初始化中间代码管理器

Parameters
----------
intermediate_codes_flow_graph:中间代码流图
*/
void Intermediate_code_manager::init(struct ic_flow_graph * intermediate_codes_flow_graph)
{
    intermediate_codes_flow_graph_=intermediate_codes_flow_graph;
}

/*
获取一个函数中所有的跨越基本块的临时变量

Parameters
----------
func:要获取信息的函数基本块流图
*/
void Intermediate_code_manager::build_temp_vars_over_basic_blocks_info_in_func(struct ic_func_flow_graph * func)
{
    map<struct ic_data *,struct ic_basic_block * > temp_vars_s_basic_block;
    for(auto basic_block:func->basic_blocks)
    {
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            for(auto var_in_ic_with_info:ic_with_info.get_all_datas())
            {
                if(var_in_ic_with_info->is_tmp_var())
                {
                    if(temp_vars_s_basic_block.find(var_in_ic_with_info)==temp_vars_s_basic_block.end())
                    {
                        temp_vars_s_basic_block.insert(make_pair(var_in_ic_with_info,basic_block));
                    }
                    if(temp_vars_s_basic_block.at(var_in_ic_with_info)!=basic_block)
                    {
                        map_set_insert(temp_vars_over_basic_blocks_,func->func,var_in_ic_with_info);
                    }
                }
            }
        }
    }
}

struct event Intermediate_code_manager::handle_NEXT_IC()
{
    const size_t func_s_max_ic_ins_num_threshold=500;
    Symbol_table * symbol_table=Symbol_table::get_instance();
    struct event res(event_type::RESPONSE_POINTER,nullptr);
    static list<struct quaternion>::iterator current_global_define=intermediate_codes_flow_graph_->global_defines.begin();
    static bool first_tag=true,end_tag=false;
    if(intermediate_codes_flow_graph_)
    {
        if(current_global_define!=intermediate_codes_flow_graph_->global_defines.end())
        {
            res.pointer_data=&(*current_global_define);
            current_global_define++;
        }
        else if(!end_tag)
        {
            if(!first_tag)
            {
                goto not_first_pos;
            }
            first_tag=false;
            current_func_pos_=intermediate_codes_flow_graph_->func_flow_graphs.begin();
            while(current_func_pos_!=intermediate_codes_flow_graph_->func_flow_graphs.end())
            {
                //根据函数的情况判断通知整个后端管理器应该选择哪一个寄存器分配器
                //如果不开启优化，获取开启了优化，但是一个函数涉及浮点操作，获取函数的长度过长，那么使用简单的寄存器分配器
                //其余的情况使用效果更优的寄存器分配器
                if(!symbol_table->get_optimize_setting() || (*current_func_pos_)->get_effective_ic_instruction_num()>func_s_max_ic_ins_num_threshold || symbol_table->is_func_related_to_a_data_type((*current_func_pos_)->func,language_data_type::FLOAT))
                {
                    notify(event(event_type::CHANGE_TO_EASYER_REGISTER_MANAGER,nullptr));
                }
                else
                {
                    notify(event(event_type::CHANGE_TO_BETTER_REGISTER_MANAGER,nullptr));
                }
                //开始一个新的函数
                notify(event(event_type::START_FUNC,(void *)(*current_func_pos_)));
                //每当开始一个新的函数，就获取该函数中所有会跨越基本块的临时变量
                build_temp_vars_over_basic_blocks_info_in_func(*current_func_pos_);
                current_basic_block_pos_=(*current_func_pos_)->basic_blocks.begin();
                while(current_basic_block_pos_!=(*current_func_pos_)->basic_blocks.end())
                {
                    notify(event(event_type::START_BASIC_BLOCK,(void *)(*current_basic_block_pos_)));
                    current_ic_with_info_pos_=(*current_basic_block_pos_)->ic_sequence.begin();
not_first_pos:
                    while(current_ic_with_info_pos_!=(*current_basic_block_pos_)->ic_sequence.end())
                    {
                        current_ic_woth_info_=(*current_ic_with_info_pos_);
                        res.pointer_data=(void *)&(current_ic_woth_info_.intermediate_code);
                        current_ic_with_info_pos_++;
                        return res;
                    }
                    notify(event(event_type::END_BASIC_BLOCK,(void *)(*current_basic_block_pos_)));
                    current_basic_block_pos_++;
                }
                notify(event(event_type::END_FUNC,nullptr));
                current_func_pos_++;
            }
            end_tag=true;
        }
    }
    return res;
}

struct event Intermediate_code_manager::handle_IS_VAR_STILL_ALIVE(struct ic_data * var)
{
    struct event res(event_type::RESPONSE_BOOL,true);
    if(var->is_tmp_var())
    {
        //暂时认为临时变量的作用域不会超过一个基本块
        res.bool_data=false;
        for(vector<struct quaternion_with_info>::iterator ic_with_info=current_ic_with_info_pos_-2;ic_with_info!=(*current_basic_block_pos_)->ic_sequence.end();ic_with_info++)
        {
            if((*ic_with_info).uses.find(var)!=(*ic_with_info).uses.end())
            {
                res.bool_data=true;
                break;
            }
        }
    }
    return res;
}

struct event Intermediate_code_manager::handle_IS_FUNC_NEED_PASS_PARAMS_BY_STACK(struct ic_func * func)
{
    static size_t cpu_argument_reg_num=notify(event(event_type::GET_CPU_ARGUMENT_REG_NUM,nullptr)).int_data;
    static size_t vfp_argument_reg_num=notify(event(event_type::GET_VFP_ARGUMENT_REG_NUM,nullptr)).int_data;
    size_t param_num_passed_by_cpu_reg=0,param_num_passed_by_vfp_reg=0;
    for(auto f_param:(*func->f_params))
    {
        if(f_param->is_array_var() || f_param->get_data_type()==language_data_type::INT)
        {
            param_num_passed_by_cpu_reg++;
            if(param_num_passed_by_cpu_reg>cpu_argument_reg_num)
            {
                return event(event_type::RESPONSE_BOOL,true);
            }
        }
        else if(f_param->get_data_type()==language_data_type::FLOAT)
        {
            param_num_passed_by_vfp_reg++;
            if(param_num_passed_by_vfp_reg>vfp_argument_reg_num)
            {
                return event(event_type::RESPONSE_BOOL,true);
            }
        }
    }
    return event(event_type::RESPONSE_BOOL,false);
}

struct event Intermediate_code_manager::handle_GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC()
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(temp_vars_over_basic_blocks_[(*current_func_pos_)->func])));
}

struct event Intermediate_code_manager::handle_IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC(struct ic_data * var)
{
    return event(event_type::RESPONSE_BOOL,temp_vars_over_basic_blocks_[(*current_func_pos_)->func].find(var)!=temp_vars_over_basic_blocks_[(*current_func_pos_)->func].end());
}

struct event Intermediate_code_manager::handle_GET_TEMP_VARS_IN_CURRENT_BASIC_BLOCK()
{
    set<struct ic_data * > * temp_vars_in_basic_block=new set<struct ic_data * >;
    for(auto ic_with_info:(*current_basic_block_pos_)->ic_sequence)
    {
        for(auto var:ic_with_info.get_all_datas())
        {
            if(var->is_tmp_var() && temp_vars_in_basic_block->find(var)==temp_vars_in_basic_block->end())
            {
                temp_vars_in_basic_block->insert(var);
            }
        }
    }
    return event(event_type::RESPONSE_POINTER,(void *)temp_vars_in_basic_block);
}

struct event Intermediate_code_manager::handle_GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC(struct ic_data * array)
{
    if((*current_func_pos_)->array_to_array_member_map.find(array)!=(*current_func_pos_)->array_to_array_member_map.end())
    {
        return event(event_type::RESPONSE_POINTER,(void *)(&((*current_func_pos_)->array_to_array_member_map.at(array))));
    }
    return event(event_type::RESPONSE_POINTER,nullptr);
}

struct event Intermediate_code_manager::handle_GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET(struct ic_data * offset)
{
    if((*current_func_pos_)->offset_to_array_member_map.find(offset)!=(*current_func_pos_)->offset_to_array_member_map.end())
    {
        return event(event_type::RESPONSE_POINTER,(void *)(&((*current_func_pos_)->offset_to_array_member_map.at(offset))));
    }
    return event(event_type::RESPONSE_POINTER,nullptr);
}

struct event Intermediate_code_manager::handle_GET_CURRENT_FUNC_S_F_PARAMS()
{
    return event(event_type::RESPONSE_POINTER,(void *)((*current_func_pos_)->func->f_params));
}

void Intermediate_code_manager::handle_INEFFECTIVE_CONST_VALUE_ANALYSIS(struct ic_func_flow_graph * func_flow_graph)
{
    //无效常量分析，目的是为了计算出每一个基本块开始的时候有哪些常量的值对应的虚拟寄存器是无效的
    map<struct ic_basic_block *,set<pair<OAA,enum language_data_type > > > gens,kills;
    set<pair<OAA,enum language_data_type > > all_const_datas,old_out;
    set<struct ic_basic_block * > precursors;
    bool tag=true;
    //清空所有基本块的in和out
    ineffective_const_value_info_.ins.clear();
    ineffective_const_value_info_.outs.clear();
    //计算每一个基本块的gen和kill集合
    //同时获取该函数中所有可能出现的常数
    for(auto basic_block:func_flow_graph->basic_blocks)
    {
        ineffective_const_value_info_.ins.insert(make_pair(basic_block,set<pair<OAA,enum language_data_type > >()));
        ineffective_const_value_info_.outs.insert(make_pair(basic_block,set<pair<OAA,enum language_data_type > >()));
        gens.insert(make_pair(basic_block,set<pair<OAA,enum language_data_type > >()));
        kills.insert(make_pair(basic_block,set<pair<OAA,enum language_data_type > >()));
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            for(auto data:ic_with_info.get_all_datas())
            {
                if(data->is_const())
                {
                    kills.at(basic_block).insert(make_pair(data->get_value(),data->get_data_type()));
                    all_const_datas.insert(make_pair(data->get_value(),data->get_data_type()));
                }
            }
        }
    }
    //然后再开始迭代计算每一个基本块的in和out
    while(tag)
    {
        tag=false;
        for(auto basic_block:func_flow_graph->basic_blocks)
        {
            precursors=basic_block->get_precursors();
            if(precursors.empty())
            {
                //第一个基本块的in就是该函数中所有可能出现的常数
                ineffective_const_value_info_.ins.at(basic_block)=all_const_datas;
            }
            else
            {
                for(auto precursor:precursors)
                {
                    set_union(ineffective_const_value_info_.outs.at(precursor).begin(),ineffective_const_value_info_.outs.at(precursor).end(),ineffective_const_value_info_.ins.at(basic_block).begin(),ineffective_const_value_info_.ins.at(basic_block).end(),inserter(ineffective_const_value_info_.ins.at(basic_block),ineffective_const_value_info_.ins.at(basic_block).begin()));
                }
            }
            old_out=ineffective_const_value_info_.outs.at(basic_block);
            ineffective_const_value_info_.outs.at(basic_block).clear();
            set_difference(ineffective_const_value_info_.ins.at(basic_block).begin(),ineffective_const_value_info_.ins.at(basic_block).end(),kills.at(basic_block).begin(),kills.at(basic_block).end(),inserter(ineffective_const_value_info_.outs.at(basic_block),ineffective_const_value_info_.outs.at(basic_block).begin()));
            set_union(ineffective_const_value_info_.outs.at(basic_block).begin(),ineffective_const_value_info_.outs.at(basic_block).end(),gens.at(basic_block).begin(),gens.at(basic_block).end(),inserter(ineffective_const_value_info_.outs.at(basic_block),ineffective_const_value_info_.outs.at(basic_block).begin()));
            if(old_out!=ineffective_const_value_info_.outs.at(basic_block))
            {
                tag=true;
            }
        }
    }
}

void Intermediate_code_manager::handle_INEFFECTIVE_VALUE_ANALYSIS(struct ic_func_flow_graph * func_flow_graph)
{
    // cout<<"INEFFECTIVE_VALUE_ANALYSIS"<<endl;
    //无效值分析，目的是为了计算出有哪些数据在基本块开始的时候是无效的
    //需要计算的数据如下：
    //全局变量
    //数组元素
    //非函数形参的数组，以及不放在寄存器中传递的函数数组形参
    //不放在寄存器中传递的函数形参
    map<struct ic_basic_block *,set<struct ic_data * > > gens,kills;
    set<struct ic_data * > members,old_out,all_cared_vars;
    struct ic_data * belong_array,* offset;
    set<struct ic_basic_block * > precursors;
    bool tag=true;
    set<struct ic_data * > * f_params_in_regs=(set<struct ic_data * > *)notify(event(event_type::GET_FUNC_S_F_PARAMS_IN_REGS,(void *)func_flow_graph->func)).pointer_data;
    //先清空所有基本块的in和out
    ineffective_value_info_.ins.clear();
    ineffective_value_info_.outs.clear();
    //首先计算每一个基本块的gen和kill集合
    //同时获取该函数中所有出现的全局变量和数组元素
    for(auto basic_block:func_flow_graph->basic_blocks)
    {
        ineffective_value_info_.ins.insert(make_pair(basic_block,set<struct ic_data * >()));
        ineffective_value_info_.outs.insert(make_pair(basic_block,set<struct ic_data * >()));
        gens.insert(make_pair(basic_block,set<struct ic_data * >()));
        kills.insert(make_pair(basic_block,set<struct ic_data * >()));
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            for(auto var:ic_with_info.uses)
            {
                //如果一个数据要被使用，那么它一定会被载入到寄存器中
                if(!var->is_const() && 
                (var->is_global() || 
                var->is_array_member() || 
                (var->is_array_var() && (!var->is_f_param() || f_params_in_regs->find(var)==f_params_in_regs->end())) || 
                (var->is_f_param() && f_params_in_regs->find(var)==f_params_in_regs->end())))
                {
                    all_cared_vars.insert(var);
                    if(gens.at(basic_block).find(var)!=gens.at(basic_block).end())
                    {
                        gens.at(basic_block).erase(var);
                    }
                    kills.at(basic_block).insert(var);
                }
            }
            for(auto var:ic_with_info.vague_defs)
            {
                if(!var->is_const() && 
                (var->is_global() || 
                var->is_array_member() || 
                (var->is_array_var() && (!var->is_f_param() || f_params_in_regs->find(var)==f_params_in_regs->end())) || 
                (var->is_f_param() && f_params_in_regs->find(var)==f_params_in_regs->end())))
                {
                    all_cared_vars.insert(var);
                }
                if(var->is_array_member())
                {
                    belong_array=var->get_belong_array();
                    if(func_flow_graph->array_to_array_member_map.find(belong_array)!=func_flow_graph->array_to_array_member_map.end())
                    {
                        members=func_flow_graph->array_to_array_member_map.at(belong_array);
                        for(auto member:members)
                        {
                            gens.at(basic_block).insert(member);
                        }
                    }
                }
                if(var->is_array_var())
                {
                    if(func_flow_graph->array_to_array_member_map.find(var)!=func_flow_graph->array_to_array_member_map.end())
                    {
                        members=func_flow_graph->array_to_array_member_map.at(var);
                        for(auto member:members)
                        {
                            gens.at(basic_block).insert(member);
                        }
                    }
                }
                if(var->is_global() && !var->is_array_var())
                {
                    gens.at(basic_block).insert(var);
                }
                offset=var;
                if(func_flow_graph->offset_to_array_member_map.find(offset)!=func_flow_graph->offset_to_array_member_map.end())
                {
                    members=func_flow_graph->offset_to_array_member_map.at(offset);
                    for(auto member:members)
                    {
                        gens.at(basic_block).insert(member);
                    }
                }
            }
            if(ic_with_info.explicit_def && !ic_with_info.explicit_def->is_const())
            {
                //如果一个数据被明确定义了，那么它的值此时会在寄存器中
                if(ic_with_info.explicit_def->is_array_member())
                {
                    //如果这个被明确定义的数据是一个数组元素，那么这个数组元素由于变成了脏值
                    //所以一定会在该基本块的最后被写回内存，那么在写回内存的时候一定会把数组变量和数组偏移量的值写入寄存器
                    belong_array=ic_with_info.explicit_def->get_belong_array();
                    if(belong_array->is_global() || f_params_in_regs->find(belong_array)==f_params_in_regs->end())
                    {
                        all_cared_vars.insert(belong_array);
                    }
                    if(!belong_array->is_f_param() || 
                    f_params_in_regs->find(belong_array)==f_params_in_regs->end())
                    {
                        if(gens.at(basic_block).find(belong_array)!=gens.at(basic_block).end())
                        {
                            gens.at(basic_block).erase(belong_array);
                        }
                        kills.at(basic_block).insert(belong_array);
                    }
                    offset=ic_with_info.explicit_def->get_offset();
                    if(offset->is_global() || 
                    (offset->is_f_param() && f_params_in_regs->find(offset)==f_params_in_regs->end()))
                    {
                        all_cared_vars.insert(offset);
                        if(gens.at(basic_block).find(offset)!=gens.at(basic_block).end())
                        {
                            gens.at(basic_block).erase(offset);
                        }
                        kills.at(basic_block).insert(offset);
                    }
                    //对一个数组元素进行明确定义的时候，会将同属于该数组的其他所有数组元素设置为无效
                    if(func_flow_graph->array_to_array_member_map.find(belong_array)!=func_flow_graph->array_to_array_member_map.end())
                    {
                        members=func_flow_graph->array_to_array_member_map.at(belong_array);
                        for(auto member:members)
                        {
                            if(member!=ic_with_info.explicit_def)
                            {
                                gens.at(basic_block).insert(member);
                            }
                            else
                            {
                                if(gens.at(basic_block).find(member)!=gens.at(basic_block).end())
                                {
                                    gens.at(basic_block).erase(member);
                                }
                                kills.at(basic_block).insert(member);
                            }
                        }
                    }
                }
                //对一个变量的明确定义也会将所有其他的以该变量为偏移量的数组元素设置为无效
                offset=ic_with_info.explicit_def;
                if(func_flow_graph->offset_to_array_member_map.find(offset)!=func_flow_graph->offset_to_array_member_map.end())
                {
                    members=func_flow_graph->offset_to_array_member_map.at(offset);
                    for(auto member:members)
                    {
                        gens.at(basic_block).insert(member);
                    }
                }
                //对一个变量的明确定义会使得该变量变为有效
                if(ic_with_info.explicit_def->is_array_member() || 
                ic_with_info.explicit_def->is_global() || 
                (ic_with_info.explicit_def->is_array_var() && (!ic_with_info.explicit_def->is_f_param() || f_params_in_regs->find(ic_with_info.explicit_def)==f_params_in_regs->end())) || 
                (ic_with_info.explicit_def->is_f_param() && f_params_in_regs->find(ic_with_info.explicit_def)==f_params_in_regs->end()))
                {
                    all_cared_vars.insert(ic_with_info.explicit_def);
                    if(gens.at(basic_block).find(ic_with_info.explicit_def)!=gens.at(basic_block).end())
                    {
                        gens.at(basic_block).erase(ic_with_info.explicit_def);
                    }
                    kills.at(basic_block).insert(ic_with_info.explicit_def);
                }
            }
        }
    }
    delete f_params_in_regs;
    // cout<<"gens:"<<endl;
    // for(auto i:gens)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl;
    // cout<<"kills:"<<endl;
    // for(auto i:kills)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl;
    //然后再开始迭代计算每一个基本块的in和out
    while(tag)
    {
        tag=false;
        for(auto basic_block:func_flow_graph->basic_blocks)
        {
            precursors=basic_block->get_precursors();
            if(precursors.empty())
            {
                ineffective_value_info_.ins.at(basic_block)=all_cared_vars;
            }
            else
            {
                ineffective_value_info_.ins.at(basic_block).clear();
                for(auto precursor:precursors)
                {
                    set_union(ineffective_value_info_.outs.at(precursor).begin(),ineffective_value_info_.outs.at(precursor).end(),ineffective_value_info_.ins.at(basic_block).begin(),ineffective_value_info_.ins.at(basic_block).end(),inserter(ineffective_value_info_.ins.at(basic_block),ineffective_value_info_.ins.at(basic_block).begin()));
                }
            }
            old_out=ineffective_value_info_.outs.at(basic_block);
            ineffective_value_info_.outs.at(basic_block).clear();
            set_difference(ineffective_value_info_.ins.at(basic_block).begin(),ineffective_value_info_.ins.at(basic_block).end(),kills.at(basic_block).begin(),kills.at(basic_block).end(),inserter(ineffective_value_info_.outs.at(basic_block),ineffective_value_info_.outs.at(basic_block).begin()));
            set_union(ineffective_value_info_.outs.at(basic_block).begin(),ineffective_value_info_.outs.at(basic_block).end(),gens.at(basic_block).begin(),gens.at(basic_block).end(),inserter(ineffective_value_info_.outs.at(basic_block),ineffective_value_info_.outs.at(basic_block).begin()));
            if(old_out!=ineffective_value_info_.outs.at(basic_block))
            {
                tag=true;
            }
        }
    }
    // cout<<"ins:"<<endl;
    // for(auto i:ineffective_value_info_.ins)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl;
    // cout<<"outs:"<<endl;
    // for(auto i:ineffective_value_info_.outs)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl<<endl<<endl;
}

void Intermediate_code_manager::handle_DIRTY_VALUE_ANALYSIS(struct ic_func_flow_graph * func_flow_graph)
{
    // cout<<"DIRTY_VALUE_ANALYSIS"<<endl;
    //脏值分析，目的是为了计算出每一个基本块开始和退出的时候有哪些我们关注的值在虚拟寄存器中可以是（也可以不是）脏值
    //我们关注的变量有：
    //非数组的全局变量
    //放在内存中传递的非数组函数形参
    // //脏值非数组全局变量分析，目的是为了计算出每一个基本块开始和退出的时候有哪些非数组全局变量的值在虚拟寄存器中可以（也可以不是）是脏值
    set<struct ic_basic_block * > successors;
    set<struct ic_data * > all_not_array_globals,temp;
    bool first_set_intersection;
    set<struct ic_data * > * f_params_in_regs=(set<struct ic_data * > *)notify(event(event_type::GET_FUNC_S_F_PARAMS_IN_REGS,(void *)func_flow_graph->func)).pointer_data;
    //在求进入基本块的时候是是脏值的变量之前，需要先完成到达定义分析
    //如果存在一个变量的定义能够到达一个基本块的入口处，那么这个变量在该基本块的入口处就是脏值
    dirty_not_array_global_value_info_.ins.clear();
    dirty_not_array_global_value_info_.outs.clear();
    for(auto basic_block:func_flow_graph->basic_blocks)
    {
        dirty_not_array_global_value_info_.ins.insert(make_pair(basic_block,set<struct ic_data * >()));
        for(auto def_pos:basic_block->use_def_analysis_info.in)
        {
            if((def_pos.first->is_global() && !def_pos.first->is_array_var()) || (def_pos.first->is_f_param() && !def_pos.first->is_array_var() && f_params_in_regs->find(def_pos.first)==f_params_in_regs->end()))
            {
                dirty_not_array_global_value_info_.ins.at(basic_block).insert(def_pos.first);
            }
        }
    }
    //在求退出基本块的时候能够继续维持脏值的变量之前，需要先完成有效变量分析
    for(auto var:ineffective_value_info_.ins.at(func_flow_graph->basic_blocks.front()))
    {
        if((var->is_global() && !var->is_array_var()) || (var->is_f_param() && !var->is_array_var() && f_params_in_regs->find(var)==f_params_in_regs->end()))
        {
            all_not_array_globals.insert(var);
        }
    }
    delete f_params_in_regs;
    //遍历基本块的所有后继，获取这些基本块入口处的无效变量，对这些无效变量取并集，得到的结果就是本基本块退出的时候不能是脏值的全局变量
    for(auto basic_block:func_flow_graph->basic_blocks)
    {
        dirty_not_array_global_value_info_.outs.insert(make_pair(basic_block,set<struct ic_data * >()));
        successors=basic_block->get_successors();
        temp.clear();
        for(auto successor:successors)
        {
            set_union(dirty_not_array_global_value_info_.outs.at(basic_block).begin(),dirty_not_array_global_value_info_.outs.at(basic_block).end(),ineffective_value_info_.ins.at(successor).begin(),ineffective_value_info_.ins.at(successor).end(),inserter(temp,temp.begin()));
        }
        set_difference(all_not_array_globals.begin(),all_not_array_globals.end(),temp.begin(),temp.end(),inserter(dirty_not_array_global_value_info_.outs.at(basic_block),dirty_not_array_global_value_info_.outs.at(basic_block).begin()));
    }
    // cout<<"ins:"<<endl;
    // for(auto i:dirty_not_array_global_value_info_.ins)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl;
    // cout<<"outs:"<<endl;
    // for(auto i:dirty_not_array_global_value_info_.outs)
    // {
    //     cout<<i.first<<":{";
    //     for(auto j:i.second)
    //     {
    //         if(j->is_const())
    //         {
    //             cout<<j->get_value().int_data<<",";
    //         }
    //         else if(j->is_tmp_var())
    //         {
    //             cout<<"@"<<j->tmp_index<<",";
    //         }
    //         else if(j->is_array_member())
    //         {
    //             cout<<j->get_belong_array()->get_var_name()<<"[";
    //             if(j->get_offset()->is_const())
    //             {
    //                 cout<<j->get_offset()->get_value().int_data;
    //             }
    //             else if(j->get_offset()->is_tmp_var())
    //             {
    //                 cout<<"@"<<j->get_offset()->tmp_index;
    //             }
    //             else
    //             {
    //                 cout<<j->get_offset()->get_var_name();
    //             }
    //             cout<<"],";
    //         }
    //         else
    //         {
    //             cout<<j->get_var_name()<<",";
    //         }
    //     }
    //     cout<<"}"<<endl;
    // }
    // cout<<endl<<endl<<endl;
}

void Intermediate_code_manager::handle_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS(struct ic_func_flow_graph * func_flow_graph)
{
//     //TO-DO
//     //这里的分析结果是错误的，需要重写代码
//     cout<<"INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS"<<endl;
//     //无效非数组全局变量地址分析，目的是为了计算出每一个基本块开始的时候有哪些非数组全局变量的地址对应的虚拟寄存器是无效的
//     //要求先完成脏值非数组全局变量分析
//     map<struct ic_basic_block *,set<struct ic_data * > > gens,kills;
//     set<struct ic_data * > old_out,all_not_array_globals;
//     set<struct ic_basic_block * > precursors;
//     set<struct ic_data * > explicit_def_globals;
//     bool tag=true;
//     //先清空所有基本块的in和out
//     ineffective_not_array_global_addr_info_.ins.clear();
//     ineffective_not_array_global_addr_info_.outs.clear();
//     //首先计算每一个基本块的gen和kill集合
//     //同时获取该函数中出现的所有全局变量
//     for(auto basic_block:func_flow_graph->basic_blocks)
//     {
//         ineffective_not_array_global_addr_info_.ins.insert(make_pair(basic_block,set<struct ic_data * >()));
//         ineffective_not_array_global_addr_info_.outs.insert(make_pair(basic_block,set<struct ic_data * >()));
//         gens.insert(make_pair(basic_block,set<struct ic_data * >()));
//         kills.insert(make_pair(basic_block,set<struct ic_data * >()));
//         for(auto ic_with_info:basic_block->ic_sequence)
//         {
//             for(auto var:ic_with_info.uses)
//             {
//                 if(var->is_global() && !var->is_array_var())
//                 {
//                     all_not_array_globals.insert(var);
//                     kills.at(basic_block).insert(var);
//                 }
//             }
//             set_intersection(ic_with_info.vague_defs.begin(),ic_with_info.vague_defs.end(),explicit_def_globals.begin(),explicit_def_globals.end(),inserter(kills.at(basic_block),kills.at(basic_block).begin()));
//             if(ic_with_info.explicit_def && ic_with_info.explicit_def->is_global() && !ic_with_info.explicit_def->is_array_var())
//             {
//                 all_not_array_globals.insert(ic_with_info.explicit_def);
//                 explicit_def_globals.insert(ic_with_info.explicit_def);
//             }
//         }
//     }
//     cout<<"gens:"<<endl;
//     for(auto i:gens)
//     {
//         cout<<i.first<<":{";
//         for(auto j:i.second)
//         {
//             if(j->is_const())
//             {
//                 cout<<j->get_value().int_data<<",";
//             }
//             else if(j->is_tmp_var())
//             {
//                 cout<<"@"<<j->tmp_index<<",";
//             }
//             else if(j->is_array_member())
//             {
//                 cout<<j->get_belong_array()->get_var_name()<<"[";
//                 if(j->get_offset()->is_const())
//                 {
//                     cout<<j->get_offset()->get_value().int_data;
//                 }
//                 else if(j->get_offset()->is_tmp_var())
//                 {
//                     cout<<"@"<<j->get_offset()->tmp_index;
//                 }
//                 else
//                 {
//                     cout<<j->get_offset()->get_var_name();
//                 }
//                 cout<<"],";
//             }
//             else
//             {
//                 cout<<j->get_var_name()<<",";
//             }
//         }
//         cout<<"}"<<endl;
//     }
//     cout<<endl;
//     cout<<"kills:"<<endl;
//     for(auto i:kills)
//     {
//         cout<<i.first<<":{";
//         for(auto j:i.second)
//         {
//             if(j->is_const())
//             {
//                 cout<<j->get_value().int_data<<",";
//             }
//             else if(j->is_tmp_var())
//             {
//                 cout<<"@"<<j->tmp_index<<",";
//             }
//             else if(j->is_array_member())
//             {
//                 cout<<j->get_belong_array()->get_var_name()<<"[";
//                 if(j->get_offset()->is_const())
//                 {
//                     cout<<j->get_offset()->get_value().int_data;
//                 }
//                 else if(j->get_offset()->is_tmp_var())
//                 {
//                     cout<<"@"<<j->get_offset()->tmp_index;
//                 }
//                 else
//                 {
//                     cout<<j->get_offset()->get_var_name();
//                 }
//                 cout<<"],";
//             }
//             else
//             {
//                 cout<<j->get_var_name()<<",";
//             }
//         }
//         cout<<"}"<<endl;
//     }
//     cout<<endl;
//     //然后再开始迭代计算每一个基本块的in和out
//     while(tag)
//     {
//         tag=false;
//         for(auto basic_block:func_flow_graph->basic_blocks)
//         {
//             precursors=basic_block->get_precursors();
//             if(precursors.empty())
//             {
//                 ineffective_not_array_global_addr_info_.ins.at(basic_block)=all_not_array_globals;
//             }
//             else
//             {
//                 ineffective_not_array_global_addr_info_.ins.at(basic_block).clear();
//                 for(auto precursor:precursors)
//                 {
//                     set_union(ineffective_not_array_global_addr_info_.outs.at(precursor).begin(),ineffective_not_array_global_addr_info_.outs.at(precursor).end(),ineffective_not_array_global_addr_info_.ins.at(basic_block).begin(),ineffective_not_array_global_addr_info_.ins.at(basic_block).end(),inserter(ineffective_not_array_global_addr_info_.ins.at(basic_block),ineffective_not_array_global_addr_info_.ins.at(basic_block).begin()));
//                 }
//             }
//             old_out=ineffective_not_array_global_addr_info_.outs.at(basic_block);
//             ineffective_not_array_global_addr_info_.outs.at(basic_block).clear();
//             set_difference(ineffective_not_array_global_addr_info_.ins.at(basic_block).begin(),ineffective_not_array_global_addr_info_.ins.at(basic_block).end(),kills.at(basic_block).begin(),kills.at(basic_block).end(),inserter(ineffective_not_array_global_addr_info_.outs.at(basic_block),ineffective_not_array_global_addr_info_.outs.at(basic_block).begin()));
//             set_union(ineffective_not_array_global_addr_info_.outs.at(basic_block).begin(),ineffective_not_array_global_addr_info_.outs.at(basic_block).end(),gens.at(basic_block).begin(),gens.at(basic_block).end(),inserter(ineffective_not_array_global_addr_info_.outs.at(basic_block),ineffective_not_array_global_addr_info_.outs.at(basic_block).begin()));
//             if(old_out!=ineffective_not_array_global_addr_info_.outs.at(basic_block))
//             {
//                 tag=true;
//             }
//         }
//     }
//     cout<<"ins:"<<endl;
//     for(auto i:ineffective_not_array_global_addr_info_.ins)
//     {
//         cout<<i.first<<":{";
//         for(auto j:i.second)
//         {
//             if(j->is_const())
//             {
//                 cout<<j->get_value().int_data<<",";
//             }
//             else if(j->is_tmp_var())
//             {
//                 cout<<"@"<<j->tmp_index<<",";
//             }
//             else if(j->is_array_member())
//             {
//                 cout<<j->get_belong_array()->get_var_name()<<"[";
//                 if(j->get_offset()->is_const())
//                 {
//                     cout<<j->get_offset()->get_value().int_data;
//                 }
//                 else if(j->get_offset()->is_tmp_var())
//                 {
//                     cout<<"@"<<j->get_offset()->tmp_index;
//                 }
//                 else
//                 {
//                     cout<<j->get_offset()->get_var_name();
//                 }
//                 cout<<"],";
//             }
//             else
//             {
//                 cout<<j->get_var_name()<<",";
//             }
//         }
//         cout<<"}"<<endl;
//     }
//     cout<<endl;
//     cout<<"outs:"<<endl;
//     for(auto i:ineffective_not_array_global_addr_info_.outs)
//     {
//         cout<<i.first<<":{";
//         for(auto j:i.second)
//         {
//             if(j->is_const())
//             {
//                 cout<<j->get_value().int_data<<",";
//             }
//             else if(j->is_tmp_var())
//             {
//                 cout<<"@"<<j->tmp_index<<",";
//             }
//             else if(j->is_array_member())
//             {
//                 cout<<j->get_belong_array()->get_var_name()<<"[";
//                 if(j->get_offset()->is_const())
//                 {
//                     cout<<j->get_offset()->get_value().int_data;
//                 }
//                 else if(j->get_offset()->is_tmp_var())
//                 {
//                     cout<<"@"<<j->get_offset()->tmp_index;
//                 }
//                 else
//                 {
//                     cout<<j->get_offset()->get_var_name();
//                 }
//                 cout<<"],";
//             }
//             else
//             {
//                 cout<<j->get_var_name()<<",";
//             }
//         }
//         cout<<"}"<<endl;
//     }
//     cout<<endl<<endl<<endl;
}

struct event Intermediate_code_manager::handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_CONST_VALUE_ANALYSIS_INFO(struct ic_basic_block * basic_block)
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(ineffective_const_value_info_.ins[basic_block])));
}

struct event Intermediate_code_manager::handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_VALUE_ANALYSIS_INFO(struct ic_basic_block * basic_block)
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(ineffective_value_info_.ins[basic_block])));
}

struct event Intermediate_code_manager::handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS_INFO(struct ic_basic_block * basic_block)
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(ineffective_not_array_global_addr_info_.ins[basic_block])));
}

struct event Intermediate_code_manager::handle_GET_BASIC_BLOCK_ENTER_S_DIRTY_VALUE_ANALYSIS_INFO(struct ic_basic_block * basic_block)
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(dirty_not_array_global_value_info_.ins[basic_block])));
}

struct event Intermediate_code_manager::handle_GET_CURRENT_BASIC_BLOCK_EXIT_S_DIRTY_VALUE_ANALYSIS_INFO()
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(dirty_not_array_global_value_info_.outs[*current_basic_block_pos_])));
}

struct event Intermediate_code_manager::handle_IS_A_START_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    return event(event_type::RESPONSE_BOOL,((*(basic_block->belong_func_flow_graph->basic_blocks.begin()))==basic_block));
}

struct event Intermediate_code_manager::handle_GET_CURRENT_BASIC_BLOCK()
{
    return event(event_type::RESPONSE_POINTER,(void *)(*current_basic_block_pos_));
}

/*
事件处理函数(由中介者进行调用)

Parameters
----------
event:传入的事件

Return
------
返回的事件
*/
struct event Intermediate_code_manager::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::NEXT_IC:
            res=handle_NEXT_IC();
            break;
        case event_type::IS_VAR_STILL_ALIVE:
            res=handle_IS_VAR_STILL_ALIVE((struct ic_data *)event.pointer_data);
            break;
        case event_type::IS_FUNC_NEED_PASS_PARAMS_BY_STACK:
            res=handle_IS_FUNC_NEED_PASS_PARAMS_BY_STACK((struct ic_func *)event.pointer_data);
            break;
        case event_type::GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC:
            res=handle_GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC();
            break;
        case event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC:
            res=handle_IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_TEMP_VARS_IN_CURRENT_BASIC_BLOCK:
            res=handle_GET_TEMP_VARS_IN_CURRENT_BASIC_BLOCK();
            break;
        case event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC:
            res=handle_GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET:
            res=handle_GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_CURRENT_FUNC_S_F_PARAMS:
            res=handle_GET_CURRENT_FUNC_S_F_PARAMS();
            break;
        case event_type::INEFFECTIVE_CONST_VALUE_ANALYSIS:
            handle_INEFFECTIVE_CONST_VALUE_ANALYSIS((struct ic_func_flow_graph * )event.pointer_data);
            break;
        case event_type::INEFFECTIVE_VALUE_ANALYSIS:
            handle_INEFFECTIVE_VALUE_ANALYSIS((struct ic_func_flow_graph * )event.pointer_data);
            break;
        case event_type::INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS:
            handle_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS((struct ic_func_flow_graph * )event.pointer_data);
            break;
        case event_type::DIRTY_VALUE_ANALYSIS:
            handle_DIRTY_VALUE_ANALYSIS((struct ic_func_flow_graph * )event.pointer_data);
            break;
        case event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_CONST_VALUE_ANALYSIS_INFO:
            res=handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_CONST_VALUE_ANALYSIS_INFO((struct ic_basic_block * )event.pointer_data);
            break;
        case event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_VALUE_ANALYSIS_INFO:
            res=handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_VALUE_ANALYSIS_INFO((struct ic_basic_block * )event.pointer_data);
            break;
        case event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS_INFO:
            res=handle_GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS_INFO((struct ic_basic_block * )event.pointer_data);
            break;
        case event_type::GET_BASIC_BLOCK_ENTER_S_DIRTY_VALUE_ANALYSIS_INFO:
            res=handle_GET_BASIC_BLOCK_ENTER_S_DIRTY_VALUE_ANALYSIS_INFO((struct ic_basic_block * )event.pointer_data);
            break;
        case event_type::GET_CURRENT_BASIC_BLOCK_EXIT_S_DIRTY_VALUE_ANALYSIS_INFO:
            res=handle_GET_CURRENT_BASIC_BLOCK_EXIT_S_DIRTY_VALUE_ANALYSIS_INFO();
            break;
        case event_type::IS_A_START_BASIC_BLOCK:
            res=handle_IS_A_START_BASIC_BLOCK((struct ic_basic_block *)event.pointer_data);
            break;
        case event_type::GET_CURRENT_BASIC_BLOCK:
            res=handle_GET_CURRENT_BASIC_BLOCK();
            break;
        default:
            break;
    }
    return res;
}