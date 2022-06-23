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

    //利用arm数据处理指令能够更改标志位进行优化
    void data_process_instructions_change_flags(struct arm_basic_block * basic_block);
    
    //局部优化
    void local_optimize();

    //优化函数的出入口代码
    void optimize_func_enter_and_exit(struct arm_func_flow_graph * func);

    //利用arm的条件执行进行优化
    void conditional_execute(struct arm_func_flow_graph * func);

    //全局优化
    void global_optimize();

    //进行arm汇编优化
    void handle_OPTIMIZE(struct arm_flow_graph * arm_flow_graph);

public:
    //构造函数
    Arm_asm_optimizer(bool optimize);

    //析构函数
    ~Arm_asm_optimizer();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);
};

#endif //__ARM_ASM_OPTIMIZER_H