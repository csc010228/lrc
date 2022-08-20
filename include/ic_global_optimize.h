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

//循环展开
void loop_unroll(struct ic_func_flow_graph * func);

//局部数组全局化
void globalize_local_arrays(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func);

//有价值的代码保留
void global_valuable_code_reserve(struct ic_func_flow_graph * func);

// 空分支删除, 并进行相应的分支结构调整
void empty_branch_del(struct ic_func_flow_graph * func);

#endif //__IC_GLOBAL_OPTIMIZE_H