/*
 *
 *  union_optimize.h
 *  联合优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#ifndef __UNION_OPTIMIZE_H
#define __UNION_OPTIMIZE_H

#include"ic_optimizer.h"

//多线程优化
void thread_optimize(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func);

#endif //__UNION_OPTIMIZE_H