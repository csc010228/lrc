/*
 *
 *  ic_local_optimize.h
 *  中间代码局部优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#ifndef __IC_LOCAL_OPTIMIZE_H
#define __IC_LOCAL_OPTIMIZE_H

#include"ic_optimizer.h"

//删除无用的函数返回值
void remove_useless_return();

//窥孔优化
void peephole_optimization(struct ic_basic_block * basic_block);

//函数内联
void function_inline(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func);

//DAG相关优化
void DAG_optimize(struct ic_basic_block * basic_block);

#endif //__IC_LOCAL_OPTIMIZE_H