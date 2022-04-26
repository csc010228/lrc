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

//带有变量定义和使用信息的中间代码
struct quaternion_with_def_use_info
{
    quaternion_with_def_use_info()
    {

    };

    quaternion_with_def_use_info(struct quaternion ic):intermediate_code(ic)//,explicit_def(nullptr)
    {
        /*struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
        struct ic_func * func=nullptr;
        list<struct ic_data * > * r_params=nullptr;
        switch(ic.op)
        {
            case ic_op::ASSIGN:
            case ic_op::NOT:
                result=(struct ic_data *)ic.result.second;
                arg1=(struct ic_data *)ic.arg1.second;
                if(result->is_array_member())
                {
                    if(!result->get_offset()->is_const())
                    {
                        uses.insert(result->get_offset());
                    }
                    // if(!result->get_belong_array()->is_const_var())
                    // {
                    //     uses.insert(result->get_belong_array());
                    // }
                }
                explicit_def=result;
                if(arg1->is_array_member() && !arg1->get_belong_array()->is_const_var())
                {
                    if(!arg1->get_offset()->is_const())
                    {
                        uses.insert(arg1->get_offset());
                    }
                    // uses.insert(arg1->get_belong_array());
                    uses.insert(arg1);
                }
                else if(!arg1->is_const())
                {
                    uses.insert(arg1);
                }
                break;
            case ic_op::ADD:
            case ic_op::SUB:
            case ic_op::MUL:
            case ic_op::DIV:
            case ic_op::MOD:
            //case ic_op::L_SHIFT:
            //case ic_op::R_SHIFT:
            case ic_op::AND:
            case ic_op::OR:
            case ic_op::XOR:
            case ic_op::EQ:
            case ic_op::UEQ:
            case ic_op::GT:
            case ic_op::LT:
            case ic_op::GE:
            case ic_op::LE:
                result=(struct ic_data *)ic.result.second;
                arg1=(struct ic_data *)ic.arg1.second;
                arg2=(struct ic_data *)ic.arg2.second;
                if(result->is_array_member())
                {
                    if(!result->get_offset()->is_const())
                    {
                        uses.insert(result->get_offset());
                    }
                    // if(!result->get_belong_array()->is_const_var())
                    // {
                    //     uses.insert(result->get_belong_array());
                    // }
                }
                explicit_def=result;
                if(arg1->is_array_member() && !arg1->get_belong_array()->is_const_var())
                {
                    if(!arg1->get_offset()->is_const())
                    {
                        uses.insert(arg1->get_offset());
                    }
                    // uses.insert(arg1->get_belong_array());
                    uses.insert(arg1);
                }
                else if(!arg1->is_const())
                {
                    uses.insert(arg1);
                }
                if(arg2->is_array_member() && !arg2->get_belong_array()->is_const_var())
                {
                    if(!arg2->get_offset()->is_const())
                    {
                        uses.insert(arg2->get_offset());
                    }
                    // uses.insert(arg2->get_belong_array());
                    uses.insert(arg2);
                }
                else if(!arg2->is_const())
                {
                    uses.insert(arg2);
                }
                break;
            case ic_op::IF_JMP:
            case ic_op::IF_NOT_JMP:
                arg1=(struct ic_data *)ic.arg1.second;
                if(arg1->is_array_member() && !arg1->get_belong_array()->is_const_var())
                {
                    if(!arg1->get_offset()->is_const())
                    {
                        uses.insert(arg1->get_offset());
                    }
                    // uses.insert(arg1->get_belong_array());
                    uses.insert(arg1);
                }
                else if(!arg1->is_const())
                {
                    uses.insert(arg1);
                }
                break;
            case ic_op::VAR_DEFINE:
                result=(struct ic_data *)ic.result.second;
                if(result->is_array_var() && !result->is_const_var() && result->get_value().bool_data==true)
                {
                    explicit_def=result;
                }
                break;
            case ic_op::CALL:
                result=(struct ic_data *)ic.result.second;
                func=(struct ic_func *)ic.arg1.second;
                r_params=(list<struct ic_data * > *)ic.arg2.second;
                if(result)
                {
                    if(result->is_array_member())
                    {
                        if(!result->get_offset()->is_const())
                        {
                            uses.insert(result->get_offset());
                        }
                        // if(!result->get_belong_array()->is_const_var())
                        // {
                        //     uses.insert(result->get_belong_array());
                        // }
                    }
                    explicit_def=result;
                }
                for(auto i:*r_params)
                {
                    if(i->is_array_member() && !i->get_belong_array()->is_const_var())
                    {
                        if(!i->get_offset()->is_const())
                        {
                            uses.insert(i->get_offset());
                        }
                        // uses.insert(i->get_belong_array());
                        uses.insert(i);
                    }
                    else if(!i->is_const())
                    {
                        uses.insert(i);
                    }
                    if(i->is_array_var() && !i->is_const_var())
                    {
                        vague_defs.insert(i);
                    }
                }
                break;
            default:
                break;
        }*/
    };

