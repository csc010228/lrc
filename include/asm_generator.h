/*
 *
 *  asm_generator.h
 *  汇编代码生成
 *  Creator:Sichao Chen
 *  Create time:2021/11/23
 *
*/
#ifndef __ASM_GENERATOR_H
#define __ASM_GENERATOR_H

#include "intermediate_code_manager.h"
#include "instruction_generator.h"
#include "register_manager.h"
#include "local_register_manager.h"
#include "global_register_manager.h"
#include "graph_coloring_register_manager.h"
#include "memory_manager.h"
#include "asm_optimizer.h"
#include "abi_manager.h"
#include "virtual_asm_optimizer.h"

//汇编代码生成器
class Asm_generator
{

protected:
    //中间代码管理器
    Intermediate_code_manager * intermediate_code_manager_;

    //汇编代码优化器
    Asm_optimizer * asm_optimizer_;

    //汇编指令生成器
    Instruction_generator * instruction_generator_;

    //寄存器管理器
    Register_manager * register_manager_;

    //速度更快的寄存器管理器
    Register_manager * easyer_register_manager_;

    //效果更好的寄存器
    Register_manager * better_register_manager_;

    //内存管理器
    Memory_manager * memory_manager_;

    //ABI管理器
    Abi_manager * abi_manager_;

    //初始化寄存器管理器
    virtual bool create_register_manager(set<struct reg> regs,struct flag_reg flag_reg,bool optimize);

    //初始化内存管理器
    virtual bool create_memory_manager(string memory_info);

    //初始化中间代码管理器
    virtual bool create_intermediate_code_manager();

    //初始化汇编指令优化器
    virtual bool create_asm_optimizer(bool optimize)=0;

    //初始化ABI管理器
    virtual bool create_abi_manager()=0;

    //初始化汇编指令生成器
    virtual bool create_instruction_generator(map<reg_index,string> regs_info)=0;

    //初始化各个组件
    virtual bool init_components()=0;

    //事件处理函数
    void handle_CHANGE_TO_EASYER_REGISTER_MANAGER();
    void handle_CHANGE_TO_BETTER_REGISTER_MANAGER();

public:
    //构造函数
    Asm_generator();

    //初始化
    bool init(set<struct reg> regs,struct flag_reg flag_reg,string memory_info,bool optimize);

    //析构函数
    virtual ~Asm_generator();

    //组件通知该汇编代码生成器有事件发生
    virtual struct event notify(Asm_generator_component *sender, struct event event) const =0 ;

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

    //生成最终的汇编代码文件
    bool generate_asm_then_output(struct ic_flow_graph * intermediate_codes_flow_graph,const char * filename);
};

#endif //__ASM_GENERATOR_H