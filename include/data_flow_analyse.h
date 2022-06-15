/*
 *
 *  data_flow_analyse.h
 *  DAG构造器
 *  Creator:Sichao Chen
 *  Create time:2022/6/9
 *
*/
#ifndef __DATA_FLOW_ANALYSE_H
#define __DATA_FLOW_ANALYSE_H

#include"ic_optimizer.h"


//数据流分析的准备
void prepare_before_data_flow_analyse(struct ic_func_flow_graph * func);

//到达-定义分析
void use_define_analysis(struct ic_func_flow_graph * func);

//构建ud-链
void build_ud_chain(struct ic_func_flow_graph * func);

//活跃变量分析
void live_variable_analysis(struct ic_func_flow_graph * func);

//构建du-链
void build_du_chain(struct ic_func_flow_graph * func);

//可用表达式分析
void available_expression_analysis(struct ic_func_flow_graph * func);

#endif //__DATA_FLOW_ANALYSE_H