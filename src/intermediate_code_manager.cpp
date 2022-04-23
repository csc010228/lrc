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
    // static list<struct ic_func_flow_graph * >::iterator current_func;
    // static list<struct ic_basic_block * >::iterator current_basic_block;
    // static vector<struct quaternion_with_def_use_info>::iterator current_ic;
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
            current_func_=intermediate_codes_flow_graph_->func_flow_graphs.begin();
            while(current_func_!=intermediate_codes_flow_graph_->func_flow_graphs.end())
            {
                notify(event(event_type::START_FUNC,(void *)(*current_func_)->func));
                current_basic_block_=(*current_func_)->basic_blocks.begin();
                while(current_basic_block_!=(*current_func_)->basic_blocks.end())
                {
                    notify(event(event_type::START_BASIC_BLOCK,nullptr));
                    current_ic_=(*current_basic_block_)->ic_sequence.begin();
not_first_pos:
                    while(current_ic_!=(*current_basic_block_)->ic_sequence.end())
                    {
                        res.pointer_data=(void *)&((*current_ic_).intermediate_code);
                        current_ic_++;
                        return res;
                    }
                    notify(event(event_type::END_BASIC_BLOCK,nullptr));
                    current_basic_block_++;
                }
                notify(event(event_type::END_FUNC,nullptr));
                current_func_++;
            }
            end_tag=true;
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
        default:
            break;
    }
    return res;
}