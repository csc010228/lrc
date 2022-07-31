/*
 *
 *  asm_generator.cpp
 *  汇编代码生成
 *  Creator:Sichao Chen
 *  Create time:2021/11/23
 *
*/
#include "asm_generator.h"
#include<iostream>
#include<fstream>

using namespace std;

/*
汇编代码生成器的构造函数
*/
Asm_generator::Asm_generator()
{

}

/*
初始化寄存器管理器

Parameters
----------
regs:目标机器的寄存器信息
flag_reg:标志寄存器
optimize:是否需要优化
*/
bool Asm_generator::create_register_manager(set<struct reg> regs,struct flag_reg flag_reg,bool optimize)
{
    easyer_register_manager_=new Local_register_manager(regs,flag_reg);
    easyer_register_manager_->set_mediator(this);
    better_register_manager_=new Graph_coloring_register_manager(regs,flag_reg);
    better_register_manager_->set_mediator(this);
    // if(optimize)
    // {
    //     register_manager_=new Graph_coloring_register_manager(regs,flag_reg);
    // }
    // else
    // {
    //     register_manager_=new Local_register_manager(regs,flag_reg);
    // }
    if(optimize)
    {
        register_manager_=better_register_manager_;
    }
    else
    {
        register_manager_=easyer_register_manager_;
    }
    // register_manager_->set_mediator(this);
    // return register_manager_->is_init_successful();
    return better_register_manager_->is_init_successful() && easyer_register_manager_->is_init_successful();
}

/*
初始化内存管理器

Parameters
----------
memory_info:内存信息
*/
bool Asm_generator::create_memory_manager(string memory_info)
{
    memory_manager_=new Memory_manager(memory_info);
    memory_manager_->set_mediator(this);
    return memory_manager_->is_init_successful();
}

/*
初始化中间代码管理器
*/
bool Asm_generator::create_intermediate_code_manager()
{
    intermediate_code_manager_=new Intermediate_code_manager;
    intermediate_code_manager_->set_mediator(this);
    return true;
}

/*
汇编代码生成器的初始化

Parameters
----------
regs:目标机器下的可用的寄存器以及其类型
memory_info:目标机器的内存信息
optimize:是否需要优化
*/
bool Asm_generator::init(set<struct reg> regs,struct flag_reg flag_reg,string memory_info,bool optimize)
{
    map<reg_index,string> regs_info;
    for(auto i:regs)
    {
        regs_info.insert(make_pair(i.index,i.name));
    }
    return create_register_manager(regs,flag_reg,optimize) && create_memory_manager(memory_info) && create_instruction_generator(regs_info) && create_intermediate_code_manager() && create_asm_optimizer(optimize) && create_abi_manager() && init_components();
}

/*
汇编代码生成器的析构函数
*/
Asm_generator::~Asm_generator()
{
    // if(register_manager_)
    // {
    //     delete register_manager_;
    // }
    if(easyer_register_manager_)
    {
        delete easyer_register_manager_;
    }

    if(better_register_manager_)
    {
        delete better_register_manager_;
    }

    if(memory_manager_)
    {
        delete memory_manager_;
    }

    if(instruction_generator_)
    {
        delete instruction_generator_;
    }

    if(intermediate_code_manager_)
    {
        delete intermediate_code_manager_;
    }
}

void Asm_generator::handle_CHANGE_TO_EASYER_REGISTER_MANAGER()
{
    register_manager_=easyer_register_manager_;
}

void Asm_generator::handle_CHANGE_TO_BETTER_REGISTER_MANAGER()
{
    register_manager_=better_register_manager_;
}

struct event Asm_generator::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::CHANGE_TO_EASYER_REGISTER_MANAGER:
            handle_CHANGE_TO_EASYER_REGISTER_MANAGER();
            break;
        case event_type::CHANGE_TO_BETTER_REGISTER_MANAGER:
            handle_CHANGE_TO_BETTER_REGISTER_MANAGER();
            break;
        default:
            break;
    }
    return res;
}

/*
生成最终的汇编代码文件

Parameters
----------
intermediate_codes_flow_graph:中间代码流图
filename:要输出的汇编文件名
*/
bool Asm_generator::generate_asm_then_output(struct ic_flow_graph * intermediate_codes_flow_graph,const char * filename)
{
    list<string> asm_codes;
    ofstream outFile;
	outFile.open(filename,ios::out);
	if (!outFile)
    {
        cout<<"Open file "<<filename<<" error!"<<endl;
        return false;
	}

    intermediate_code_manager_->init(intermediate_codes_flow_graph);
    asm_codes=instruction_generator_->generate_asm();

    //把所有的最终代码输出到文件
    for(auto i : asm_codes)
    {
        outFile<<(i)<<endl;
    }

    outFile.close();

    return true;
}