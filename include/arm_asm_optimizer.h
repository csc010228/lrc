/*
 *
 *  arm_asm_optimizer.h
 *  arm汇编代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/4/24
 *
*/
#ifndef __ARM_ASM_OPTIMIZER_H
#define __ARM_ASM_OPTIMIZER_H

#include "arm_asm.h"
#include "symbol_table.h"

//arm流图中的基本块
struct arm_basic_block
{
    arm_basic_block()
    {
        sequential_next=nullptr;
        jump_next=nullptr;
    };

    ~arm_basic_block()
    {
        for(auto i:arm_sequence)
        {
            delete i;
        }
    };

    //往汇编函数流图中添加一条arm汇编
    void add_arm_asm(Arm_asm_file_line * arm_asm)
    {
        arm_sequence.push_back(arm_asm);
    };

    //转换成字符串
    list<string> to_string()
    {
        list<string> res;
        for(auto i:arm_sequence)
        {
            res.push_back(i->to_string());
        }
        return res;
    };

    list<Arm_asm_file_line * > arm_sequence;
    struct arm_basic_block * sequential_next,* jump_next;
};

//arm汇编函数的流图
struct arm_func_flow_graph
{
    arm_func_flow_graph(struct ic_func * function):function(function)
    {
    };

    ~arm_func_flow_graph()
    {
        for(auto i:basic_blocks)
        {
            delete i;
        }
    };

    //往汇编函数流图中添加一条arm汇编
    void add_arm_asm(Arm_asm_file_line * arm_asm,bool new_basic_block)
    {
        static struct arm_basic_block * current_arm_basic_block=nullptr;
        if(new_basic_block)
        {
            current_arm_basic_block=new struct arm_basic_block;
            basic_blocks.push_back(current_arm_basic_block);
        }
        if(arm_asm)
        {
            current_arm_basic_block->add_arm_asm(arm_asm);
        }
    };

    //转换成字符串
    list<string> to_string()
    {
        string func_name=function->name;
        list<string> res;
        res.push_back("\t.text");
        res.push_back("\t.align 1");
        res.push_back("\t.global	__aeabi_idiv");
        res.push_back("\t.global	__aeabi_idivmod");
        res.push_back("\t.global "+func_name);
        res.push_back("\t.syntax unified");
        res.push_back("\t.thumb");
        res.push_back("\t.thumb_func");
        res.push_back("\t.fpu vfp");
        res.push_back("\t.type	"+func_name+", %function");
        res.push_back(func_name+":");
        for(auto i:basic_blocks)
        {
            res.splice(res.end(),i->to_string());
        }
        res.push_back("\t.size	"+func_name+", .-"+func_name);
        return res;
    };

    //函数
    struct ic_func * function;
    //函数流图中的所有基本块序列，顺序就是中间代码的书写顺序
    list<struct arm_basic_block * > basic_blocks;
};

//arm汇编的流图
struct arm_flow_graph
{
    arm_flow_graph()
    {
        
    };

    //把一条arm汇编语句插入到汇编流图中
    //参数arm_asm时要插入的汇编语句
    //参数new_func如果不是nullptr，就表示要新建一个函数，将arm_asm插入到这个函数中
    //参数new_basic_block如果不是false，就表示要新建一个基本块，将arm_asm插入到这个基本块中
    void add_arm_asm_to_current_func(Arm_asm_file_line * arm_asm,struct ic_func * new_func,bool new_basic_block)
    {
        static struct arm_func_flow_graph * current_arm_func_flow_graph=nullptr;
        if(new_func!=nullptr)
        {
            current_arm_func_flow_graph=new struct arm_func_flow_graph(new_func);
            func_flow_graphs.insert(current_arm_func_flow_graph);
        }
        if(current_arm_func_flow_graph)
        {
            current_arm_func_flow_graph->add_arm_asm(arm_asm,new_basic_block);
        }
    };

    void add_arm_asm_to_global(Arm_asm_file_line * arm_asm)
    {
        global_basic_block.add_arm_asm(arm_asm);
    };

    ~arm_flow_graph()
    {
        for(auto i:func_flow_graphs)
        {
            delete i;
        }
    };

    //转换成字符串
    list<string> to_string()
    {
        list<string> res,tmp;
        res.push_back("\t.arch armv7");
        res.push_back("\t.eabi_attribute 28, 1");
	    res.push_back("\t.eabi_attribute 20, 1");
	    res.push_back("\t.eabi_attribute 21, 1");
	    res.push_back("\t.eabi_attribute 23, 3");
	    res.push_back("\t.eabi_attribute 24, 1");
	    res.push_back("\t.eabi_attribute 25, 1");
	    res.push_back("\t.eabi_attribute 26, 2");
	    res.push_back("\t.eabi_attribute 30, 6");
	    res.push_back("\t.eabi_attribute 34, 1");
	    res.push_back("\t.eabi_attribute 18, 4");
        //res.push_back("\t.file	\"lrc_test.sy\"");
        for(auto i:func_flow_graphs)
        {
            tmp=i->to_string();
            res.splice(res.end(),tmp);
        }
        tmp=global_basic_block.to_string();
        res.splice(res.end(),tmp);
        res.push_back("\t.ident	\"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0\"");
	    res.push_back("\t.section	.note.GNU-stack,\"\",%progbits");
        return res;
    };

    //所有函数的流图，这些流图之间相互独立
    set<struct arm_func_flow_graph * > func_flow_graphs;
    //全局定义域流图（里面的语句只能是变量定义语句）
    struct arm_basic_block global_basic_block;
};

class Arm_asm_optimizer
{
private:
    //要进行优化的arm汇编代码流图
    //struct arm_flow_graph * arm_flow_graph_;

    //局部优化
    void local_optimize(struct arm_flow_graph & arm_flow_graph);

    //优化函数的出入口代码
    void optimize_func_enter_and_exit(struct arm_func_flow_graph * func);

    //全局优化
    void global_optimize(struct arm_flow_graph & arm_flow_graph);

public:
    //进行arm汇编优化
    void optimize(struct arm_flow_graph & arm_flow_graph);
};

#endif //__ARM_ASM_OPTIMIZER_H