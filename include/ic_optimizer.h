/*
 *
 *  ic_optimizer.h
 *  中间代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/3/8
 *
*/
#ifndef __IC_OPTIMIZER_H
#define __IC_OPTIMIZER_H

#include<algorithm>
#include "semantic_rules.h"

//中间代码的位置
typedef struct ic_pos
{
    ic_pos();

    ic_pos(struct ic_basic_block * basic_block,size_t offset);

    bool operator <(const ic_pos & a) const
    {
        return (this->basic_block<a.basic_block || (this->basic_block==a.basic_block && this->offset<a.offset));
    };

    bool operator >(const ic_pos & a) const
    {
        return (this->basic_block>a.basic_block || (this->basic_block==a.basic_block && this->offset>a.offset));
    };

    bool operator <=(const ic_pos & a) const
    {
        return (this->basic_block<=a.basic_block || (this->basic_block==a.basic_block && this->offset<=a.offset));
    };

    bool operator >=(const ic_pos & a) const
    {
        return (this->basic_block>=a.basic_block || (this->basic_block==a.basic_block && this->offset>=a.offset));
    };

    bool operator ==(const ic_pos & a) const
    {
        return (this->basic_block==a.basic_block && this->offset==a.offset);
    };

    bool operator !=(const ic_pos & a) const
    {
        return (this->basic_block!=a.basic_block || this->offset!=a.offset);
    };

    struct ic_basic_block * basic_block;
    size_t offset;
} ic_pos;

//带有变量定义和使用信息的中间代码
struct quaternion_with_def_use_info
{
    quaternion_with_def_use_info();

    quaternion_with_def_use_info(struct quaternion ic);

    //根据中间代码建立信息
    void build_info(bool clear_data_flow_analysis_info=true);

    //中间代码化简
    void simplify();

    //尝试将一个数据放入该中间代码的明确定义数据
    void set_explicit_def(struct ic_data * data);

    //尝试将一个数据放入该中间代码的模糊定义数据
    void add_to_vague_defs(struct ic_data * data);

    //尝试将一个数据放入该中间代码的使用数据
    void add_to_uses(struct ic_data * data);

    //尝试将一个ud-链的数据放入ud-链
    void add_to_ud_chain(struct ic_data * data,set<ic_pos> poses);

    //尝试将一个du-链的数据放入ud-链
    void add_to_du_chain(struct ic_data * data,set<ic_pos> poses);

    //将该条中间代码使用的某一个数据替换成另一个常量数据
    void replace_used_data(struct ic_data * source,struct ic_data * destination);

    //中间代码
    struct quaternion intermediate_code;

    //该中间代码明确定义的数据（一般只有一个）
    struct ic_data * explicit_def;
    //该中间代码模糊定义的数据（可以有多个）
    set<struct ic_data * > vague_defs;
    //该中间代码使用的数据（可以有多个）
    set<struct ic_data * > uses;

    //ud-链信息
    map<struct ic_data *,set<ic_pos> > ud_chain;
    //du-链信息
    map<struct ic_data *,set<ic_pos> > du_chain;
};

//中间代码的基本块
struct ic_basic_block
{
    ic_basic_block(struct ic_func_flow_graph * belong_func_flow_graph);

    //设置该基本块顺序执行的时候的下一个基本块
    void set_sequential_next(struct ic_basic_block * next);

    //设置该基本块跳转执行的时候的下一个基本块
    void set_jump_next(struct ic_basic_block * next);

    //往当前基本块中加入一条中间代码
    void add_ic(struct quaternion ic);

    //获取一个基本块的所有前驱基本块
    set<struct ic_basic_block * > get_precursors();

    //获取一个基本块的所有后继基本块
    set<struct ic_basic_block * > get_successors();

    //基本块中的中间代码序列
    vector<struct quaternion_with_def_use_info> ic_sequence;
    //该基本块的后续基本块
    struct ic_basic_block * sequential_next,* jump_next;
    //该基本块的所属函数
    struct ic_func_flow_graph * belong_func_flow_graph;

    //到达-定义分析信息
    struct
    {
        map<struct ic_data *,set<ic_pos> > in;
        map<struct ic_data *,set<ic_pos> > out;
    } use_def_analysis_info;

    //活跃变量分析信息
    struct
    {
        set<struct ic_data * > in;
        set<struct ic_data * > out;
    } live_analysis_info;
};

//一个中间代码的函数的流图
struct ic_func_flow_graph
{
    ic_func_flow_graph(struct ic_func * func);

    ~ic_func_flow_graph();

    //往当前的函数流图中加入一条中间代码
    void add_ic(struct quaternion ic);

    //获取指定位置的中间代码及其信息
    static struct quaternion_with_def_use_info get_ic_with_info(ic_pos pos);

    //对应的函数在符号表中的指针
    struct ic_func * func;
    //函数流图中的所有基本块序列，顺序就是中间代码的书写顺序
    list<struct ic_basic_block * > basic_blocks;
};

//中间代码的流图表示
struct ic_flow_graph
{
    ic_flow_graph(list<struct quaternion> * intermediate_codes);

    ~ic_flow_graph();

    //所有函数的流图，这些流图之间相互独立
    list<struct ic_func_flow_graph * > func_flow_graphs;
    //全局定义域流图（里面的语句只能是变量定义语句）
    list<struct quaternion> global_defines;
};

//中间代码优化器
class Ic_optimizer
{

protected:
    //是否需要优化
    bool need_optimize_;

    //目前正在处理的中间代码流图
    struct ic_flow_graph * intermediate_codes_flow_graph_;

    //强度削弱
    void reduction_in_strength(struct ic_basic_block * basic_block);
    //常量合并
    void constant_folding(struct ic_basic_block * basic_block);
    //复制传播
    void copy_progagation(struct ic_basic_block * basic_block);
    //公共子表达式删除
    void local_elimination_of_common_subexpression(struct ic_basic_block * basic_block);
    //局部死代码消除
    void local_dead_code_elimination(struct ic_basic_block * basic_block);

    //局部优化
    void local_optimize();
    
    //到达-定义分析
    void use_define_analysis(struct ic_func_flow_graph * func);
    //构建ud-链
    void build_ud_chain(struct ic_func_flow_graph * func);
    //活跃变量分析
    void live_variable_analysis(struct ic_func_flow_graph * func);
    //构建du-链
    void build_du_chain(struct ic_func_flow_graph * func);
    //可用表达式分析
    void available_expression_analysis(struct ic_func_flow_graph * func);
    
    //数据流分析
    void data_flow_analysis();

    //全局常量合并
    void globale_constant_folding(struct ic_func_flow_graph * func);
    //全局公共子表达式删除
    void global_elimination_of_common_subexpression(struct ic_func_flow_graph * func);
    //全局死代码消除
    void global_dead_code_elimination(struct ic_func_flow_graph * func);
    //循环不变量外提
    void loop_invariant_computation_motion(struct ic_func_flow_graph * func);
    //归纳变量删除
    void induction_variable_elimination(struct ic_func_flow_graph * func);
    //函数内联
    void function_inline(struct ic_func_flow_graph * func);

    //全局优化
    void global_optimize();

public:
    //构造函数
    Ic_optimizer();

    //初始化
    void init(bool optimize);

    //代码优化
    struct ic_flow_graph * optimize(list<struct quaternion> * intermediate_codes);

    //代码优化并将最终优化的中间代码流图输出到文件中
    struct ic_flow_graph * optimize_then_output(list<struct quaternion> * intermediate_codes,const char * filename);
};

#endif //__IC_OPTIMIZER_H