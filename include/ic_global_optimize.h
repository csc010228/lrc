/*
 *
 *  ic_global_optimize.h
 *  中间代码全局优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#ifndef __IC_GLOBAL_OPTIMIZE_H
#define __IC_GLOBAL_OPTIMIZE_H

#include"ic_optimizer.h"

//全局复制传播
void global_copy_progagation(struct ic_func_flow_graph * func);

//全局公共子表达式删除
void global_elimination_of_common_subexpression(struct ic_func_flow_graph * func);

//全局死代码消除
void global_dead_code_elimination(struct ic_func_flow_graph * func);

//循环不变量外提
void loop_invariant_computation_motion(struct ic_func_flow_graph * func);

//归纳变量删除
void induction_variable_elimination(struct ic_func_flow_graph * func);

#endif //__IC_GLOBAL_OPTIMIZE_H