    //中间代码
    struct quaternion intermediate_code;
    /*//该中间代码明确定义的数据（一般只有一个）
    struct ic_data * explicit_def;
    //该中间代码模糊定义的数据（可以有多个）
    set<struct ic_data * > vague_defs;
    //该中间代码使用的数据（可以有多个）
    set<struct ic_data * > uses;*/
};

//中间代码的基本块
struct ic_basic_block
{
    ic_basic_block()
    {
        sequential_next=nullptr;
        jump_next=nullptr;
    };

    //设置该基本块顺序执行的时候的下一个基本块
    void set_sequential_next(struct ic_basic_block * next)
    {
        sequential_next=next;
    };

    //设置该基本块跳转执行的时候的下一个基本块
    void set_jump_next(struct ic_basic_block * next)
    {
        jump_next=next;
    };

    /*//获取最后生成的一条中间代码
    struct quaternion_with_def_use_info get_last_ic()
    {
        return ic_sequence.back();
    };

    //获取最后生成的一条中间代码的位置
    size_t get_last_ic_pos()
    {
        return ic_sequence.size()-1;
    };

    //记录一个变量被明确定义的位置
    void add_to_explicit_defs_pos(struct ic_data * ic_data,size_t pos)
    {
        set<size_t> empty;
        if(explicit_defs_pos.find(ic_data)==explicit_defs_pos.end())
        {
            explicit_defs_pos.insert(make_pair(ic_data,empty));
        }
        explicit_defs_pos.at(ic_data).insert(pos);
    };

    //记录一个变量被模糊的位置
    void add_to_vague_defs_pos(struct ic_data * ic_data,size_t pos)
    {
        set<size_t> empty;
        if(vague_defs_pos.find(ic_data)==vague_defs_pos.end())
        {
            vague_defs_pos.insert(make_pair(ic_data,empty));
        }
        vague_defs_pos.at(ic_data).insert(pos);
    };

    //记录一个变量被使用的位置
    void add_to_uses_pos(struct ic_data * ic_data,size_t pos)
    {
        set<size_t> empty;
        if(uses_pos.find(ic_data)==uses_pos.end())
        {
            uses_pos.insert(make_pair(ic_data,empty));
        }
        uses_pos.at(ic_data).insert(pos);
    };*/

    //往当前基本块中加入一条中间代码
    void add_ic(struct quaternion ic)
    {
        struct quaternion_with_def_use_info ic_with_def_use_info(ic);
        //size_t current_pos;
        ic_sequence.push_back(ic_with_def_use_info);
        //current_pos=get_last_ic_pos();
        //记录该中间代码的明确定义，模糊定义和使用
        /*explicit_defs.insert(ic_with_def_use_info.explicit_def);
        set_union(vague_defs.begin(),vague_defs.end(),ic_with_def_use_info.vague_defs.begin(),ic_with_def_use_info.vague_defs.end(),inserter(vague_defs,vague_defs.begin()));
        set_union(uses.begin(),uses.end(),ic_with_def_use_info.uses.begin(),ic_with_def_use_info.uses.end(),inserter(uses,uses.begin()));*/
        /*//同时记录下该中间代码的明确定义，模糊定义和使用及其位置
        if(ic_with_def_use_info.explicit_def)
        {
            add_to_explicit_defs_pos(ic_with_def_use_info.explicit_def,current_pos);
        }
        for(auto i:ic_with_def_use_info.vague_defs)
        {
            add_to_vague_defs_pos(i,current_pos);
        }
        for(auto i:ic_with_def_use_info.uses)
        {
            add_to_uses_pos(i,current_pos);
        }*/
    };

    //基本块中的中间代码序列
    vector<struct quaternion_with_def_use_info> ic_sequence;
    //该基本块的后续基本块
    struct ic_basic_block * sequential_next,* jump_next;
    /*//该基本块明确定义的变量
    set<struct ic_data * > explicit_defs;
    //该基本块模糊定义的变量
    set<struct ic_data * > vague_defs;
    //该基本块使用的变量
    set<struct ic_data * > uses;*/
    /*//变量的明确定义点
    map<struct ic_data *,set<size_t> > explicit_defs_pos;
    //变量的模糊定义点
    map<struct ic_data *,set<size_t> > vague_defs_pos;
    //变量的使用点
    map<struct ic_data *,set<size_t> > uses_pos;
    //可以到达该基本块入口的定义
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > in_defs;
    //可以到达该基本块出口的定义
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > out_defs;
    //从该基本块的入口处出发可以到达该基本块的引用
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > in_uses;
    //从该基本块的出口处出发可以到达该基本块的引用
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > out_uses;*/
};

