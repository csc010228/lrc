/*
 *
 *  intermediate_code_manager.h
 *  中间代码管理器
 *  Creator:Sichao Chen
 *  Create time:2022/4/19
 *
*/
#ifndef __INTERMEDIATE_CODE_MANAGER_H
#define __INTERMEDIATE_CODE_MANAGER_H

#include "asm_generator_component.h"
#include "ic_optimizer.h"

class Intermediate_code_manager:public Asm_generator_component
{

protected:
    //中间代码流图
    struct ic_flow_graph * intermediate_codes_flow_graph_;

    //当前正在转换成汇编的中间代码所在函数
    list<struct ic_func_flow_graph * >::iterator current_func_;

    //当前正在转换成汇编的中间代码所在基本块
    list<struct ic_basic_block * >::iterator current_basic_block_;

    //当前正在转换成汇编的中间代码的位置
    vector<struct quaternion_with_def_use_info>::iterator current_ic_;

    //事件处理函数
    struct event handle_NEXT_IC();

public:
    //构造函数
    Intermediate_code_manager();

    //析构函数
    //virtual ~Intermediate_code_manager();

    //初始化
    void init(struct ic_flow_graph * intermediate_codes_flow_graph);

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__INTERMEDIATE_CODE_MANAGER_H