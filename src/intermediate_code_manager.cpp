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
                notify(event(event_type::START_FUNC,(void *)(*current_func_pos_)->func));
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
                    notify(event(event_type::END_BASIC_BLOCK,nullptr));
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
        default:
            break;
    }
    return res;
}