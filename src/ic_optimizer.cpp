/*
 *
 *  ic_optimizer.cpp
 *  中间代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/3/8
 *
*/
#include"ic_optimizer.h"
#include"ic_local_optimize.h"
#include"ic_global_optimize.h"
#include"pool.h"
#include"union_optimize.h"
#include"data_flow_analyse.h"
#include"symbol_table.h"
#include<fstream>
#include<iostream>
#include<algorithm>

//中间代码输出信息
extern map<ic_op,ic_output> ic_outputs;



//===================================== struct ic_pos =====================================//

ic_pos::ic_pos():basic_block(nullptr),offset(0)
{

}

ic_pos::ic_pos(struct ic_basic_block * basic_block,size_t offset):basic_block(basic_block),offset(offset)
{

}

//==========================================================================//



//===================================== struct quaternion_with_info =====================================//

quaternion_with_info::quaternion_with_info():intermediate_code(quaternion()),explicit_def(nullptr)
{

}

quaternion_with_info::quaternion_with_info(struct quaternion ic):intermediate_code(ic)
{
    simplify();
    build_info();
}

//根据中间代码建立信息
void quaternion_with_info::build_info(bool clear_data_flow_analysis_info)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
    struct ic_func * func=nullptr;
    list<struct ic_data * > * r_params=nullptr;
    set<struct ic_data * > func_def_globals_and_f_params;
    explicit_def=nullptr;
    vague_defs.clear();
    uses.clear();
    if(clear_data_flow_analysis_info)
    {
        ud_chain.clear();
        du_chain.clear();
    }
    switch(intermediate_code.op)
    {
        case ic_op::ASSIGN:
        case ic_op::NOT:
            result=(struct ic_data *)intermediate_code.result.second;
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            set_explicit_def(result);
            add_to_uses(arg1);
            break;
        case ic_op::ADD:
        case ic_op::SUB:
        case ic_op::MUL:
        case ic_op::DIV:
        case ic_op::MOD:
        case ic_op::EQ:
        case ic_op::UEQ:
        case ic_op::GT:
        case ic_op::LT:
        case ic_op::GE:
        case ic_op::LE:
            result=(struct ic_data *)intermediate_code.result.second;
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            arg2=(struct ic_data *)intermediate_code.arg2.second;
            set_explicit_def(result);
            add_to_uses(arg1);
            add_to_uses(arg2);
            break;
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            add_to_uses(arg1);
            break;
        case ic_op::VAR_DEFINE:
            result=(struct ic_data *)intermediate_code.result.second;
            if(result->is_array_var() && result->get_value().bool_data==true)
            {
                set_explicit_def(result);
            }
            break;
        case ic_op::CALL:
            if(intermediate_code.result.first==ic_operand::DATA)
            {
                result=(struct ic_data *)intermediate_code.result.second;
                set_explicit_def(result);
            }
            func=(struct ic_func *)intermediate_code.arg1.second;
            r_params=(list<struct ic_data * > *)intermediate_code.arg2.second;
            //把调用的函数可能更改的全局变量，数组形参对应的实参设置为模糊定义
            for(auto r_param:*r_params)
            {
                add_to_uses(r_param);
                if(r_param->is_array_var())
                {
                    add_to_vague_defs(r_param);
                }
            }
            func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_array_f_params(func);
            for(auto vague_var:func_def_globals_and_f_params)
            {
                if(vague_var->is_global())
                {
                    add_to_vague_defs(vague_var);
                }
            }
            break;
        case ic_op::RET:
            if(intermediate_code.result.first==ic_operand::DATA)
            {
                result=(struct ic_data *)intermediate_code.result.second;
                add_to_uses(result);
            }
            break;
        default:
            break;
    }
}