//一个中间代码的函数的流图
struct ic_func_flow_graph
{
    ic_func_flow_graph(struct ic_func * func)
    {
        this->func=func;
    };

    ~ic_func_flow_graph()
    {
        for(auto i:basic_blocks)
        {
            delete i;
        }
    };

    /*//记录一个变量被明确定义的位置
    void add_to_explicit_defs_pos(struct ic_data * ic_data,struct ic_basic_block * basic_block,size_t pos)
    {
        set<pair<struct ic_basic_block *,size_t> > empty;
        if(explicit_defs_pos.find(ic_data)==explicit_defs_pos.end())
        {
            explicit_defs_pos.insert(make_pair(ic_data,empty));
        }
        explicit_defs_pos.at(ic_data).insert(make_pair(basic_block,pos));
    };

    //记录一个变量被模糊的位置
    void add_to_vague_defs_pos(struct ic_data * ic_data,struct ic_basic_block * basic_block,size_t pos)
    {
        set<pair<struct ic_basic_block *,size_t> > empty;
        if(vague_defs_pos.find(ic_data)==vague_defs_pos.end())
        {
            vague_defs_pos.insert(make_pair(ic_data,empty));
        }
        vague_defs_pos.at(ic_data).insert(make_pair(basic_block,pos));
    };

    //记录一个变量被使用的位置
    void add_to_uses_pos(struct ic_data * ic_data,struct ic_basic_block * basic_block,size_t pos)
    {
        set<pair<struct ic_basic_block *,size_t> > empty;
        if(uses_pos.find(ic_data)==uses_pos.end())
        {
            uses_pos.insert(make_pair(ic_data,empty));
        }
        uses_pos.at(ic_data).insert(make_pair(basic_block,pos));
    };*/

    //往当前的函数流图中加入一条中间代码
    void add_ic(struct quaternion ic)
    {
        static map<struct ic_label *,struct ic_basic_block *> ic_label_basic_block_map;
        static struct ic_basic_block * current_basic_block=nullptr;
        static bool previous_ic_is_jump=false;
        struct quaternion_with_def_use_info current_ic_with_def_use_info;
        size_t current_ic_with_def_use_info_pos;
        unsigned short new_basic_block_tag=0;
        bool func_end=false;
        if(func_end)
        {
            return;
        }
        
        //划分基本块
        if(previous_ic_is_jump)
        {
            //跳转语句的下一条语句是基本块的入口语句
            new_basic_block_tag=2;
            previous_ic_is_jump=false;
        }
        switch(ic.op)
        {
            case ic_op::JMP:
            case ic_op::IF_JMP:
            case ic_op::IF_NOT_JMP:
                previous_ic_is_jump=true;
                break;
            case ic_op::FUNC_DEFINE:
                //第一条语句是基本块的入口语句
                new_basic_block_tag=1;
                break;
            case ic_op::LABEL_DEFINE:
                //跳转语句跳转到的语句是基本块的入口语句
                new_basic_block_tag=3;
                break;
            case ic_op::END_FUNC_DEFINE:
                func_end=true;
                new_basic_block_tag=4;
                break;
            default:
                break;
        }
        switch(new_basic_block_tag)
        {
            case 0:
                break;
            case 1:
                current_basic_block=new struct ic_basic_block;
                new_basic_block_tag=0;
                break;
            case 2:
                basic_blocks.push_back(current_basic_block);
                current_basic_block=new struct ic_basic_block;
                basic_blocks.back()->set_sequential_next(current_basic_block);
                new_basic_block_tag=0;
                break;
            case 3:
                basic_blocks.push_back(current_basic_block);
                current_basic_block=new struct ic_basic_block;
                basic_blocks.back()->set_sequential_next(current_basic_block);
                ic_label_basic_block_map.insert(make_pair((struct ic_label *)ic.result.second,current_basic_block));
                new_basic_block_tag=0;
                break;
            case 4:
                basic_blocks.push_back(current_basic_block);
                new_basic_block_tag=0;
                break;
            default:
                new_basic_block_tag=0;
                break;
        }
        
        //将中间代码加入当前基本块中
        current_basic_block->add_ic(ic);
        
        /*current_ic_with_def_use_info=current_basic_block->get_last_ic();
        current_ic_with_def_use_info_pos=current_basic_block->get_last_ic_pos();
        //获取中间代码明确定义，模糊定义以及使用的全局变量和数组形参，并获取该中间代码明确定义，模糊定义以及使用的变量和它们的位置
        if(current_ic_with_def_use_info.explicit_def)
        {
            if(current_ic_with_def_use_info.explicit_def->is_global() || (current_ic_with_def_use_info.explicit_def->is_f_param() && current_ic_with_def_use_info.explicit_def->is_array_var()))
            {
                globals_and_array_f_params_explicit_def.insert(current_ic_with_def_use_info.explicit_def);
            }
            add_to_explicit_defs_pos(current_ic_with_def_use_info.explicit_def,current_basic_block,current_ic_with_def_use_info_pos);
        }
        for(auto i:current_ic_with_def_use_info.vague_defs)
        {
            if(i->is_global() || (i->is_f_param() && i->is_array_var()))
            {
                globals_and_array_f_params_vague_def.insert(i);
            }
            add_to_vague_defs_pos(i,current_basic_block,current_ic_with_def_use_info_pos);
        }
        for(auto i:current_ic_with_def_use_info.uses)
        {
            if(i->is_global() || (i->is_f_param() && i->is_array_var()))
            {
                globals_and_array_f_params_use.insert(i);
            }
            add_to_uses_pos(i,current_basic_block,current_ic_with_def_use_info_pos);
        }*/
        
        //如果该函数的流图构造完毕，那么就对各个基本块之间的跳转情况进行设置
        if(func_end)
        {
            for(auto i:basic_blocks)
            {
                if(i->ic_sequence.back().intermediate_code.op==ic_op::JMP || i->ic_sequence.back().intermediate_code.op==ic_op::IF_JMP || i->ic_sequence.back().intermediate_code.op==ic_op::IF_NOT_JMP)
                {
                    i->set_jump_next(ic_label_basic_block_map.at((struct ic_label *)(i->ic_sequence.back().intermediate_code.result.second)));
                }
                else
                {
                    i->set_jump_next(nullptr);
                }
            }
        }
    };

