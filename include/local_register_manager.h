/*
 *
 *  local_register_manager.h
 *  局部寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/6
 *
*/
#ifndef __LOCAL_REGISTER_MANAGER_H
#define __LOCAL_REGISTER_MANAGER_H

#include"register_manager.h"

class Local_register_manager:public Register_manager
{
protected:
    //事件处理
    void handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block);
    void handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block);
    void handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block);

public:
    Local_register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    ~Local_register_manager();
};

#endif //__LOCAL_REGISTER_MANAGER_H
