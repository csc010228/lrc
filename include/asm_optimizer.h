/*
 *
 *  asm_optimizer.h
 *  汇编代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/4/27
 *
*/
#ifndef __ASM_OPTIMIZER_H
#define __ASM_OPTIMIZER_H

#include "asm_generator_component.h"

class Asm_optimizer:public Asm_generator_component
{
private:

public:
    //构造函数
    Asm_optimizer();

    //析构函数
    virtual ~Asm_optimizer();
};

#endif //__ASM_OPTIMIZER_H