    //对应的函数在符号表中的指针
    struct ic_func * func;
    //函数流图中的所有基本块序列，顺序就是中间代码的书写顺序
    list<struct ic_basic_block * > basic_blocks;
    /*//函数中明确定义的所有全局变量和数组形参
    set<struct ic_data * > globals_and_array_f_params_explicit_def;
    //函数中模糊定义的所有全局变量和数组形参
    set<struct ic_data * > globals_and_array_f_params_vague_def;
    //函数中使用的所有全局变量和数组形参
    set<struct ic_data * > globals_and_array_f_params_use;
    //变量的明确定义点
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > explicit_defs_pos;
    //变量的模糊定义点
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > vague_defs_pos;
    //变量的使用点
    map<struct ic_data *,set<pair<struct ic_basic_block *,size_t> > > uses_pos;*/
};

//中间代码的流图表示
struct ic_flow_graph
{
    ic_flow_graph(list<struct quaternion> * intermediate_codes)
    {
        struct ic_func_flow_graph * current_ic_func_flow_graph=nullptr;
        for(auto i:*intermediate_codes)
        {
            if(i.op==ic_op::VAR_DEFINE && ((struct ic_data *)i.result.second)->is_global())
            {
                global_defines.push_back(i);
            }
            else
            {
                if(i.op==ic_op::FUNC_DEFINE)
                {
                    current_ic_func_flow_graph=new struct ic_func_flow_graph((struct ic_func *)i.result.second);
                    func_flow_graphs.push_back(current_ic_func_flow_graph);
                }
                current_ic_func_flow_graph->add_ic(i);
            }
        }
    };

    ~ic_flow_graph()
    {
        for(auto i:func_flow_graphs)
        {
            delete i;
        }
    };

    //所有函数的流图，这些流图之间相互独立
    list<struct ic_func_flow_graph * > func_flow_graphs;
    //全局定义域流图（里面的语句只能是变量定义语句）
    //struct ic_basic_block * global_basic_block;
    list<struct quaternion> global_defines;
};

//中间代码优化器
class Ic_optimizer
{

protected:
    //目前正在处理的中间代码流图
    struct ic_flow_graph * intermediate_codes_flow_graph_;

    //局部优化
    void local_optimize();

    //数据流分析
    void data_flow_analysis();

    //全局优化
    void global_optimize();

public:
    //构造函数
    Ic_optimizer();

    //初始化
    void init();

    //代码优化
    struct ic_flow_graph * optimize(list<struct quaternion> * intermediate_codes);

    //代码优化并将最终优化的中间代码流图输出到文件中
    struct ic_flow_graph * optimize_then_output(list<struct quaternion> * intermediate_codes,const char * filename);
};

#endif //__IC_OPTIMIZER_H