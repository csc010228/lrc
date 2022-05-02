/*
 *
 *  memory_manager.h
 *  内存管理器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#ifndef __MEMORY_MANAGER_H
#define __MEMORY_MANAGER_H

#include<string>
#include<utility>
#include<map>
#include "symbol_table.h"
#include "asm_generator_component.h"

//栈的位置
typedef size_t stack_pos;

//内存管理器
class Memory_manager:public Asm_generator_component
{

private:
    //有效位
    bool is_init_successful_;

    //当前函数的栈空间
    struct func_stack_space
    {
        list<pair<stack_pos,struct ic_data * > > f_params_passed_by_stack_as_callee;
        list<pair<stack_pos,reg_index> > context_saved_cpu_regs_as_callee;
        list<pair<stack_pos,reg_index> > context_saved_vfp_regs_as_callee;
        list<pair<stack_pos,struct ic_data * > > f_params_passed_by_cpu_regs_as_callee;
        list<pair<stack_pos,struct ic_data * > > f_params_passed_by_vfp_regs_as_callee;
        list<pair<stack_pos,struct ic_data * > > local_vars;
        list<pair<stack_pos,struct ic_data * > > temp_vars;
        list<pair<stack_pos,struct ic_data * > > f_params_passed_by_stack_as_caller;
        stack_pos stack_pointer,frame_pointer;

        void clear()
        {
            stack_pointer=0;
            frame_pointer=0;
            f_params_passed_by_cpu_regs_as_callee.clear();
            context_saved_cpu_regs_as_callee.clear();
            context_saved_vfp_regs_as_callee.clear();
            f_params_passed_by_cpu_regs_as_callee.clear();
            f_params_passed_by_vfp_regs_as_callee.clear();
            local_vars.clear();
            temp_vars.clear();
            f_params_passed_by_stack_as_caller.clear();
        };

        void push_to_f_params_passed_by_stack_as_callee(struct ic_data * var)
        {
            f_params_passed_by_stack_as_callee.push_back(make_pair(stack_pointer,var));
            frame_pointer+=var->get_byte_size();
            stack_pointer+=var->get_byte_size();
        };

        void push_to_context_saved_cpu_regs(reg_index reg,size_t reg_byte_size)
        {
            context_saved_cpu_regs_as_callee.push_back(make_pair(stack_pointer,reg));
            stack_pointer+=reg_byte_size;
        };

        void push_to_context_saved_vfp_regs(reg_index reg,size_t reg_byte_size)
        {
            context_saved_vfp_regs_as_callee.push_back(make_pair(stack_pointer,reg));
            stack_pointer+=reg_byte_size;
        };

        void push_to_f_params_passed_by_cpu_regs_as_callee(struct ic_data * var)
        {
            f_params_passed_by_cpu_regs_as_callee.push_back(make_pair(stack_pointer,var));
            stack_pointer+=var->get_byte_size();
        };

        void push_to_f_params_passed_by_vfp_regs_as_callee(struct ic_data * var)
        {
            f_params_passed_by_vfp_regs_as_callee.push_back(make_pair(stack_pointer,var));
            stack_pointer+=var->get_byte_size();
        };

        void push_to_local_vars(struct ic_data * var)
        {
            local_vars.push_back(make_pair(stack_pointer,var));
            stack_pointer+=var->get_byte_size();
        };

        void push_to_temp_vars(struct ic_data * var)
        {
            temp_vars.push_back(make_pair(stack_pointer,var));
            stack_pointer+=var->get_byte_size();
        };

        void push_to_f_params_passed_by_stack_as_caller(struct ic_data * var)
        {
            f_params_passed_by_stack_as_caller.push_back(make_pair(stack_pointer,var));
            stack_pointer+=var->get_byte_size(true);
        };
    } current_func_stack_space_;

    //事件处理函数
    void handle_FUNC_DEFINE(struct ic_func * func);
    struct event handle_READY_TO_PUSH_LOCAL_VARS(struct ic_func * func);
    void handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(list<reg_index> * regs);
    void handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(list<reg_index> * regs);
    struct event handle_GET_CURRENT_FUNC_STACK_SIZE();
    void handle_READY_TO_PUSH_F_PARAM_CPU_REGS(list<struct ic_data * > * f_params);
    void handle_READY_TO_PUSH_F_PARAM_VFP_REGS(list<struct ic_data * > * f_params);
    struct event handle_READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS();
    struct event handle_READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS();
    struct event handle_READY_TO_POP_TEMP_VARS();
    struct event handle_READY_TO_POP_LOCAL_VARS();
    struct event handle_READY_TO_POP_F_PARAM_CPU_REGS();
    struct event handle_READY_TO_POP_F_PARAM_VFP_REGS();
    struct event handle_GET_VAR_STACK_POS_FROM_SP(struct ic_data * var);
    struct event handle_GET_VAR_STACK_POS_FROM_FP(struct ic_data * var);
    void handle_PUSH_VAR_TO_STACK(struct ic_data * var);
    void handle_PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC(struct ic_data * argument);
    struct event handle_RET_FROM_CALLED_FUNC();
    struct event handle_CHECK_TEMP_VAR_IN_STACK(struct ic_data * var);
    void handle_END_BASIC_BLOCK();
    void handle_END_BASIC_BLOCK_WITHOUT_FLAG();
    struct event handle_IS_F_PARAM_PASSED_BY_STACK(struct ic_data * var);

public:
    //构造函数
    Memory_manager(string memory_info);

    //析构函数
    virtual ~Memory_manager();

    //检查是否构造成功
    bool is_init_successful();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__MEMORY_MANAGER_H