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

typedef size_t virtual_target_code_pos;

//基本块的虚拟寄存器的活跃分析结果
struct basic_block_s_live_analysis
{
    set<reg_index> live_def;
    set<reg_index> live_use;
    set<reg_index> live_in;
    set<reg_index> live_out;
};

//函数的虚拟寄存器的活跃分析结果
struct live_analysis
{
    //清空信息
    void clear();

    //每一个基本块的活跃虚拟寄存器信息
    map<struct arm_basic_block *,struct basic_block_s_live_analysis> basic_block_s_live_analysis_infos;
};

//一个虚拟寄存器的生命周期
struct virutal_reg_s_live_interval
{
    virutal_reg_s_live_interval();

    //新增
    void start(virtual_target_code_pos start_pos,virtual_target_code_pos end_pos);

    //截断
    void cut_off(virtual_target_code_pos pos);

    //延长
    void extend(virtual_target_code_pos pos);

    //增加该虚拟寄存器的使用点
    void add_use_pos(struct arm_basic_block * bb,virtual_target_code_pos pos);

    //增加该虚拟寄存器的定义点
    void add_def_pos(struct arm_basic_block * bb,virtual_target_code_pos pos);

    //该生命周期是否正在延长
    bool is_extending;

    //该虚拟寄存器的所有使用点所在的基本块
    map<struct arm_basic_block * ,set<virtual_target_code_pos> > use_poses;

    //该虚拟寄存器的所有定义点所在的基本块
    map<struct arm_basic_block * ,set<virtual_target_code_pos> > def_poses;

    //生命周期，每一个pair的first是起点，second是终点
    list<pair<virtual_target_code_pos,virtual_target_code_pos> > live_interval;
};

//一个函数中的所有虚拟寄存器的生命周期
struct live_intervals
{
    //清空信息
    void clear();

    //将整个虚拟目标代码新增一段空白
    void new_empty_virtual_code_segment(virtual_target_code_pos pos,size_t add_size);

    //获取某一个虚拟寄存器的生命周期
    inline struct virutal_reg_s_live_interval & get_reg_s_live_interval(reg_index reg)
    {
        if(virtual_regs_s_live_intervals.find(reg)==virtual_regs_s_live_intervals.end())
        {
            virtual_regs_s_live_intervals.insert(make_pair(reg,virutal_reg_s_live_interval()));
        }
        return virtual_regs_s_live_intervals.at(reg);
    };

    //该函数中所有虚拟寄存器的生命周期
    map<reg_index,struct virutal_reg_s_live_interval> virtual_regs_s_live_intervals;
};

//虚拟寄存器相干图中的一个点
struct coherent_diagram_node
{
    coherent_diagram_node(reg_index reg);

    //增加一个点重合（移动关联）的邻居
    void add_a_move_related_neighbour(struct coherent_diagram_node * node);

    //增加一个线重合（冲突）的邻居
    void add_a_collision_neighbour(struct coherent_diagram_node * node);
    
    //该点对应的虚拟寄存器
    reg_index reg;

    //度数
    size_t degree;

    //和该点点重合的所有点
    set<struct coherent_diagram_node * > move_related_nodes;

    //和该点线重合的所有点
    set<struct coherent_diagram_node * > collision_nodes;
};

//虚拟寄存器的相干图
struct coherent_diagram
{   
    ~coherent_diagram();

    //清空信息
    void clear();

    //新建一个相干图中的节点
    struct coherent_diagram_node * new_node(reg_index reg);

    //获取一个寄存器对应的相干图中的点
    struct coherent_diagram_node * get_coherent_diagram_node(reg_index reg);

    //给两个寄存器增加点重合（移动关联）（虚线）
    void add_move_related(reg_index reg_1,reg_index reg_2);

    //给两个寄存器增加线重合（冲突）（实线）
    void add_collision(reg_index reg_1,reg_index reg_2);

    //相干图中的所有点
    map<reg_index,struct coherent_diagram_node * > nodes;
};

class Graph_coloring_register_manager:public Global_register_manager
{
private:
    //正在进行图着色的虚拟目标代码
    struct arm_func_flow_graph * virtual_target_code;

    //虚拟目标代码中基本块的区间
    map<struct arm_basic_block *,pair<virtual_target_code_pos,virtual_target_code_pos> > basic_block_s_interval;

    //目前正在处理哪一个处理器上的寄存器
    enum reg_processor current_processor;

    //本函数中可以用于着色的CPU物理寄存器个数
    size_t available_physical_cpu_reg_num;

    //本函数中可以用于着色的CPU物理寄存器
    set<reg_index> available_physical_cpu_regs;

    //本函数中可以用于着色的VFP物理寄存器个数
    size_t available_physical_vfp_reg_num;

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
    void graph_coloring_register_distribute();

    //根据着色的结果从虚拟目标代码生成实际目标代码
    void virtual_target_code_to_physical_target_code();

    //把虚拟寄存器分配的信息清空
    void clear_info();
    
protected:
    void handle_END_FUNC();

public:
    Graph_coloring_register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    ~Graph_coloring_register_manager();
};

#endif //__GRAPH_COLORING_REGISTER_MANAGER_H