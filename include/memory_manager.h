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
#include "arm_asm.h"

//栈的位置
typedef size_t stack_pos;

//函数运行时某一个时刻的栈空间
struct runtime_stack_space
{
    list<pair<stack_pos,struct ic_data * > > f_params_passed_by_stack_as_callee;
    list<pair<stack_pos,reg_index> > context_saved_cpu_regs_as_callee;
    list<pair<stack_pos,reg_index> > context_saved_vfp_regs_as_callee;
    size_t padding_bytes_after_context_saved_regs;
    list<pair<stack_pos,struct ic_data * > > f_params_passed_by_cpu_regs_as_callee;
    list<pair<stack_pos,struct ic_data * > > f_params_passed_by_vfp_regs_as_callee;
    list<pair<stack_pos,struct ic_data * > > local_vars;
    size_t padding_bytes_after_local_vars;
    list<pair<stack_pos,struct ic_data * > > temp_vars;
    size_t padding_bytes_after_temp_vars;
    size_t padding_bytes_before_r_params_passed_by_stack_as_caller;
    list<pair<stack_pos,struct ic_data * > > r_params_passed_by_stack_as_caller;
    stack_pos stack_pointer,frame_pointer;

    runtime_stack_space():stack_pointer(0),frame_pointer(0),padding_bytes_after_context_saved_regs(0),padding_bytes_after_local_vars(0),padding_bytes_after_temp_vars(0),padding_bytes_before_r_params_passed_by_stack_as_caller(0)
    {

    };

    void clear()
    {
        stack_pointer=0;
        frame_pointer=0;
        padding_bytes_after_context_saved_regs=0;
        padding_bytes_after_local_vars=0;
        padding_bytes_after_temp_vars=0;
        padding_bytes_before_r_params_passed_by_stack_as_caller=0;
        f_params_passed_by_stack_as_callee.clear();
        context_saved_cpu_regs_as_callee.clear();
        context_saved_vfp_regs_as_callee.clear();
        f_params_passed_by_cpu_regs_as_callee.clear();
        f_params_passed_by_vfp_regs_as_callee.clear();
        local_vars.clear();
        temp_vars.clear();
        r_params_passed_by_stack_as_caller.clear();
    };

    void set_padding_bytes_after_context_saved_regs(size_t padding_bytes)
    {
        padding_bytes_after_context_saved_regs=padding_bytes;
        stack_pointer+=padding_bytes;
    };

    void set_padding_bytes_after_temp_vars(size_t padding_bytes)
    {
        padding_bytes_after_temp_vars=padding_bytes;
        stack_pointer+=padding_bytes;
    };

    void set_padding_bytes_after_local_vars(size_t padding_bytes)
    {
        padding_bytes_after_local_vars=padding_bytes;
        stack_pointer+=padding_bytes;
    };

    void set_padding_bytes_before_r_params_passed_by_stack_as_caller(size_t padding_bytes)
    {
        padding_bytes_before_r_params_passed_by_stack_as_caller=padding_bytes;
        stack_pointer+=padding_bytes;
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

    void push_to_r_params_passed_by_stack_as_caller(struct ic_data * var)
    {
        r_params_passed_by_stack_as_caller.push_back(make_pair(stack_pointer,var));
        stack_pointer+=var->get_byte_size(true);
    };
};

//内存管理器
class Memory_manager:public Asm_generator_component
{

private:
    //有效位
    bool is_init_successful_;

    //当前函数的栈空间
    struct runtime_stack_space current_func_stack_space_;

    //当前函数中每一次栈空间改变的位置和改变的情况
    list<pair<list<Arm_asm_file_line * >::iterator,struct runtime_stack_space > > stack_space_changed_poses;

    //事件处理函数
    void handle_FUNC_DEFINE(struct ic_func * func);
    struct event handle_READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK(struct ic_func * func);
    struct event handle_READY_TO_PUSH_TEMP_VARS(set<struct ic_data * > * temp_vars);
    struct event handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(struct ic_func * func);
    struct event handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(struct ic_func * func);
    struct event handle_GET_CURRENT_FUNC_STACK_SIZE();
    struct event handle_READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS();
    struct event handle_CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC();
    struct event handle_GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC();
    struct event handle_READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS();
    struct event handle_READY_TO_POP_WHEN_RET();
    struct event handle_GET_VAR_STACK_POS_FROM_SP(struct ic_data * var);
    struct event handle_GET_VAR_STACK_POS_FROM_FP(struct ic_data * var);
    void handle_PUSH_VAR_TO_STACK(struct ic_data * var);
    void handle_PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC(struct ic_data * argument);
    void handle_PADDING_WHEN_CALL_FUNC(int padding_bytes);
    struct event handle_RET_FROM_CALLED_FUNC();
    struct event handle_RET_FROM_CALLED_ABI_FUNC();
    struct event handle_CHECK_TEMP_VAR_IN_STACK(struct ic_data * var);
    void handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block);
    void handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block);
    struct event handle_IS_F_PARAM_PASSED_BY_STACK(struct ic_data * var);
    void handle_RECORD_STACK_SPACE_CHANGED_HERE(list<Arm_asm_file_line * >::iterator * current_instruction_pos);
    void handle_SET_STACK_SPACE(list<Arm_asm_file_line * >::iterator * current_instruction_pos);
    void handle_END_FUNC();

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