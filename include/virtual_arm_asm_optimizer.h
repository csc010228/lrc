/*
 *
 *  virtual_arm_asm_optimizer.h
 *  arm架构下的虚拟目标代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/8/1
 *
*/
#ifndef __VIRTUAL_ARM_ASM_OPTIMIZER_H
#define __VIRTUAL_ARM_ASM_OPTIMIZER_H

#include"virtual_asm_optimizer.h"
#include"arm_asm.h"

//arm虚拟目标代码优化
class Virtual_arm_asm_optimizer:public Virtual_asm_optimizer
{
protected:
    //要优化的虚拟目标函数
    struct arm_func_flow_graph * virtual_target_code_;

    //优化函数和基本块的出入口代码
    void optimize_func_and_basic_block_s_enter_and_exit();

    //局部优化
    void local_optimize();

    //全局优化
    void global_optimize();

    void handle_OPTIMIZE(struct arm_func_flow_graph * virtual_target_code);

public:
    Virtual_arm_asm_optimizer(bool optimize);

    ~Virtual_arm_asm_optimizer();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);
};

#endif //__VIRTUAL_ARM_ASM_OPTIMIZER_H