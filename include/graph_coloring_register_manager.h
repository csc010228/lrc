/*
 *
 *  graph_coloring_register_manager.h
 *  全局寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/6
 *
*/
#ifndef __GRAPH_COLORING_REGISTER_MANAGER_H
#define __GRAPH_COLORING_REGISTER_MANAGER_H

#include"global_register_manager.h"
#include"virtual_target_code.h"

class Graph_coloring_register_manager:public Global_register_manager
{
private:
    //正在进行图着色的虚拟目标代码
    struct arm_func_flow_graph * virtual_target_code;

    //虚拟目标代码中基本块的区间
    map<struct arm_basic_block *,pair<virtual_target_code_pos,virtual_target_code_pos> > basic_block_s_interval;

    //目前正在处理哪一个处理器上的寄存器
    enum reg_processor current_processor;

    //本函数中可以用于着色的CPU物理寄存器
    set<reg_index> available_physical_cpu_regs;

    //本函数中可以用于着色的VFP物理寄存器
    set<reg_index> available_physical_vfp_regs;

    //当前函数中的寄存器的活跃分析结果
    struct live_analysis current_func_s_live_analysis;

    //当前函数中的寄存器的live intervals
    struct live_intervals current_func_s_live_intervals;

    //当前函数中的寄存器的相干图
    struct coherent_diagram current_func_s_coherent_diagram;

    set<struct coherent_diagram_node * > precolored;

    set<struct coherent_diagram_node * > initial;

    set<struct coherent_diagram_node * > simplify_worklist;

    set<struct coherent_diagram_node * > freeze_worklist;

    set<struct coherent_diagram_node * > spill_worklist;

    set<struct coherent_diagram_node * > spilled_nodes;

    set<struct coherent_diagram_node * > coalesced_nodes;

    map<struct coherent_diagram_node * ,reg_index > colored_nodes;

    stack<struct coherent_diagram_node * > select_stack;

    set<struct coherent_diagram_node * > worklist_moves;

    //减少常量寄存器
    void reduce_const_regs();
    //分裂虚拟寄存器
    void fission_regs();
    //窥孔优化
    void peephole_optimization();
    //删除无用寄存器
    void remove_useless_regs();
    //优化函数和基本块的出入口代码
    void optimize_func_and_basic_block_s_enter_and_exit();
    //局部优化
    void local_optimize();
    //全局优化
    void global_optimize();

    void optimize_for_less_spill_regs();
    void optimize_before_rewrite_program();
    void optimize_virtual_code();

    //获取某一个位置的虚拟目标代码
    list<Arm_asm_file_line *>::iterator get_virtual_target_code(virtual_target_code_pos pos) const;

    //获取某一个位置所在的基本块
    struct arm_basic_block * get_pos_s_basic_block(virtual_target_code_pos pos) const;

    //获取某一个基本块的起始位置
    virtual_target_code_pos get_basic_block_s_start_pos(struct arm_basic_block * bb) const;

    //获取某一个基本块的终止位置
    virtual_target_code_pos get_basic_block_s_end_pos(struct arm_basic_block * bb) const;

    //获取一条虚拟目标代码中的所有源寄存器
    set<reg_index> get_virtual_traget_instruction_s_all_source_regs(Arm_asm_file_line * line);

    //获取一条虚拟目标代码中的所有目的寄存器
    set<reg_index> get_virtual_traget_instruction_s_all_destination_regs(Arm_asm_file_line * line);

    //获取一条虚拟目标代码中的所有的寄存器
    set<reg_index> get_virtual_traget_instruction_s_all_regs(Arm_asm_file_line * line);

    //虚拟寄存器的活跃分析
    void live_analyze();

    //构建live intervals
    void build_live_intervals();

    //构建相干图
    void build_coherent_diagram();

    //根据相干图初始化各个worklist
    void mk_worklists();

    //减小一个节点的度数
    void decrement_degree(struct coherent_diagram_node * node);

    //简化
    void simplify();

    //合并
    void coalesce();

    //冻结
    void freeze();

    //选择溢出
    void select_spill();

    //着色
    void assign_colors();

    //当要溢出某一个变量的值所在的寄存器之前，重新设置寄存器的信息
    void set_registers_when_spilling_var_value_s_reg(struct arm_basic_block * bb,list<Arm_asm_file_line * >::iterator ins_pos,struct ic_data * var);

    //生成一段将变量值load到寄存器中的虚拟目标代码
    list<Arm_asm_file_line * > generate_instructions_for_loading_var_value(struct ic_data * var,reg_index reg);

    //生成一段将变量值store到寄存器中的虚拟目标代码
    list<Arm_asm_file_line * > generate_instructions_for_storing_var_value(struct ic_data * var,reg_index reg);

    //生成一段将变量地址写入到寄存器中的虚拟目标代码
    list<Arm_asm_file_line * > generate_instructions_for_writing_var_addr_to_new_reg(struct ic_data * var,reg_index reg);

    //生成一段将常量写入到寄存器中的虚拟目标代码
    list<Arm_asm_file_line * > generate_instructions_for_writing_const_value_to_new_reg(OAA const_value,reg_index reg);

    //重写程序
    void rewrite_program();

    //对虚拟目标代码进行图着色寄存器分配
    void graph_coloring_register_distribute(bool first_time);

    //根据着色的结果从虚拟目标代码生成实际目标代码
    void virtual_target_code_to_physical_target_code();

    //把虚拟寄存器分配的信息清空
    void clear_info();

    //输出虚拟目标代码到文件
    bool output_virtual_target_code_to_file(const char * filename);
    
protected:
    void handle_END_FUNC();

public:
    Graph_coloring_register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    ~Graph_coloring_register_manager();
};

#endif //__GRAPH_COLORING_REGISTER_MANAGER_H