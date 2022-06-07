/*
 *
 *  dag.h
 *  DAG构造器
 *  Creator:Sichao Chen
 *  Create time:2022/5/4
 *
*/
#ifndef __DAG_H
#define __DAG_H

#include"ic_optimizer.h"

//DAG节点的运算类型
enum class DAG_node_op
{
    NOP,
    ASSIGN,
    FLOAT_TO_INT,
    INT_TO_FLOAT,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NOT,
    EQ,
    UEQ,
    GE,
    GT,
    LE,
    LT,
    ARRAY_MEMBER,
    ARRAY_MEMBER_ASSIGNED_BY_ASSIGN,
    ARRAY_MEMBER_ASSIGNED_BY_FLOAT_TO_INT,
    ARRAY_MEMBER_ASSIGNED_BY_INT_TO_FLOAT,
    ARRAY_MEMBER_ASSIGNED_BY_ADD,
    ARRAY_MEMBER_ASSIGNED_BY_SUB,
    ARRAY_MEMBER_ASSIGNED_BY_MUL,
    ARRAY_MEMBER_ASSIGNED_BY_DIV,
    ARRAY_MEMBER_ASSIGNED_BY_MOD,
    ARRAY_MEMBER_ASSIGNED_BY_NOT,
    ARRAY_MEMBER_ASSIGNED_BY_EQ,
    ARRAY_MEMBER_ASSIGNED_BY_UEQ,
    ARRAY_MEMBER_ASSIGNED_BY_GE,
    ARRAY_MEMBER_ASSIGNED_BY_GT,
    ARRAY_MEMBER_ASSIGNED_BY_LE,
    ARRAY_MEMBER_ASSIGNED_BY_LT,
};

//DAG的节点
struct DAG_node
{
    //根据一个数据构造一个DAG节点
    DAG_node(struct ic_data * data);

    //根据一个操作符和一个数据构造一个DAG节点
    DAG_node(enum ic_op op,struct ic_data * data,list<struct DAG_node * > children,void * special_data=nullptr);

    //向该DAG节点中添加相关的数据
    void add_data(struct ic_data * data);

    //该节点是否是叶子节点
    bool is_leaf();

    //如果该节点是叶子节点的话，就返回这个叶子节点关联的唯一数据，否则的话返回nullptr
    struct ic_data * get_leaf_node_s_only_data();

    //获取该节点的第一个关联数据
    struct ic_data * get_first_data();

    //获取该节点的唯一的子节点，如果该节点有多个子节点，返回nullptr
    struct DAG_node * get_only_child();

    //如果该节点有两个孩子节点，那么获取其左孩子节点，否则的话返回nullptr
    struct DAG_node * get_left_child();

    //如果该节点有两个孩子节点，那么获取其右孩子节点，否则的话返回nullptr
    struct DAG_node * get_right_child();

    //获取该节点关联的数据的数据类型
    enum language_data_type get_related_data_type();

    set<struct DAG_node * > fathers;                //该节点的若干个父节点
    list<struct DAG_node * > children;              //该节点的若干个儿子节点
    list<struct ic_data * > related_datas;           //该节点关联的数据
    void * special_data;                            //特殊的数据，比如说调用的函数，跳转到的标签等等
    enum ic_op related_op;                          //该节点关联的操作
};

//DAG的类
class DAG
{
private:
    //所有的DAG节点
    set<struct DAG_node * > all_nodes_;

    //存储数据到DAG节点的映射，只有数据到最后一个DAG节点的映射是目前有效的，如果最后一个DAG节点是nullptr，那么就说明这个数据目前没有节点与之映射
    //map<struct ic_data *,list<struct DAG_node * > > data_to_nodes_map_;
    map<struct ic_data *,struct DAG_node * > data_to_node_;

    //还原成DAG节点的时候的顺序
    list<struct DAG_node * > nodes_order_;

    //存储数组到数组取元素的映射
    map<struct ic_data *,set<struct ic_data * > > array_to_array_member_map_;

    //存储数组取元素的偏移量到数组取元素的映射
    map<struct ic_data *,set<struct ic_data * > > offset_to_array_member_map_;

    //基本块中的变量定义
    list<struct quaternion> var_defs_;

    //基本块中的跳转语句
    struct quaternion jmp_ic_;

    //基本块中的label定义语句
    struct quaternion label_def_;

    //基本块中的函数定义语句
    struct quaternion func_def_;

    //基本块中的函数定义结束语句
    struct quaternion func_end_;

    //将某一个数据和某一个DAG节点建立关联
    void attach_data_to_node(struct ic_data * data,struct DAG_node * node);

    //将某一个数据和它当前对应的DAG节点解除关联
    void unattach_data_s_current_node(struct ic_data * data);

    //根据给定的数据新建一个DAG节点
    struct DAG_node * new_DAG_node(struct ic_data * data);

    //根据给定的表达式建立一个DAG节点
    struct DAG_node * new_DAG_node(enum ic_op op,struct ic_data * data,size_t operands_num,...);

    //根据给定的表达式建立一个DAG节点
    struct DAG_node * new_DAG_node(enum ic_op op,struct ic_data * data,list<struct ic_data * > * operands,void * special_data=nullptr);

    //获取一个数据对应的DAG节点，如果没有的话就创建并返回
    struct DAG_node * get_DAG_node(struct ic_data * data);

    //查看某一个节点是否是某一个数据对应的当前节点
    bool check_data_s_node_available(struct ic_data * data,struct DAG_node * node);

    //查看一个叶子节点是否有效
    bool check_leaf_node_available(struct DAG_node * node);

    //生成一条赋值语句的中间代码对应的DAG信息
    void generate_ASSIGN_in_DAG(struct ic_data * to,struct ic_data * from);

    //生成一条有条件跳转语句的中间代码对应的DAG信息
    void generate_IF_JMP_or_IF_NOT_JMP_in_DAG(enum ic_op op,struct ic_data * arg,struct ic_label * label);

    //生成一条函数调用语句的中间代码对应的DAG信息
    void generate_CALL_in_DAG(struct ic_func * func,struct ic_data * ret,list<struct ic_data * > * r_params);

    //生成一条返回语句的中间代码对应的DAG信息
    void generate_RET_in_DAG(struct ic_data * ret_value);

    //找到一个数据的复制传播得最深的数据
    struct ic_data * copy_progagation(struct ic_data * data,bool stop_when_array_member=false);

    //尝试进行常量合并
    bool constant_merge(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2=nullptr);

    //尝试进行代数表达式的化简
    bool algebraic_simplify(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2=nullptr);

    //尝试进行公共子表达的删除
    bool common_expression_delete(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2=nullptr);

    //进行DAG节点的建立和节点之间关系的建立
    bool build_DAG_node_and_relation(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2=nullptr);

public:
    //构造函数
    DAG();
    DAG(struct ic_basic_block * basic_block);

    //析构函数
    ~DAG();

    //对DAG进行优化
    void optimize();

    //把一个DAG还原成一个基本块
    list<struct quaternion> to_basic_block();
};

#endif //__DAG_H