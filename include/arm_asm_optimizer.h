/*
 *
 *  arm_asm_optimizer.h
 *  arm汇编代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/4/24
 *
*/
#ifndef __ARM_ASM_OPTIMIZER_H
#define __ARM_ASM_OPTIMIZER_H

#include "asm_optimizer.h"
#include "arm_asm.h"

class Arm_asm_optimizer:public Asm_optimizer
{
private:
    //要优化的arm汇编流图
    struct arm_flow_graph * arm_flow_graph_;

    //局部优化
    void local_optimize();

    //优化函数的出入口代码
    void optimize_func_enter_and_exit(struct arm_func_flow_graph * func);

    //全局优化
    void global_optimize();

    //进行arm汇编优化
    void handle_OPTIMIZE(struct arm_flow_graph * arm_flow_graph);

public:
    //构造函数
    Arm_asm_optimizer();

    //析构函数
    ~Arm_asm_optimizer();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);
};

#endif //__ARM_ASM_OPTIMIZER_H