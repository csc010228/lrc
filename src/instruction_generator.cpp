/*
 *
 *  instruction_generator.cpp
 *  汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#include "instruction_generator.h"

/*
构造函数

Parameters
----------
regs_info:寄存器信息
*/
Instruction_generator::Instruction_generator(map<reg_index,string> regs_info)
{
    regs_info_=regs_info;
    is_init_successful_=true;
}

/*
析构函数
*/
Instruction_generator::~Instruction_generator()
{

}

/*
检查是否初始化成功
*/
bool Instruction_generator::is_init_successful()
{
    return is_init_successful_;
}

/*
获取下一条要翻译的中间代码

Return
------
返回下一条要翻译的中间代码
*/
struct quaternion Instruction_generator::get_next_intermediate_code()
{
    struct quaternion res;
    struct quaternion * p_ic=(struct quaternion *)notify(event(event_type::NEXT_IC,nullptr)).pointer_data;
    if(p_ic)
    {
        res=*p_ic;
    }
    return res;
}

/*
返回生成出来的汇编文件序列

Return
------
返回asm_codes_成员的引用
*/
list<string> & Instruction_generator::generate_asm()
{
    generate_asm_codes();
    return asm_codes_;
}