/*
 *
 *  abi_manager.h
 *  ABI函数管理者
 *  Creator:Sichao Chen
 *  Create time:2022/7/30
 *
*/
#ifndef __ABI_MANAGER_H
#define __ABI_MANAGER_H

#include"asm_generator_component.h"
#include<map>
#include<list>
#include<string>

using namespace std;

//ABI函数结构体
struct abi_func
{
    abi_func(string name,list<reg_index> param_regs):name(name),has_return_value(false),param_regs(param_regs)
    {

    };

    abi_func(string name,reg_index return_reg,list<reg_index> param_regs):name(name),has_return_value(true),return_reg(return_reg),param_regs(param_regs)
    {

    };

    string name;                                    //标识符名称字符串
    bool has_return_value;                          //是否有返回值
    reg_index return_reg;                           //返回值寄存器
    list<reg_index> param_regs;                   //函数形参寄存器
};


//ABI管理者
class Abi_manager:public Asm_generator_component
{
protected:
    //ABI函数
    map<string,struct abi_func> abi_funcs_;
public:
    Abi_manager();

    virtual ~Abi_manager();

    //初始化
    virtual bool init()=0;
};

#endif //__ABI_MANAGER_H