/*
 *
 *  ic_optimizer.cpp
 *  中间代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/3/8
 *
*/
#include"ic_optimizer.h"
#include<fstream>
#include<iostream>

//中间代码输出信息
extern map<ic_op,ic_output> ic_outputs;

Ic_optimizer::Ic_optimizer()
{

}

/*
初始化，设置优化选项
*/
void Ic_optimizer::init()
{
    
}

/*
强度削弱

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::reduction_in_strength(struct ic_basic_block * basic_block)
{
    struct quaternion ic;
    struct ic_data * arg1,* arg2;
    for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
    {
        ic=(*ic_with_info).intermediate_code;
        arg1=(ic_data *)ic.arg1.second;
        arg2=(ic_data *)ic.arg2.second;
        switch(ic.op)
        {
            case ic_op::ADD:
                if(arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg2,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                else if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                break;
            case ic_op::SUB:
                if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                break;
            case ic_op::MUL:
                if(arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==1) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==1.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg2,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                else if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                break;
            case ic_op::DIV:
                if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.0f)))
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                break;
            case ic_op::MOD:
                if(arg2->is_const() && arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1)
                {
                    (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,ic.result.second));
                }
                break;
            case ic_op::IF_JMP:
                if(arg1->is_const() && arg1->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data==0)
                    {
                        (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::NOP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
                    }
                    else
                    {
                        (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,ic.result.second));
                        basic_block->set_sequential_next(nullptr);
                    }
                }
                break;
            case ic_op::IF_NOT_JMP:
                if(arg1->is_const() && arg1->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data!=0)
                    {
                        (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::NOP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
                    }
                    else
                    {
                        (*ic_with_info)=quaternion_with_def_use_info(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,ic.result.second));
                        basic_block->set_sequential_next(nullptr);
                    }
                }
                break;
            default:
                break;
        }
    }
}

/*
常量合并

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::constant_folding(struct ic_basic_block * basic_block)
{
    /*struct quaternion ic;
    struct ic_data * arg1,* arg2;
    for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
    {

    }*/
}

/*
复制传播

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::copy_progagation(struct ic_basic_block * basic_block)
{

}

/*
局部公共子表达式删除

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::local_elimination_of_common_subexpression(struct ic_basic_block * basic_block)
{

}

/*
局部死代码消除

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::local_dead_code_elimination(struct ic_basic_block * basic_block)
{

}

/*
局部优化
*/
void Ic_optimizer::local_optimize()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            //强度削弱
            reduction_in_strength(basic_block);
            //常量合并
            constant_folding(basic_block);
            //复制传播
            copy_progagation(basic_block);
            //局部公共子表达式删除
            local_elimination_of_common_subexpression(basic_block);
            //局部死代码消除
            local_dead_code_elimination(basic_block);
        }
    }
}

/*
到达-定义分析

Parameters
----------
func:要分析的函数流图
*/
void Ic_optimizer::use_define_analysis(struct ic_func_flow_graph * func)
{

}

/*
活跃变量分析

Parameters
----------
func:要分析的函数流图
*/
void Ic_optimizer::live_variable_analysis(struct ic_func_flow_graph * func)
{

}

/*
可用表达式分析

Parameters
----------
func:要分析的函数流图
*/
void Ic_optimizer::available_expression_analysis(struct ic_func_flow_graph * func)
{

}

/*
数据流分析
*/
void Ic_optimizer::data_flow_analysis()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        //到达-定义分析
        use_define_analysis(func);
        //活跃变量分析
        live_variable_analysis(func);
        //可用表达式分析
        available_expression_analysis(func);
    }
}

/*
全局公共子表达式删除

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::global_elimination_of_common_subexpression(struct ic_func_flow_graph * func)
{

}

/*
全局死代码消除

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::global_dead_code_elimination(struct ic_func_flow_graph * func)
{

}

/*
循环不变量外提

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::loop_invariant_computation_motion(struct ic_func_flow_graph * func)
{

}

/*
归纳变量删除

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::induction_variable_elimination(struct ic_func_flow_graph * func)
{

}

/*
函数内联

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::function_inline(struct ic_func_flow_graph * func)
{

}

/*
全局优化
*/
void Ic_optimizer::global_optimize()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        //全局公共子表达式删除
        global_elimination_of_common_subexpression(func);
        //全局死代码消除
        global_dead_code_elimination(func);
        //循环不变量外提
        loop_invariant_computation_motion(func);
        //归纳变量删除
        induction_variable_elimination(func);
    }
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        //函数内联
        function_inline(func);
    }
}

/*
代码优化

Parameters
----------
intermediate_codes:需要优化的中间代码序列

Return
------
返回最终的流图
*/
struct ic_flow_graph * Ic_optimizer::optimize(list<struct quaternion> * intermediate_codes)
{
    //建立中间代码流图
    intermediate_codes_flow_graph_=new struct ic_flow_graph(intermediate_codes);
    //进行局部优化
    local_optimize();
    //进行数据流分析
    data_flow_analysis();
    //进行全局优化
    global_optimize();
    //返回优化结果
    return intermediate_codes_flow_graph_;
}

/*
代码优化并将最终优化的中间代码流图输出到文件中

Parameters
----------
intermediate_codes:需要优化的中间代码序列
filename：要输出的文件名

Return
------
返回最终的流图
*/
struct ic_flow_graph * Ic_optimizer::optimize_then_output(list<struct quaternion> * intermediate_codes,const char * filename)
{
	ofstream outFile;
    struct ic_flow_graph * res;

    //进行优化
    res=optimize(intermediate_codes);

    if(intermediate_codes)
    {
        //把中间代码流图输出到文件中
        outFile.open(filename,ios::out);
	    if (!outFile)
        {
            cout<<"Open file "<<filename<<" error!"<<endl;
	    }
        else
        {
            //先输出全局定义
            for(auto i:res->global_defines)
            {
                outFile<<(ic_outputs[i.op](i))<<endl;
            }
            //再输出函数流图
            for(auto i:res->func_flow_graphs)
            {
                for(auto j:i->basic_blocks)
                {
                    outFile<<"=========================================BASIC_BLOCK:"<<(j)<<"========================================="<<endl;
                    for(auto k:j->ic_sequence)
                    {
                        outFile<<(ic_outputs[k.intermediate_code.op](k.intermediate_code))<<endl;
                    }
                    if(j->jump_next)
                    {
                        outFile<<"=========================================MAY_JUMP_TO:"<<(j->jump_next)<<"========================================="<<endl;
                    }
                    else
                    {
                        outFile<<"============================================================================================================"<<endl;
                    }
                }
            }
            outFile.close();
        }
    }

    return res;
}