//中间代码化简
void quaternion_with_info::simplify()
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
    arg1=(struct ic_data *)intermediate_code.arg1.second;
    arg2=(struct ic_data *)intermediate_code.arg2.second;
    result=(struct ic_data *)intermediate_code.result.second;
    switch(intermediate_code.op)
    {
        case ic_op::ADD:
            if(arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg2,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,(int)(arg1->get_value().int_data+arg2->get_value().int_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::FLOAT,(float)(arg1->get_value().float_data+arg2->get_value().float_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::SUB:
            if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,(int)(arg1->get_value().int_data-arg2->get_value().int_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::FLOAT,(float)(arg1->get_value().float_data-arg2->get_value().float_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::MUL:
            if(arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==1) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==1.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg2,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if((arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.0f))) || (arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.0f))))
            {
                if(result->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(result->get_data_type(),OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(result->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(result->get_data_type(),OAA((float)0.0f)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,(int)(arg1->get_value().int_data*arg2->get_value().int_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::FLOAT,(float)(arg1->get_value().float_data*arg2->get_value().float_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::DIV:
            if(arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.0f)))
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,(int)(arg1->get_value().int_data/arg2->get_value().int_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::FLOAT,(float)(arg1->get_value().float_data/arg2->get_value().float_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::MOD:
            if(arg2->is_const() && arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,(int)(arg1->get_value().int_data%arg2->get_value().int_data)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::NOT:
            if(arg1->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,(arg1->get_value().int_data==0)?symbol_table->const_entry(language_data_type::INT,OAA((int)1)):symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT)
                {
                    intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,(arg1->get_value().float_data==0.0f)?symbol_table->const_entry(language_data_type::INT,OAA((int)1)):symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                }
            }
            break;
        case ic_op::EQ:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data==arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data==arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::UEQ:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data!=arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data!=arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::GE:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data>=arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data>=arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::GT:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data>arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data>arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::LE:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data<=arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data<=arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::LT:
            if(arg1==arg2)
            {
                intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
            }
            else if(arg1->is_const() && arg2->is_const())
            {
                if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::INT)
                {
                    if(arg1->get_value().int_data<arg2->get_value().int_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
                else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::FLOAT)
                {
                    if(arg1->get_value().float_data<arg2->get_value().float_data)
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)1)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                    else
                    {
                        intermediate_code=(quaternion(ic_op::ASSIGN,ic_operand::DATA,symbol_table->const_entry(language_data_type::INT,OAA((int)0)),ic_operand::NONE,nullptr,ic_operand::DATA,result));
                    }
                }
            }
            break;
        case ic_op::IF_JMP:
            if(arg1->is_const() && arg1->get_data_type()==language_data_type::INT)
            {
                if(arg1->get_value().int_data==0)
                {
                    intermediate_code=(quaternion(ic_op::NOP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
                }
                else
                {
                    intermediate_code=(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,result));
                }
            }
            break;
        case ic_op::IF_NOT_JMP:
            if(arg1->is_const() && arg1->get_data_type()==language_data_type::INT)
            {
                if(arg1->get_value().int_data!=0)
                {
                    intermediate_code=(quaternion(ic_op::NOP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
                }
                else
                {
                    intermediate_code=(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,result));
                }
            }
            break;
        default:
            break;
    }
}

void quaternion_with_info::clear_all_data_flow_analyse_info()
{
    ud_chain.clear();
    du_chain.clear();
}

void quaternion_with_info::set_explicit_def(struct ic_data * data)
{
    if(!data->is_const())
    {
        if(data->is_array_member())
        {
            //add_to_uses(data->get_belong_array());
            add_to_uses(data->get_offset());
            if(data->is_array_var())
            {
                explicit_def=data->get_belong_array();
                return;
            }
        }
        explicit_def=data;
    }
}

void quaternion_with_info:: add_to_vague_defs(struct ic_data * data)
{
    if(!data->is_const())
    {
        if(data->is_array_member())
        {
            //add_to_uses(data->get_belong_array());
            add_to_uses(data->get_offset());
            if(data->is_array_var())
            {
                add_to_vague_defs(data->get_belong_array());
                return;
            }
        }
        vague_defs.insert(data);
    }
}

void quaternion_with_info::add_to_uses(struct ic_data * data)
{
    if(!data->is_const())
    {
        uses.insert(data);
        if(data->is_array_member())
        {
            add_to_uses(data->get_belong_array());
            add_to_uses(data->get_offset());
        }
    }
}

set<struct ic_data * > quaternion_with_info::get_all_datas() const
{
    set<struct ic_data * > res;
    if(explicit_def)
    {
        res.insert(explicit_def);
    }
    set_union(vague_defs.begin(),vague_defs.end(),res.begin(),res.end(),inserter(res,res.begin()));
    set_union(uses.begin(),uses.end(),res.begin(),res.end(),inserter(res,res.begin()));
    return res;
}

struct ic_label * quaternion_with_info::get_related_label() const
{
    switch(intermediate_code.op)
    {
        case ic_op::JMP:
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
        case ic_op::LABEL_DEFINE:
            return (struct ic_label * )intermediate_code.result.second;
            break;
        default:
            break;
    }
    return nullptr;
}

void quaternion_with_info::add_to_ud_chain(struct ic_data * data,set<ic_pos> poses)
{
    if(!data->is_const())
    {
        if(ud_chain.find(data)==ud_chain.end())
        {
            ud_chain.insert(make_pair(data,poses));
        }
        else
        {
            set_union(poses.begin(),poses.end(),ud_chain.at(data).begin(),ud_chain.at(data).end(),inserter(ud_chain.at(data),ud_chain.at(data).begin()));
        }
    }
}

void quaternion_with_info::add_to_du_chain(struct ic_data * data,set<ic_pos> poses)
{
    if(!data->is_const())
    {
        if(du_chain.find(data)==du_chain.end())
        {
            du_chain.insert(make_pair(data,poses));
        }
        else
        {
            set_union(poses.begin(),poses.end(),du_chain.at(data).begin(),du_chain.at(data).end(),inserter(du_chain.at(data),du_chain.at(data).begin()));
        }
    }
}

//尝试将一个du-链的数据放入ud-链
void quaternion_with_info::add_to_du_chain(struct ic_data * data,ic_pos pos)
{
    if(!data->is_const())
    {
        if(du_chain.find(data)==du_chain.end())
        {
            du_chain.insert(make_pair(data,set<ic_pos>()));
        }
        du_chain.at(data).insert(pos);
    }
}

//查看当前的中间代码是否定义了全局变量或者数组函数形参
bool quaternion_with_info::check_if_def_global_or_f_param_array()
{
    if(explicit_def && (explicit_def->is_global() || (explicit_def->is_array_member() && (explicit_def->get_belong_array()->is_f_param() || explicit_def->get_belong_array()->is_global())) || (explicit_def->is_array_var() && explicit_def->is_f_param())))
    {
        return true;
    }
    for(auto vauge_def:vague_defs)
    {
        if(vauge_def && (vauge_def->is_global() || (vauge_def->is_array_member() && (vauge_def->get_belong_array()->is_f_param() || vauge_def->get_belong_array()->is_global())) || (vauge_def->is_array_var() && vauge_def->is_f_param())))
        {
            return true;
        }
    }
    return false;
}

//将该条中间代码中使用的某一个数据替换成另一个数据
void quaternion_with_info::replace_datas(struct ic_data * source,struct ic_data * destination,bool only_use_datas,bool update_info)
{
    intermediate_code.replace_datas(source,destination,only_use_datas);
    simplify();
    if(update_info)
    {
        build_info();
    }
}

//将中间代码中涉及的所有数据都进行替换
void quaternion_with_info::replace_all_vars(const map<struct ic_data *,struct ic_data *> & old_and_new_vars_map)
{
    intermediate_code.replace_all_vars(old_and_new_vars_map);
    simplify();
}

//==========================================================================//



//===================================== struct ic_basic_block =====================================//

ic_basic_block::ic_basic_block(struct ic_func_flow_graph * belong_func_flow_graph):sequential_next(nullptr),jump_next(nullptr),belong_func_flow_graph(belong_func_flow_graph)
{
    
};

ic_basic_block::ic_basic_block(const struct ic_basic_block & source,struct ic_func_flow_graph * new_belong_func_flow_graph,const map<struct ic_data *,struct ic_data * > & old_and_new_vars_map,const map<struct ic_label *,struct ic_label * > & old_and_new_labels_map):sequential_next(source.sequential_next),jump_next(source.jump_next),belong_func_flow_graph(new_belong_func_flow_graph)
{
    struct quaternion old_ic;
    for(auto old_ic_with_info:source.ic_sequence)
    {
        old_ic=old_ic_with_info.intermediate_code;
        old_ic.replace_all_vars(old_and_new_vars_map);
        old_ic.replace_all_labels(old_and_new_labels_map);
        add_ic(old_ic);
    }
}

void ic_basic_block::set_sequential_next(struct ic_basic_block * next)
{
    sequential_next=next;
}

void ic_basic_block::set_jump_next(struct ic_basic_block * next)
{
    jump_next=next;
}

void ic_basic_block::add_ic_info(struct quaternion_with_info ic_with_info)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    set<struct ic_data * > all_datas;
    if(ic_with_info.explicit_def!=nullptr)
    {
        if(ic_with_info.explicit_def->is_array_member())
        {
            symbol_table->add_func_def_globals_and_f_params(belong_func_flow_graph->func,ic_with_info.explicit_def->get_belong_array());
        }
        else
        {
            symbol_table->add_func_def_globals_and_f_params(belong_func_flow_graph->func,ic_with_info.explicit_def);
        }
        all_datas.insert(ic_with_info.explicit_def);
    }
    // for(auto vague_var:ic_with_info.vague_defs)
    // {
    //     symbol_table->add_func_def_globals_and_f_params(belong_func_flow_graph->func,vague_var);
    //     all_datas.insert(vague_var);
    // }
    for(auto use_var:ic_with_info.uses)
    {
        symbol_table->add_func_use_globals_and_f_params(belong_func_flow_graph->func,use_var);
        all_datas.insert(use_var);
    }
    if(ic_with_info.intermediate_code.op==ic_op::CALL)
    {
        symbol_table->add_func_direct_calls(belong_func_flow_graph->func,(struct ic_func *)ic_with_info.intermediate_code.arg1.second,(list<struct ic_data * > *)ic_with_info.intermediate_code.arg2.second);
    }
    for(auto data:all_datas)
    {
        if(data->is_array_member())
        {
            if(array_to_array_member_map.find(data->get_belong_array())==array_to_array_member_map.end())
            {
                array_to_array_member_map.insert(make_pair(data->get_belong_array(),set<struct ic_data * >()));
            }
            if(array_to_array_member_map.at(data->get_belong_array()).find(data)==array_to_array_member_map.at(data->get_belong_array()).end())
            {
                array_to_array_member_map.at(data->get_belong_array()).insert(data);
            }
            if(offset_to_array_member_map.find(data->get_offset())==offset_to_array_member_map.end())
            {
                offset_to_array_member_map.insert(make_pair(data->get_offset(),set<struct ic_data * >()));
            }
            if(offset_to_array_member_map.at(data->get_offset()).find(data)==offset_to_array_member_map.at(data->get_offset()).end())
            {
                offset_to_array_member_map.at(data->get_offset()).insert(data);
            }
        }
    }
}

void ic_basic_block::add_ic(struct quaternion ic,bool do_not_add_ic_info)
{
    struct quaternion_with_info ic_with_def_use_info(ic);
    ic_sequence.push_back(ic_with_def_use_info);
    if(!do_not_add_ic_info)
    {
        add_ic_info(ic_with_def_use_info);
    }
}

void ic_basic_block::add_ic_to_front(struct quaternion ic)
{
    struct quaternion_with_info ic_with_def_use_info(ic);
    ic_sequence.insert(ic_sequence.begin(),ic_with_def_use_info);
    add_ic_info(ic_with_def_use_info);
}

void ic_basic_block::clear_ic_sequence()
{
    ic_sequence.clear();
    array_to_array_member_map.clear();
    offset_to_array_member_map.clear();
}

set<struct ic_basic_block * > ic_basic_block::get_precursors()
{
    if(precursors.empty())
    {
        for(auto basic_block:belong_func_flow_graph->basic_blocks)
        {
            if(basic_block->sequential_next==this || basic_block->jump_next==this)
            {
                precursors.insert(basic_block);
            }
        }
    }
    return precursors;
}

set<struct ic_basic_block * > ic_basic_block::get_successors()
{
    set<struct ic_basic_block * > res;
    if(sequential_next)
    {
        res.insert(sequential_next);
    }
    if(jump_next)
    {
        res.insert(jump_next);
    }
    return res;
}

void ic_basic_block::clear_all_data_flow_analyse_info()
{
    use_def_analysis_info.in.clear();
    use_def_analysis_info.out.clear();
    live_analysis_info.in.clear();
    live_analysis_info.out.clear();
}


//==========================================================================//



//===================================== struct ic_func_flow_graph =====================================//

ic_func_flow_graph::ic_func_flow_graph(struct ic_func * func)
{
    this->func=func;
}

ic_func_flow_graph::~ic_func_flow_graph()
{
    for(auto i:basic_blocks)
    {
        delete i;
    }
}

//往当前的函数流图中加入一条中间代码
void ic_func_flow_graph::add_ic(struct quaternion ic)
{
    static struct ic_basic_block * current_basic_block=nullptr;
    static bool previous_ic_is_jump=false,basic_block_has_return=false;
    unsigned char new_basic_block_tag=0;
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
            previous_ic_is_jump=false;
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
            current_basic_block=new struct ic_basic_block(this);
            basic_block_has_return=false;
            break;
        case 2:
            basic_blocks.push_back(current_basic_block);
            current_basic_block=new struct ic_basic_block(this);
            basic_block_has_return=false;
            break;
        case 3:
            basic_blocks.push_back(current_basic_block);
            current_basic_block=new struct ic_basic_block(this);
            basic_block_has_return=false;
            label_basic_block_map.insert(make_pair((struct ic_label *)ic.result.second,current_basic_block));
            break;
        case 4:
            basic_blocks.push_back(current_basic_block);
            break;
        default:
            break;
    }
    new_basic_block_tag=0;
    
    //将中间代码加入当前基本块中
    if(!basic_block_has_return || ic.op==ic_op::END_FUNC_DEFINE)
    {
        current_basic_block->add_ic(ic);
    }
    if(ic.op==ic_op::RET)
    {
        basic_block_has_return=true;
    }
    
    if(func_end)
    {
        //如果该函数的流图构造完毕，那么就对各个基本块之间的跳转情况进行设置
        build_nexts_between_basic_blocks();
    }
}

//构建函数中的基本块之间的跳转关系
void ic_func_flow_graph::build_nexts_between_basic_blocks()
{
    struct ic_basic_block * pre_basic_block;
    bool need_set_sequential_next=false;
    for(auto basic_block:basic_blocks)
    {
        basic_block->precursors.clear();
        if(need_set_sequential_next)
        {
            pre_basic_block->set_sequential_next(basic_block);
        }
        switch(basic_block->ic_sequence.back().intermediate_code.op)
        {
            case ic_op::JMP:
                need_set_sequential_next=false;
                basic_block->set_sequential_next(nullptr);
                basic_block->set_jump_next(label_basic_block_map.at((struct ic_label *)(basic_block->ic_sequence.back().intermediate_code.result.second)));
                break;
            case ic_op::IF_JMP:
            case ic_op::IF_NOT_JMP:
                need_set_sequential_next=true;
                basic_block->set_jump_next(label_basic_block_map.at((struct ic_label *)(basic_block->ic_sequence.back().intermediate_code.result.second)));
                break;
            case ic_op::RET:
            case ic_op::END_FUNC_DEFINE:
                need_set_sequential_next=false;
                basic_block->set_sequential_next(nullptr);
                basic_block->set_jump_next(nullptr);
                break;
            default:
                need_set_sequential_next=true;
                basic_block->set_jump_next(nullptr);
                break;
        }
        pre_basic_block=basic_block;
    }
}

//清空函数中所有的数据流分析信息
void ic_func_flow_graph::clear_all_data_flow_analyse_info(bool further_analyse)
{
    array_to_array_member_map.clear();
    offset_to_array_member_map.clear();
    vars_def_positions.clear();
    arrays_def_positions.clear();
    vars_use_positions.clear();
    if(further_analyse)
    {
        for(auto loop:loops_info)
        {
            delete loop.second;
        }
        loops_info.clear();
        dominated_relations.clear();
    }
    for(auto basic_block:basic_blocks)
    {
        for(auto & ic_with_info:basic_block->ic_sequence)
        {
            ic_with_info.clear_all_data_flow_analyse_info();
        }
    }
}

//构建函数流图中的数组变量和数组元素之间的映射，以及偏移量和数组元素之间的映射
void ic_func_flow_graph::build_array_and_offset_to_array_member_map()
{
    for(auto basic_block:basic_blocks)
    {
        map_set_union_and_assign_to_arg1(array_to_array_member_map,basic_block->array_to_array_member_map);
        map_set_union_and_assign_to_arg1(offset_to_array_member_map,basic_block->offset_to_array_member_map);
    }
}

//构建函数流图中所有变量的定义点和使用点的信息
void ic_func_flow_graph::build_vars_def_and_use_pos_info()
{
    size_t pos;
    ic_pos current_pos;
    struct ic_data * belong_array,* offset,* tmp;
    for(auto basic_block:basic_blocks)
    {
        pos=0;
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            current_pos=ic_pos(basic_block,pos);
            tmp=((struct ic_data *)ic_with_info.intermediate_code.result.second);
            if(ic_with_info.intermediate_code.op==ic_op::VAR_DEFINE && tmp->is_array_var())
            {
                if(arrays_def_positions.find(tmp)==arrays_def_positions.end())
                {
                    arrays_def_positions.insert(make_pair(tmp,current_pos));
                }
                else
                {
                    arrays_def_positions.at(tmp)=current_pos;
                }
            }
            //明确定义点
            if(ic_with_info.explicit_def)
            {
                if(ic_with_info.explicit_def->is_array_var() && ic_with_info.explicit_def->is_array_member())
                {
                    map_set_insert(vars_def_positions,ic_with_info.explicit_def->get_belong_array(),current_pos);
                }
                else
                {
                    map_set_insert(vars_def_positions,ic_with_info.explicit_def,current_pos);
                }
                if(ic_with_info.explicit_def->is_array_var())
                {
                    //一个数组变量（包括即是数组取元素又是数组变量）的明确定义，意味着所有所属数组是该变量的数组元素也在这里被明确定义了
                    if(array_to_array_member_map.find(ic_with_info.explicit_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(ic_with_info.explicit_def))
                        {
                            map_set_insert(vars_def_positions,array_member,current_pos);
                        }
                    }
                }
                else if(ic_with_info.explicit_def->is_array_member())
                {
                    //一个数组元素的明确定义，有两种情况
                    //如果该数组元素的偏移量是变量的话，那么和该数组同属一个所属数组的所有数组元素都会在这里被模糊定义
                    //如果该数组元素的偏移量是常量的话，那么和该数组同属一个所属数组的，并且偏移量是变量的数组元素都会在这里被模糊定义，偏移量等于该数组元素的偏移量的数组元素都会在这里被明确定义
                    belong_array=ic_with_info.explicit_def->get_belong_array();
                    offset=ic_with_info.explicit_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==ic_with_info.explicit_def || (offset->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=offset->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(vars_def_positions,array_member,current_pos);
                        }
                    }
                }
            }
            //模糊定义点
            for(auto vague_def:ic_with_info.vague_defs)
            {
                if(vague_def->is_array_var() && vague_def->is_array_member())
                {
                    map_set_insert(vars_def_positions,vague_def->get_belong_array(),current_pos);
                }
                else
                {
                    map_set_insert(vars_def_positions,vague_def,current_pos);
                }
                if(vague_def->is_array_var())
                {
                    //一个数组变量的模糊定义同上的明确定义
                    if(array_to_array_member_map.find(vague_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(vague_def))
                        {
                            map_set_insert(vars_def_positions,array_member,current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    //一个数组取元素的模糊定义同上的明确定义
                    belong_array=vague_def->get_belong_array();
                    offset=vague_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (offset->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=offset->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(vars_def_positions,array_member,current_pos);
                        }
                    }
                }
            }
            //使用点
            for(auto use:ic_with_info.uses)
            {
                map_set_insert(vars_use_positions,use,current_pos);
            }
            pos++;
        }
    }
}

bool ic_func_flow_graph::is_dominated(struct ic_basic_block * bb1,struct ic_basic_block * bb2) const
{
    return map_set_find(dominated_relations,bb1,bb2);
}

//获取函数的出口个数
size_t ic_func_flow_graph::get_exit_num() const
{
    size_t res=0;
    bool tag=false;
    for(auto basic_block:basic_blocks)
    {
        for(vector<struct quaternion_with_info>::reverse_iterator ic_with_info=basic_block->ic_sequence.rbegin();ic_with_info!=basic_block->ic_sequence.rend();ic_with_info++)
        {
            if((*ic_with_info).intermediate_code.op==ic_op::RET)
            {
                res++;
                break;
            }
        }
    }
    for(vector<struct quaternion_with_info>::reverse_iterator ic_with_info=basic_blocks.back()->ic_sequence.rbegin();ic_with_info!=basic_blocks.back()->ic_sequence.rend();ic_with_info++)
    {
        if((*ic_with_info).intermediate_code.op==ic_op::RET)
        {
            tag=true;
            break;
        }
    }
    if(!tag)
    {
        res++;
    }
    return res;
}

//获取函数中的有效指令条数
size_t ic_func_flow_graph::get_effective_ic_instruction_num() const
{
    size_t res=0;
    for(auto bb:basic_blocks)
    {
        for(auto ic_with_info:bb->ic_sequence)
        {
            if(ic_with_info.intermediate_code.op!=ic_op::NOP)
            {
                res++;
            }
        }
    }
    return res;
}

//获取指定位置的中间代码及其信息
struct quaternion_with_info & ic_func_flow_graph::get_ic_with_info(ic_pos pos)
{
    return pos.basic_block->ic_sequence.at(pos.offset);
}

//==========================================================================//



//===================================== struct ic_flow_graph =====================================//

ic_flow_graph::ic_flow_graph(list<struct quaternion> * intermediate_codes)
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
}

ic_flow_graph::~ic_flow_graph()
{
    for(auto i:func_flow_graphs)
    {
        delete i;
    }
}

//找到某一个函数对应的函数流图
struct ic_func_flow_graph * ic_flow_graph::get_func_flow_graph(struct ic_func * func)
{
    for(auto func_flow_graph:func_flow_graphs)
    {
        if(func==func_flow_graph->func)
        {
            return func_flow_graph;
        }
    }
    return nullptr;
}

//==========================================================================//



//===================================== class Ic_optimizer =====================================//

Ic_optimizer::Ic_optimizer()
{

}

/*
初始化，设置优化选项
*/
void Ic_optimizer::init(bool optimize)
{
    need_optimize_=optimize;
}

/*
局部优化
*/
void Ic_optimizer::local_optimize()
{
    //先进行无用的函数返回值删除
    remove_useless_return();
    //窥孔优化
    if(need_optimize_)
    {
        for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
        {
            for(auto basic_block:func->basic_blocks)
            {
                peephole_optimization(basic_block);
            }
        }
    }
    //DAG优化
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            DAG_optimize(basic_block);
        }
    }
    //再进行函数内联
    if(need_optimize_)
    {
        for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
        {
            function_inline(intermediate_codes_flow_graph_,func);
            for(auto basic_block:func->basic_blocks)
            {
                DAG_optimize(basic_block);
            }
        }
    }
}

/*
数据流分析
*/
void Ic_optimizer::data_flow_analysis()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        Data_flow_analyzer::data_flow_analysis_for_a_func(func,true);
    }
}

