/*
 *
 *  virtual_target_code.h
 *  虚拟目标代码
 *  Creator:Sichao Chen
 *  Create time:2022/8/17
 *
*/
#ifndef __VIRTUAL_TARGET_CODE_H
#define __VIRTUAL_TARGET_CODE_H

#include"register_manager.h"
#include"arm_asm.h"

typedef size_t virtual_target_code_pos;

//虚拟寄存器存储的数据种类
enum class virtual_related_data_type
{
    CONST,                  //常量
    VAR_VALUE,              //变量值
    VAR_ADDR                //变量地址
};

//虚拟寄存器
struct virtual_reg
{
    virtual_reg(enum reg_processor processor):is_effective(false),is_var_value_drity(false),processor(processor),data_type(virtual_related_data_type::CONST)
    {

    };

    //设置改虚拟寄存器相关的数据
    void set_related_data(OAA const_value);

    //设置改虚拟寄存器相关的数据
    void set_related_data(struct ic_data * var,enum virtual_related_data_type value_ro_addr);

    union
    {
        struct ic_data * related_var;              //和该虚拟寄存器相关的变量
        OAA related_const;                          //和该虚拟寄存器相关的常量
    };
    bool is_effective;                                          //该虚拟寄存器此时是否有效
    bool is_var_value_drity;                                    //该虚拟寄存器中存放的变量的值是否是脏值
    enum reg_processor processor;                               //该虚拟寄存器所属的处理器
    enum virtual_related_data_type data_type;                   //该虚拟寄存器相关的变量的状态
};

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

    //某一个点是否是该虚拟寄存器的定义点
    bool is_def_pos(virtual_target_code_pos pos) const;

    //某一个点是否是该虚拟寄存器的使用点
    bool is_use_pos(virtual_target_code_pos pos) const;

    //获取寄存器溢出时的分数，分数越高，越不能溢出
    size_t get_score();

    //该生命周期是否正在延长
    bool is_extending;

    //该虚拟寄存器的所有使用点所在的基本块
    map<struct arm_basic_block * ,set<virtual_target_code_pos> > use_poses;

    //该虚拟寄存器的所有定义点所在的基本块
    map<struct arm_basic_block * ,set<virtual_target_code_pos> > def_poses;

    //生命周期，每一个pair的first是起点，second是终点
    list<pair<virtual_target_code_pos,virtual_target_code_pos> > live_interval;

    //分数
    size_t score;
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

enum class coherent_diagram_node_s_data_type
{
    CONST,
    ADDR,
    ARRAY_MEMBER_VALUE,
    VALUE,
};

//虚拟寄存器相干图中的一个点
struct coherent_diagram_node
{
    coherent_diagram_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type,struct virutal_reg_s_live_interval live_interval);

    //增加一个点重合（移动关联）的邻居
    void add_a_move_related_neighbour(struct coherent_diagram_node * node);

    //增加一个线重合（冲突）的邻居
    void add_a_collision_neighbour(struct coherent_diagram_node * node);
    
    //获取分数
    size_t get_score();

    //该点对应的虚拟寄存器
    reg_index reg;

    //度数
    size_t degree;

    //对应的类型
    enum coherent_diagram_node_s_data_type data_type;

    //和该点点重合的所有点
    set<struct coherent_diagram_node * > move_related_nodes;

    //和该点线重合的所有点
    set<struct coherent_diagram_node * > collision_nodes;

    //该点对应的虚拟寄存器的live interval
    struct virutal_reg_s_live_interval live_interval;
};

//虚拟寄存器的相干图
struct coherent_diagram
{
    coherent_diagram()
    {

    };

    coherent_diagram(struct live_intervals current_func_s_live_intervals):current_func_s_live_intervals(current_func_s_live_intervals)
    {

    };

    ~coherent_diagram();

    //清空信息
    void clear();

    //新建一个相干图中的节点
    struct coherent_diagram_node * new_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type);

    //获取一个寄存器对应的相干图中的点
    struct coherent_diagram_node * get_coherent_diagram_node(reg_index reg);

    //给两个寄存器增加点重合（移动关联）（虚线）
    void add_move_related(reg_index reg_1,reg_index reg_2);

    //给两个寄存器增加线重合（冲突）（实线）
    void add_collision(reg_index reg_1,reg_index reg_2);

    //相干图中的所有点
    map<reg_index,struct coherent_diagram_node * > nodes;
    //当前函数中的寄存器的live intervals
    struct live_intervals current_func_s_live_intervals;
};

#endif //__VIRTUAL_TARGET_CODE_H