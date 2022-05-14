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
    //该DAG节点的操作符
    enum DAG_node_op op;

    //该DAG节点关联的变量，以及它们出现在基本块中的位置
    list<pair<struct ic_data *,size_t > > related_ic_datas_and_their_pos;

    //该DAG节点的所有父亲节点
    set<struct DAG_node * > fathers;

    //该DAG节点的孩子节点
    list<struct DAG_node * > children;

    //创建一个DAG的叶子节点
    DAG_node(struct ic_data * ic_data,size_t pos):op(DAG_node_op::NOP)
    {
        add_related_data(ic_data,pos);
    };

    //创建一个DAG的非叶子节点
    DAG_node(enum DAG_node_op op,struct ic_data * ic_data,size_t pos,size_t children_num,...):op(op)
    {
        // struct DAG_node * child;
        // va_list argptr;
	    // va_start(argptr,children_num);
        // for(size_t i=0;i<children_num;i++)
        // {
        //     child=va_arg(argptr,struct DAG_node *);
        //     child->add_father(this);
        //     children.push_back(child);
        // }
        // va_end(argptr);
        // add_related_data(ic_data,pos);
    };

    //创建一个DAG的非叶子节点
    DAG_node(enum DAG_node_op op,struct ic_data * ic_data,size_t pos,list<struct DAG_node * > children):op(op),children(children)
    {
        // for(auto child:children)
        // {
        //     child->add_father(this);
        // }
        // add_related_data(ic_data,pos);
    };

    //为该DAG节点增加一个父亲节点
    inline void add_father(struct DAG_node * father)
    {
        // fathers.insert(father);
    };

    //新关联一个变量
    inline void add_related_data(struct ic_data * ic_data,size_t pos)
    {
        // related_ic_datas_and_their_pos.push_back(make_pair(ic_data,pos));
    };

};

//DAG的类
class DAG
{
private:
    //该DAG中的所有祖先节点（即没有父亲节点的节点）
    set<struct DAG_node * > ancestors;

    //所有的DAG节点
    set<struct DAG_node * > nodes;

public:
    //构造函数
    DAG(struct DAG_node * ancestor);

    //析构函数
    ~DAG();

    //把一个DAG还原成一系列的中间代码
    vector<struct quaternion> to_ic_sequence();
};

//多个DAG
class DAGs
{
private:
    //所有的DAG图
    list<DAG * > all_DAGs;

    //DAG的节点到DAG图的映射
    map<struct DAG_node *,DAG * > DAG_node_to_DAG;

    //变量到DAG节点的映射
    map<struct ic_data *,struct DAG_node * > data_to_node;

    //获取一个变量目前关联的节点，如果没有的话返回nullptr
    struct DAG_node * get_data_s_node(struct ic_data * data);

    //销毁一个变量目前关联的节点
    void unattach_data_s_node(struct ic_data * data);

public:
    //构造函数
    DAGs(struct ic_basic_block * basic_block);

    //析构函数
    ~DAGs();

    //把多个DAG还原成一个基本块
    struct ic_basic_block * to_basic_block();
};

#endif //__DAG_H