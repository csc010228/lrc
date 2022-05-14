/*
 *
 *  instruction_generator.h
 *  汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/27
 *
*/
#ifndef __INSTRUCTION_GENERATOR_H
#define __INSTRUCTION_GENERATOR_H

#include "semantic_rules.h"
#include "asm_generator_component.h"
#include "ic_optimizer.h"

class Instruction_generator:public Asm_generator_component
{

protected:
    //有效位
    bool is_init_successful_;

    //最终的汇编代码文件
    list<string> asm_codes_;

    //生成最终的汇编文件序列,返回的是asm_codes_成员
    virtual void generate_asm_codes()=0;

    //获取下一条要翻译的中间代码
    pair<bool,struct quaternion> get_next_intermediate_code();

public:
    //寄存器信息
    static map<reg_index,string> regs_info_;

    //构造函数
    Instruction_generator(map<reg_index,string> regs_info);

    //析构函数
    virtual ~Instruction_generator();

    //检查是否初始化成功
    bool is_init_successful();

    //返回生成出来的汇编文件序列
    list<string> & generate_asm();

};

#endif //__INSTRUCTION_GENERATOR_H