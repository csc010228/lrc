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


class Data_flow_analyzer
{
private:
public:
    //数据流分析的准备
    static void prepare_before_data_flow_analyse(struct ic_func_flow_graph * func,bool further_analyse);

    //到达-定义分析
    static void use_define_analysis(struct ic_func_flow_graph * func);

    //构建ud-链
    static void build_ud_chain(struct ic_func_flow_graph * func);

    //活跃变量分析
    static void live_variable_analysis(struct ic_func_flow_graph * func);

    //构建du-链
    static void build_du_chain(struct ic_func_flow_graph * func);

    //可用表达式分析
    static void available_expression_analysis(struct ic_func_flow_graph * func);

    //构造支配点集
    static void build_dominate_relations(struct ic_func_flow_graph * func);

    //构造循环信息
    static void build_loops_info(struct ic_func_flow_graph * func);

    //更改ud链
    static void change_ud_chain(struct ic_func_flow_graph * func,ic_pos old_pos,ic_pos new_pos);

    //更改du链
    static void change_du_chain(struct ic_func_flow_graph * func,ic_pos old_pos,ic_pos new_pos);
};

#endif //__DATA_FLOW_ANALYSE_H