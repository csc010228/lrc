/*
 *
 *  arm_abi_manager.h
 *  arm架构下的ABI函数管理者
 *  Creator:Sichao Chen
 *  Create time:2022/7/30
 *
*/
#ifndef __ARM_ABI_MANAGER_H
#define __ARM_ABI_MANAGER_H

#include"abi_manager.h"

//arm架构下的ABI管理者
class Arm_abi_manager:public Abi_manager
{

protected:
    struct event hanlde_IS_AN_ABI_FUNC(string func_name);
    struct event hanlde_GET_AN_ABI_FUNC_S_PARAM_REGS(string func_name);

public:
    Arm_abi_manager();

    ~Arm_abi_manager();

    //初始化
    bool init();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);
};

#endif //__ARM_ABI_MANAGER_H