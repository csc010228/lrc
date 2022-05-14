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
                current_basic_block_pos_=(*current_func_pos_)->basic_blocks.begin();
                while(current_basic_block_pos_!=(*current_func_pos_)->basic_blocks.end())
                {
                    notify(event(event_type::START_BASIC_BLOCK,nullptr));
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
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=current_ic_with_info_pos_-2;ic_with_info!=(*current_basic_block_pos_)->ic_sequence.end();ic_with_info++)
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
        default:
            break;
    }
    return res;
}