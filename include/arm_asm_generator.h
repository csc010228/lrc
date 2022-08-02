/*
 *
 *  arm_asm_generator.h
 *  ARM的汇编代码生成
 *  Creator:Sichao Chen
 *  Create time:2021/12/04
 *
*/
#ifndef __ARM_ASM_GENERATOR_H
#define __ARM_ASM_GENERATOR_H

#include"asm_generator.h"

using namespace std;

//定义ARM汇编代码生成器对象
class Arm_asm_generator:public Asm_generator
{

private:
    bool create_instruction_generator(map<reg_index,string> regs_info);

    bool create_asm_optimizer(bool optimize);

    bool create_virtual_asm_optimizer(bool optimize);

    bool create_abi_manager();

    bool init_components();

public:
    //构造函数
    Arm_asm_generator();

    //析构函数
    ~Arm_asm_generator();

    //组件通知该汇编代码生成器有事件发生
    struct event notify(Asm_generator_component *sender, struct event event) const;

    //初始化
    bool init(bool optimize);
};

#endif //__ARM_ASM_GENERATOR_H