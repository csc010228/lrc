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

//DAG的节点
struct DAG_node
{
    //根据一个数据构造一个DAG节点
    DAG_node(struct ic_data * data);

    //根据一个操作符和一个数据构造一个DAG节点
    DAG_node(enum ic_op op,struct ic_data * data,list<struct DAG_node * > children,void * special_data=nullptr);

    //获取该节点的唯一的子节点，如果该节点有多个子节点，返回nullptr
    struct DAG_node * get_only_child();

    //如果该节点有两个孩子节点，那么获取其左孩子节点，否则的话返回nullptr
    struct DAG_node * get_left_child();

    //如果该节点有两个孩子节点，那么获取其右孩子节点，否则的话返回nullptr
    struct DAG_node * get_right_child();

    //给该节点添加一个父亲节点
    void add_a_father(struct DAG_node * father);

    //给该节点添加一个孩子节点
    void add_a_child(struct DAG_node * child);

    //删除该节点的某一个父亲节点
    void delete_a_father(struct DAG_node * father);

    //获取父亲节点的数量
    size_t get_fathers_num();

    //如果该节点有两个孩子节点，那么更改其左节点，否则的话不做修改
    void change_left_child(struct DAG_node * node);

    //如果该节点有两个孩子节点，那么更改其右节点，否则的话不做修改
    void change_right_child(struct DAG_node * node);

    //移除该节点的所有子节点
    void remove_all_children();

    //判断该DAG节点是否只和一个常数关联
    bool is_related_to_a_const(enum language_data_type data_type=language_data_type::VOID);

    //判断该DAG节点是否只和一个临时变量关联
    bool is_related_to_a_temp_var();

    map<struct DAG_node * ,size_t> fathers;             //该节点的若干个父节点
    list<struct DAG_node * > children;                  //该节点的若干个儿子节点
    //list<struct ic_data * > related_datas;              //该节点关联的数据
    struct ic_data * related_data;                      //该节点关联的数据
    void * special_data;                                //特殊的数据，比如说调用的函数，跳转到的标签等等
    enum ic_op related_op;                              //该节点关联的操作
};

//DAG的类
class DAG
{
private:
    //该DAG对应的基本块
    struct ic_basic_block * basic_block_;

    //基本块中的数组和数组元素的map
    map<struct ic_data * ,set<struct ic_data * > > array_to_array_member_map_;

    //基本块中的偏移量和数组元素的map
    map<struct ic_data * ,set<struct ic_data * > > offset_to_array_member_map_;

    //所有的DAG节点
    set<struct DAG_node * > all_nodes_;

    //垃圾桶
    set<struct DAG_node * > trash_;

    //存储数据到DAG节点的映射，只有数据到最后一个DAG节点的映射是目前有效的，如果最后一个DAG节点是nullptr，那么就说明这个数据目前没有节点与之映射
    map<struct ic_data *,struct DAG_node * > data_to_node_;

    //还原成DAG节点的时候的顺序
    list<struct DAG_node * > nodes_order_;

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

    //把一个节点移到垃圾桶里
    void move_node_to_trash(struct DAG_node * node);

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

    //尝试进行函数调用的公共子表达式删除
    bool function_call_common_expression_delete(struct ic_data * result,struct ic_func * func,list<struct ic_data * > * r_params);

    //进行DAG节点的建立和节点之间关系的建立
    bool build_DAG_node_and_relation(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2=nullptr);

    //尝试将一个DAG树中的多个加法转换成乘法
    void a_lot_of_adds_to_multi_in_a_DAG_tree(struct DAG_node * father_node);
    //将多个加法转换成乘法
    void a_lot_of_adds_to_multi();

    //尝试将一棵DAG树中的n个自增1替换成一个自增n
    void n_selfadd_one_to_one_selfadd_n_in_a_DAG_tree(struct DAG_node * father_node);
    //将n个自增1替换成一个自增n
    void n_selfadd_one_to_one_selfadd_n();

    //在一棵DAG树中进行代数化简
    void algebraic_simplification_in_DAG_tree(struct DAG_node * father_node);
    //代数化简
    void algebraic_simplification();

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