/*
全局优化
*/
void Ic_optimizer::global_optimize()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        //全局复制传播
        global_copy_progagation(func);
        if(need_optimize_)
        {
            //全局公共子表达式删除
            global_elimination_of_common_subexpression(func);
        }
        //需要重新进行数据流分析
        //Data_flow_analyzer::data_flow_analysis_for_a_func(func,false);
        Data_flow_analyzer::data_flow_analysis_for_a_func(func,true);
        //全局死代码消除
        global_dead_code_elimination(func);
        if(need_optimize_)
        {
            //需要重新进行数据流分析
            //Data_flow_analyzer::data_flow_analysis_for_a_func(func,false);
            Data_flow_analyzer::data_flow_analysis_for_a_func(func,true);
            //循环不变量外提
            loop_invariant_computation_motion(func);
            //归纳变量删除
            induction_variable_elimination(func);
        }
        //重新构建循环的基本块跳转信息，因为在优化的时候可能会被改变
        func->build_nexts_between_basic_blocks();
    }
}

/*
联合优化
*/
void Ic_optimizer::union_optimize()
{
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        thread_optimize(intermediate_codes_flow_graph_,func);
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
    //局部优化
    local_optimize();
    //数据流分析
    data_flow_analysis();
    //全局优化
    global_optimize();
    //联合优化
    //union_optimize();
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
                outFile<<endl<<"============================================================================================================"<<endl;
                for(auto j:i->basic_blocks)
                {
                    outFile<<"-----------------------------------------BASIC_BLOCK:"<<(j)<<"-----------------------------------------"<<endl;
                    for(auto k:j->ic_sequence)
                    {
                        outFile<<(ic_outputs[k.intermediate_code.op](k.intermediate_code))<<endl;
                    }
                    if(j->jump_next && j->sequential_next)
                    {
                        outFile<<"-------------------------------------NEXT:"<<(j->sequential_next)<<","<<(j->jump_next)<<"-------------------------------------"<<endl;
                    }
                    else if(j->jump_next)
                    {
                        outFile<<"--------------------------------------------NEXT:"<<(j->jump_next)<<"---------------------------------------------"<<endl;
                    }
                    else if(j->sequential_next)
                    {
                        outFile<<"--------------------------------------------NEXT:"<<(j->sequential_next)<<"---------------------------------------------"<<endl;
                    }
                    else
                    {
                        outFile<<"------------------------------------------------------------------------------------------------------------"<<endl;
                    }
                }
                outFile<<"============================================================================================================"<<endl;
            }
            outFile.close();
        }
    }
    
    return res;
}

//==========================================================================//