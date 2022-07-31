/*
 *
 *  arm_abi_manager.cpp
 *  arm架构下的ABI函数管理者
 *  Creator:Sichao Chen
 *  Create time:2022/7/30
 *
*/
#include"arm_abi_manager.h"

Arm_abi_manager::Arm_abi_manager()
{

}

Arm_abi_manager::~Arm_abi_manager()
{

}

bool Arm_abi_manager::init()
{
    reg_index r0,r1,r2,r3,s0,s1;
    string reg_name;
    reg_name="r0";
    r0=(reg_index)notify(event(event_type::GET_REG_BY_NAME,(void *)(&reg_name))).int_data;
    reg_name="r1";
    r1=(reg_index)notify(event(event_type::GET_REG_BY_NAME,(void *)(&reg_name))).int_data;
    reg_name="r2";
    r2=(reg_index)notify(event(event_type::GET_REG_BY_NAME,(void *)(&reg_name))).int_data;
    reg_name="s0";
    s0=(reg_index)notify(event(event_type::GET_REG_BY_NAME,(void *)(&reg_name))).int_data;
    //初始化各个ABI函数的信息
    abi_funcs_.insert(make_pair("memset",abi_func("memset",{r0,r1,r2})));
    abi_funcs_.insert(make_pair("__aeabi_idiv",abi_func("__aeabi_idiv",r0,{r0,r1})));
    abi_funcs_.insert(make_pair("__aeabi_idivmod",abi_func("__aeabi_idivmod",r1,{r0,r1})));
    abi_funcs_.insert(make_pair("__aeabi_i2f",abi_func("__aeabi_i2f",r0,{r0})));
    abi_funcs_.insert(make_pair("__aeabi_f2iz",abi_func("__aeabi_f2iz",r0,{r0})));
    return true;
}

struct event Arm_abi_manager::hanlde_IS_AN_ABI_FUNC(string func_name)
{
    return event(event_type::RESPONSE_BOOL,(bool)(abi_funcs_.find(func_name)!=abi_funcs_.end()));
}

struct event Arm_abi_manager::hanlde_GET_AN_ABI_FUNC_S_PARAM_REGS(string func_name)
{
    return event(event_type::RESPONSE_POINTER,(void *)(&(abi_funcs_.at(func_name).param_regs)));
}

struct event Arm_abi_manager::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::IS_AN_ABI_FUNC:
            res=hanlde_IS_AN_ABI_FUNC(*((string *)event.pointer_data));
            break;
        case event_type::GET_AN_ABI_FUNC_S_PARAM_REGS:
            res=hanlde_GET_AN_ABI_FUNC_S_PARAM_REGS(*((string *)event.pointer_data));
            break;
        default:
            break;
    }
    return res;
}