/*
 *
 *  virtual_asm_optimizer.h
 *  虚拟目标代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/8/1
 *
*/
#ifndef __VIRTUAL_ASM_OPTIMIZER_H
#define __VIRTUAL_ASM_OPTIMIZER_H

#include"asm_generator_component.h"

//虚拟目标代码优化
class Virtual_asm_optimizer:public Asm_generator_component
{
protected:
    bool optimize_;
public:
    Virtual_asm_optimizer(bool optimize);

    virtual ~Virtual_asm_optimizer();
};

#endif //__VIRTUAL_ASM_OPTIMIZER_H