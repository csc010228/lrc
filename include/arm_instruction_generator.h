/*
 *
 *  arm_instruction_generator.h
 *  arm汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#ifndef __ARM_INSTRUCTION_GENERATOR_H
#define __ARM_INSTRUCTION_GENERATOR_H

#include<stdarg.h>
#include "instruction_generator.h"
#include "arm_asm.h"

//arm汇编代码生成器
class Arm_instruction_generator:public Instruction_generator
{

protected:
    //arm汇编的流图
    struct arm_flow_graph arm_flow_graph_;

    //当前基本块是否处于开始状态
    bool is_current_basic_block_starting_;

    //为了溢出而生成的虚拟目标代码
    list<Arm_asm_file_line * > * virtual_target_codes_for_spilling_;

    //生成汇编文件序列,并将其赋值给父类Instruction_generator的asm_codes_成员
    void generate_asm_codes();

    //生成arm汇编文件的流图形式，并将其赋值给arm_flow_graph_成员
    void generate_arm_flow_graph();

    //根据一条中间代码生成最终的arm汇编代码
    void ic_to_arm_asm(struct quaternion intermediate_code);
    //获得一个operand2
    struct operand2 get_operand2(struct ic_data * ic_data,enum operand2_shift_op shift_op=operand2_shift_op::NONE,unsigned int n=0);
    struct operand2 get_operand2(int const_int_value,bool get_an_empty_reg=false);
    //获取一个flexoffset
    struct flexoffset get_flexoffset(int offset);
    struct flexoffset get_flexoffset(struct ic_data * ic_data,enum flexoffset_shift_op shift_op=flexoffset_shift_op::NONE,unsigned int shift_num=0,bool is_negative=false);
    //根据一条ASSIGN中间代码生成最终的arm汇编代码
    void assign_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result);
    //根据一条ADD中间代码生成最终的arm汇编代码
    void add_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条SUB中间代码生成最终的arm汇编代码
    void sub_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条MUL中间代码生成最终的arm汇编代码
    void mul_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条DIV中间代码生成最终的arm汇编代码
    void div_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条MOD中间代码生成最终的arm汇编代码
    void mod_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    // //根据一条L_SHIFT中间代码生成最终的arm汇编代码
    // void l_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    // //根据一条R_SHIFT中间代码生成最终的arm汇编代码
    // void r_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条NOT中间代码生成最终的arm汇编代码
    void not_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result);
    // //根据一条AND中间代码生成最终的arm汇编代码
    // void and_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    // //根据一条OR中间代码生成最终的arm汇编代码
    // void or_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    // //根据一条XOR中间代码生成最终的arm汇编代码
    // void xor_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条EQ中间代码生成最终的arm汇编代码
    void eq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条UEQ中间代码生成最终的arm汇编代码
    void ueq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条GT中间代码生成最终的arm汇编代码
    void gt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条LT中间代码生成最终的arm汇编代码
    void lt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条GE中间代码生成最终的arm汇编代码
    void ge_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条LE中间代码生成最终的arm汇编代码
    void le_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条JMP中间代码生成最终的arm汇编代码
    void jmp_ic_to_arm_asm(struct ic_label * result);
    //根据一条IF_JMP中间代码生成最终的arm汇编代码
    void if_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result);
    //根据一条IF_NOT_JMP中间代码生成最终的arm汇编代码
    void if_not_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result);
    //根据一条VAR_DEFINE中间代码生成最终的arm汇编代码
    void var_define_ic_to_arm_asm(struct ic_data * result);
    //根据一条LABEL_DEFINE中间代码生成最终的arm汇编代码
    void label_define_ic_to_arm_asm(struct ic_label * result);
    //根据一条FUNC_DEFINE中间代码生成最终的arm汇编代码
    void func_define_ic_to_arm_asm(struct ic_func * result);
    //根据一条END_FUNC_DEFINE中间代码生成最终的arm汇编代码
    void end_func_define_ic_to_arm_asm();
    //根据一条CALL中间代码生成最终的arm汇编代码
    void call_ic_to_arm_asm(struct ic_func * arg1,list<struct ic_data * > * arg2,struct ic_data * result);
    //根据一条RET中间代码生成最终的arm汇编代码
    void ret_ic_to_arm_asm(struct ic_data * result);

    //把当前一个基本块开始的部分转换成arm汇编代码
    void start_basic_block_to_arm_asm();

    //把一行instruction插入到当前汇编文件的最后
    inline void push_instruction(Arm_instruction * instruction);
    //把一行pseudo instruction插入到当前汇编文件的最后
    inline void push_pseudo_instruction(Arm_pseudo_instruction * pseudo_instruction);
    //把一行label插入到当前汇编函数的最后
    inline void push_label_to_func(Arm_label * label);
    //把一行label插入到当前汇编文件的全局区域的最后
    inline void push_label_to_global(Arm_label * label);
    //把一行directive插入到汇编的全局区域当中
    inline void push_directive(Arm_directive * directive);

    //在当前指令的位置保存运行时栈空间情况
    void record_stack_space_changed_here();

    //事件处理函数
    void handle_WRITE_CONST_TO_REG(OAA const_data,reg_index reg);
    void handle_STORE_VAR_TO_MEM(struct ic_data * var,reg_index reg);
    void handle_STORE_TEMP_VAR_TO_MEM(struct ic_data * var,reg_index reg);
    void handle_LOAD_VAR_TO_REG(struct ic_data * var,reg_index reg);
    void handle_WRITE_ADDR_TO_REG(struct ic_data * var,reg_index reg);
    void handle_PUSH_TEMP_VAR_FROM_REG_TO_STACK(struct ic_data * var,reg_index reg);
    void handle_CALL_FUNC(struct ic_func * func,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    void handle_CALL_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg,list<reg_index> * r_param_regs);
    void handle_MOVE_DATA_BETWEEN_REGS(reg_index from,reg_index to);
    void handle_ASSIGN_VAR(struct ic_data * from,struct ic_data * to);
    void handle_POP_STACK(size_t pop_size);
    void handle_START_FUNC(struct ic_func_flow_graph * func);
    void handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block);
    struct event handle_GET_VIRTUAL_TRAGET_CODE_OF_CURRENT_FUNC();
    void handle_START_GENERATING_SPILLING_VIRTUAL_CODES();
    struct event handle_GET_SPILLING_VIRTUAL_CODES();

public:
    //构造函数
    Arm_instruction_generator(map<reg_index,string> regs_info);

    //析构函数
    ~Arm_instruction_generator();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__ARM_INSTRUCTION_GENERATOR_H