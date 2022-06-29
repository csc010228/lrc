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
    list<struct ic_func_flow_graph * >::iterator current_func_pos_;

    //当前正在转换成汇编的中间代码所在基本块
    list<struct ic_basic_block * >::iterator current_basic_block_pos_;

    //当前正在转换成汇编的中间代码的位置
    vector<struct quaternion_with_info>::iterator current_ic_with_info_pos_;

    //当前正在转换成汇编的中间代码
    struct quaternion_with_info current_ic_woth_info_;

    //所有的跨越基本块的临时变量
    map<struct ic_func * ,set<struct ic_data * > > temp_vars_over_basic_blocks_;

    //获取一个函数中所有的跨越基本块的临时变量
    void build_temp_vars_over_basic_blocks_info_in_func(struct ic_func_flow_graph * func);

    //事件处理函数
    struct event handle_NEXT_IC();
    struct event handle_IS_VAR_STILL_ALIVE(struct ic_data * var);
    struct event handle_IS_FUNC_NEED_PASS_PARAMS_BY_STACK(struct ic_func * func);
    struct event handle_GET_TEMP_VARS_OVER_BASIC_BLOCK(struct ic_func * func);
    struct event handle_IS_TEMP_VAR_OVER_BASIC_BLOCKS(struct ic_data * var);

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