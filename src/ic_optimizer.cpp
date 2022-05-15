/*
 *
 *  ic_optimizer.cpp
 *  中间代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/3/8
 *
*/
#include"ic_optimizer.h"
#include"dag.h"
#include"symbol_table.h"
#include<fstream>
#include<iostream>

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



//===================================== struct quaternion_with_def_use_info =====================================//

quaternion_with_def_use_info::quaternion_with_def_use_info():intermediate_code(quaternion()),explicit_def(nullptr)
{

}

quaternion_with_def_use_info::quaternion_with_def_use_info(struct quaternion ic):intermediate_code(ic)
{
    simplify();
    build_info();
}

//根据中间代码建立信息
void quaternion_with_def_use_info::build_info(bool clear_data_flow_analysis_info)
{
    struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
    struct ic_func * func=nullptr;
    list<struct ic_data * > * r_params=nullptr;
    set<struct ic_data * > func_def_global_vars_and_array_f_params;
    Symbol_table * symbol_table=Symbol_table::get_instance();
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
            func_def_global_vars_and_array_f_params=symbol_table->get_func_def_global_vars_and_array_f_params(func);
            for(auto vague_var:func_def_global_vars_and_array_f_params)
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
void quaternion_with_def_use_info::simplify()
{
    struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
    Symbol_table * symbol_table=Symbol_table::get_instance();
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

//尝试将一个数据放入该中间代码的明确定义数据
void quaternion_with_def_use_info::set_explicit_def(struct ic_data * data)
{
    if(!data->is_const())
    {
        if(data->is_array_member())
        {
            add_to_uses(data->get_belong_array());
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

//尝试将一个数据放入该中间代码的模糊定义数据
void quaternion_with_def_use_info:: add_to_vague_defs(struct ic_data * data)
{
    if(!data->is_const())
    {
        if(data->is_array_member())
        {
            add_to_uses(data->get_belong_array());
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

//尝试将一个数据放入该中间代码的使用数据
void quaternion_with_def_use_info::add_to_uses(struct ic_data * data)
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

//尝试将一个ud-链的数据放入ud-链
void quaternion_with_def_use_info::add_to_ud_chain(struct ic_data * data,set<ic_pos> poses)
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

//尝试将一个du-链的数据放入ud-链
void quaternion_with_def_use_info::add_to_du_chain(struct ic_data * data,set<ic_pos> poses)
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

//将该条中间代码中使用的某一个数据替换成另一个常量数据
void quaternion_with_def_use_info::replace_used_data(struct ic_data * source,struct ic_data * destination)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * arg1,* arg2,* result;
    list<struct ic_data * > * r_params,* new_r_params;
    switch(intermediate_code.op)
    {
        case ic_op::ASSIGN:
        case ic_op::NOT:
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            result=(struct ic_data *)intermediate_code.result.second;
            if(arg1==source)
            {
                intermediate_code.arg1.second=(void *)destination;
            }
            else if(arg1->is_array_member() && arg1->get_offset()==source)
            {
                intermediate_code.arg1.second=symbol_table->array_member_entry(arg1->get_belong_array(),arg1->get_belong_array()->dimensions_len->size(),destination);
            }
            if(result->is_array_member() && result->get_offset()==source)
            {
                intermediate_code.result.second=symbol_table->array_member_entry(result->get_belong_array(),result->get_belong_array()->dimensions_len->size(),destination);
            }
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
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            arg2=(struct ic_data *)intermediate_code.arg2.second;
            result=(struct ic_data *)intermediate_code.result.second;
            if(arg1==source)
            {
                intermediate_code.arg1.second=(void *)destination;
            }
            else if(arg1->is_array_member() && arg1->get_offset()==source)
            {
                intermediate_code.arg1.second=symbol_table->array_member_entry(arg1->get_belong_array(),arg1->get_belong_array()->dimensions_len->size(),destination);
            }
            if(arg2==source)
            {
                intermediate_code.arg2.second=(void *)destination;
            }
            else if(arg2->is_array_member() && arg2->get_offset()==source)
            {
                intermediate_code.arg2.second=symbol_table->array_member_entry(arg2->get_belong_array(),arg2->get_belong_array()->dimensions_len->size(),destination);
            }
            if(result->is_array_member() && result->get_offset()==source)
            {
                intermediate_code.result.second=symbol_table->array_member_entry(result->get_belong_array(),result->get_belong_array()->dimensions_len->size(),destination);
            }
            break;
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
            arg1=(struct ic_data *)intermediate_code.arg1.second;
            if(arg1==source)
            {
                intermediate_code.arg1.second=(void *)destination;
            }
            else if(arg1->is_array_member() && arg1->get_offset()==source)
            {
                intermediate_code.arg1.second=symbol_table->array_member_entry(arg1->get_belong_array(),arg1->get_belong_array()->dimensions_len->size(),destination);
            }
            break;
        case ic_op::CALL:
            r_params=(list<struct ic_data * > *)intermediate_code.arg2.second;
            result=(struct ic_data *)intermediate_code.result.second;
            new_r_params=new list<struct ic_data * >;
            for(auto r_param:*r_params)
            {
                if(r_param==source)
                {
                    new_r_params->push_back(destination);
                }
                else if(r_param->is_array_member() && r_param->get_offset()==source)
                {
                    new_r_params->push_back(symbol_table->array_member_entry(r_param->get_belong_array(),r_param->get_belong_array()->dimensions_len->size()-(r_param->dimensions_len==nullptr?0:r_param->dimensions_len->size()),destination));
                }
                else
                {
                    new_r_params->push_back(r_param);
                }
            }
            delete r_params;
            intermediate_code.arg2.second=(void *)new_r_params;
            if(result && result->is_array_member() && result->get_offset()==source)
            {
                intermediate_code.result.second=symbol_table->array_member_entry(result->get_belong_array(),result->get_belong_array()->dimensions_len->size(),destination);
            }
            break;
        case ic_op::RET:
            result=(struct ic_data *)intermediate_code.result.second;
            if(result)
            {
                if(result==source)
                {
                    intermediate_code.result.second=(void *)destination;
                }
                else if(result->is_array_member() && result->get_offset()==source)
                {
                    intermediate_code.result.second=symbol_table->array_member_entry(result->get_belong_array(),result->get_belong_array()->dimensions_len->size(),destination);
                }
            }
            break;
        default:
            break;
    }
    simplify();
    build_info(false);
}

//==========================================================================//



//===================================== struct ic_basic_block =====================================//

ic_basic_block::ic_basic_block(struct ic_func_flow_graph * belong_func_flow_graph):sequential_next(nullptr),jump_next(nullptr),belong_func_flow_graph(belong_func_flow_graph)
{
    
};

//设置该基本块顺序执行的时候的下一个基本块
void ic_basic_block::set_sequential_next(struct ic_basic_block * next)
{
    sequential_next=next;
}

//设置该基本块跳转执行的时候的下一个基本块
void ic_basic_block::set_jump_next(struct ic_basic_block * next)
{
    jump_next=next;
}

//往当前基本块中加入一条中间代码
void ic_basic_block::add_ic(struct quaternion ic)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    static bool has_next_ic=true;
    struct quaternion_with_def_use_info ic_with_def_use_info(ic);
    ic_sequence.push_back(ic_with_def_use_info);
    if(ic_with_def_use_info.explicit_def!=nullptr)
    {
        symbol_table->add_func_def_global_vars_and_array_f_params(belong_func_flow_graph->func,ic_with_def_use_info.explicit_def);
    }
    for(auto vague_var:ic_with_def_use_info.vague_defs)
    {
        symbol_table->add_func_def_global_vars_and_array_f_params(belong_func_flow_graph->func,vague_var);
    }
    for(auto use_var:ic_with_def_use_info.uses)
    {
        symbol_table->add_func_use_global_vars_and_array_f_params(belong_func_flow_graph->func,use_var);
    }
}

//获取一个基本块的所有前驱基本块
set<struct ic_basic_block * > ic_basic_block::get_precursors()
{
    set<struct ic_basic_block * > res;
    for(auto basic_block:belong_func_flow_graph->basic_blocks)
    {
        if(basic_block->sequential_next==this || basic_block->jump_next==this)
        {
            res.insert(basic_block);
        }
    }
    return res;
}

//获取一个基本块的所有后继基本块
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
    static map<struct ic_label *,struct ic_basic_block *> ic_label_basic_block_map;
    static struct ic_basic_block * current_basic_block=nullptr;
    static bool previous_ic_is_jump=false,basic_block_has_return=false;
    unsigned char new_basic_block_tag=0;
    bool func_end=false,need_set_sequential_next;
    struct ic_basic_block * pre_basic_block;
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
            ic_label_basic_block_map.insert(make_pair((struct ic_label *)ic.result.second,current_basic_block));
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
        need_set_sequential_next=false;
        for(auto basic_block:basic_blocks)
        {
            if(need_set_sequential_next)
            {
                pre_basic_block->set_sequential_next(basic_block);
            }
            switch(basic_block->ic_sequence.back().intermediate_code.op)
            {
                case ic_op::JMP:
                    need_set_sequential_next=false;
                    basic_block->set_sequential_next(nullptr);
                    basic_block->set_jump_next(ic_label_basic_block_map.at((struct ic_label *)(basic_block->ic_sequence.back().intermediate_code.result.second)));
                    break;
                case ic_op::IF_JMP:
                case ic_op::IF_NOT_JMP:
                    need_set_sequential_next=true;
                    basic_block->set_jump_next(ic_label_basic_block_map.at((struct ic_label *)(basic_block->ic_sequence.back().intermediate_code.result.second)));
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
}

//获取指定位置的中间代码及其信息
struct quaternion_with_def_use_info ic_func_flow_graph::get_ic_with_info(ic_pos pos)
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

//==========================================================================//



//===================================== class Ic_optimizer =====================================//

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

}

/*
常量合并

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::constant_folding(struct ic_basic_block * basic_block)
{
    
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

map<struct ic_data *,set<ic_pos> > map_set_union(map<struct ic_data *,set<ic_pos> > arg1,map<struct ic_data *,set<ic_pos> > arg2)
{
    map<struct ic_data *,set<ic_pos> > res=arg1;
    for(auto map_member:arg2)
    {
        if(res.find(map_member.first)==res.end())
        {
            res.insert(make_pair(map_member.first,set<ic_pos>()));
        }
        set_union(map_member.second.begin(),map_member.second.end(),res.at(map_member.first).begin(),res.at(map_member.first).end(),inserter(res.at(map_member.first),res.at(map_member.first).begin()));
    }
    return res;
}

map<struct ic_data *,set<ic_pos> > map_set_difference(map<struct ic_data *,set<ic_pos> > arg1,map<struct ic_data *,set<ic_pos> > arg2)
{
    map<struct ic_data *,set<ic_pos> > res=arg1;
    for(auto map_member:arg2)
    {
        if(res.find(map_member.first)!=res.end())
        {
            set_difference(map_member.second.begin(),map_member.second.end(),res.at(map_member.first).begin(),res.at(map_member.first).end(),inserter(res.at(map_member.first),res.at(map_member.first).begin()));
        }
    }
    return res;
}

/*
到达-定义分析

Parameters
----------
func:要分析的函数流图
*/
void Ic_optimizer::use_define_analysis(struct ic_func_flow_graph * func)
{
    map<struct ic_data *,set<ic_pos> > all_defs;
    map<struct ic_data *,set<struct ic_data *> > array_to_array_member_map,offset_to_array_member_map;
    map<struct ic_basic_block *,map<struct ic_data *,set<ic_pos> > > gens,kills;
    set<struct ic_basic_block * > precursors;
    map<struct ic_data *,set<ic_pos> > oldout;
    struct ic_data * belong_array;
    size_t pos;
    ic_pos current_pos;
    bool change=true;
    //遍历函数，获取所有的数组元素信息
    for(auto basic_block:func->basic_blocks)
    {
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            if((*ic_with_info).explicit_def)
            {
                if((*ic_with_info).explicit_def->is_array_member())
                {
                    if(array_to_array_member_map.find((*ic_with_info).explicit_def->get_belong_array())==array_to_array_member_map.end())
                    {
                        array_to_array_member_map.insert(make_pair((*ic_with_info).explicit_def->get_belong_array(),set<struct ic_data *>()));
                    }
                    array_to_array_member_map.at((*ic_with_info).explicit_def->get_belong_array()).insert((*ic_with_info).explicit_def);
                    if(offset_to_array_member_map.find((*ic_with_info).explicit_def->get_offset())==offset_to_array_member_map.end())
                    {
                        offset_to_array_member_map.insert(make_pair((*ic_with_info).explicit_def->get_offset(),set<struct ic_data *>()));
                    }
                    offset_to_array_member_map.at((*ic_with_info).explicit_def->get_offset()).insert((*ic_with_info).explicit_def);
                }
            }
            for(auto vague_def:(*ic_with_info).vague_defs)
            {
                if(vague_def->is_array_member())
                {
                    if(array_to_array_member_map.find(vague_def->get_belong_array())==array_to_array_member_map.end())
                    {
                        array_to_array_member_map.insert(make_pair(vague_def->get_belong_array(),set<struct ic_data *>()));
                    }
                    array_to_array_member_map.at(vague_def->get_belong_array()).insert(vague_def);
                    if(offset_to_array_member_map.find(vague_def->get_offset())==offset_to_array_member_map.end())
                    {
                        offset_to_array_member_map.insert(make_pair(vague_def->get_offset(),set<struct ic_data *>()));
                    }
                    offset_to_array_member_map.at(vague_def->get_offset()).insert(vague_def);
                }
            }
        }
    }
    //遍历函数，获取该函数的所有变量定义点，包括明确定义和模糊定义
    for(auto basic_block:func->basic_blocks)
    {
        pos=0;
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            current_pos=ic_pos(basic_block,pos);
            if((*ic_with_info).explicit_def)
            {
                if(all_defs.find((*ic_with_info).explicit_def)==all_defs.end())
                {
                    all_defs.insert(make_pair((*ic_with_info).explicit_def,set<ic_pos>()));
                }
                all_defs.at((*ic_with_info).explicit_def).insert(current_pos);
                if((*ic_with_info).explicit_def->is_array_var())
                {
                    if(array_to_array_member_map.find((*ic_with_info).explicit_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at((*ic_with_info).explicit_def))
                        {
                            if(all_defs.find(array_member)==all_defs.end())
                            {
                                all_defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            all_defs.at(array_member).insert(current_pos);
                        }
                    }
                }
                else if((*ic_with_info).explicit_def->is_array_member())
                {
                    belong_array=(*ic_with_info).explicit_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==(*ic_with_info).explicit_def || ((*ic_with_info).explicit_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=(*ic_with_info).explicit_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(all_defs.find(array_member)==all_defs.end())
                            {
                                all_defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            all_defs.at(array_member).insert(current_pos);
                        }
                    }
                }
            }
            for(auto vague_def:(*ic_with_info).vague_defs)
            {
                if(all_defs.find(vague_def)==all_defs.end())
                {
                    all_defs.insert(make_pair(vague_def,set<ic_pos>()));
                }
                all_defs.at(vague_def).insert(current_pos);
                if(vague_def->is_array_var())
                {
                    if(array_to_array_member_map.find(vague_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(vague_def))
                        {
                            if(all_defs.find(array_member)==all_defs.end())
                            {
                                all_defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            all_defs.at(array_member).insert(current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    belong_array=vague_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (vague_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=vague_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(all_defs.find(array_member)==all_defs.end())
                            {
                                all_defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            all_defs.at(array_member).insert(current_pos);
                        }
                    }
                }
            }
            pos++;
        }
    }
    //构造每一个基本块的gen和kill
    for(auto basic_block:func->basic_blocks)
    {
        gens.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        kills.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        pos=0;
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            current_pos=ic_pos(basic_block,pos);
            if((*ic_with_info).explicit_def)
            {
                //一个变量的明确定义会生成（gen）该变量的一次定义，同时会杀死（kill）该变量的其他所有的定义
                if(gens.at(basic_block).find((*ic_with_info).explicit_def)==gens.at(basic_block).end())
                {
                    gens.at(basic_block).insert(make_pair((*ic_with_info).explicit_def,set<ic_pos>()));
                }
                else
                {
                    gens.at(basic_block).at((*ic_with_info).explicit_def).clear();
                }
                gens.at(basic_block).at((*ic_with_info).explicit_def).insert(current_pos);
                if(kills.at(basic_block).find((*ic_with_info).explicit_def)==kills.at(basic_block).end())
                {
                    kills.at(basic_block).insert(make_pair((*ic_with_info).explicit_def,set<ic_pos>()));
                }
                set_union(all_defs.at((*ic_with_info).explicit_def).begin(),all_defs.at((*ic_with_info).explicit_def).end(),kills.at(basic_block).at((*ic_with_info).explicit_def).begin(),kills.at(basic_block).at((*ic_with_info).explicit_def).end(),inserter(kills.at(basic_block).at((*ic_with_info).explicit_def),kills.at(basic_block).at((*ic_with_info).explicit_def).begin()));
                kills.at(basic_block).at((*ic_with_info).explicit_def).erase(current_pos);
                //一个变量的明确定义会杀死（kill）所有以该变量为偏移量的数组取元素变量的定义
                if(offset_to_array_member_map.find((*ic_with_info).explicit_def)!=offset_to_array_member_map.end())
                {
                    for(auto array_member:offset_to_array_member_map.at((*ic_with_info).explicit_def))
                    {
                        if(kills.at(basic_block).find(array_member)==kills.at(basic_block).end())
                        {
                            kills.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                        }
                        set_union(all_defs.at(array_member).begin(),all_defs.at(array_member).end(),kills.at(basic_block).at(array_member).begin(),kills.at(basic_block).at(array_member).end(),inserter(kills.at(basic_block).at(array_member),kills.at(basic_block).at(array_member).begin()));
                        if(gens.at(basic_block).find(array_member)!=gens.at(basic_block).end())
                        {
                            gens.at(basic_block).erase(array_member);
                        }
                    }
                }
                //当明确定义的变量是数组或者数组元素的时候，需要进行额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if((*ic_with_info).explicit_def->is_array_var())
                {
                    if(array_to_array_member_map.find((*ic_with_info).explicit_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at((*ic_with_info).explicit_def))
                        {
                            //明确定义的数组会杀死（kill）所有的该数组的数组取元素的定义
                            if(kills.at(basic_block).find(array_member)==kills.at(basic_block).end())
                            {
                                kills.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            set_union(all_defs.at(array_member).begin(),all_defs.at(array_member).end(),kills.at(basic_block).at(array_member).begin(),kills.at(basic_block).at(array_member).end(),inserter(kills.at(basic_block).at(array_member),kills.at(basic_block).at(array_member).begin()));
                            if(gens.at(basic_block).find(array_member)!=gens.at(basic_block).end())
                            {
                                gens.at(basic_block).erase(array_member);
                            }
                            //同时也会在该点生成（gen）所有的该数组的数组取元素的定义
                            if(gens.at(basic_block).find(array_member)==gens.at(basic_block).end())
                            {
                                gens.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            gens.at(basic_block).at(array_member).insert(current_pos);
                        }
                    }
                }
                else if((*ic_with_info).explicit_def->is_array_member())
                {
                    belong_array=(*ic_with_info).explicit_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        //如果明确定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义
                        //如果明确定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==(*ic_with_info).explicit_def || ((*ic_with_info).explicit_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=(*ic_with_info).explicit_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(gens.at(basic_block).find(array_member)==gens.at(basic_block).end())
                            {
                                gens.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            gens.at(basic_block).at(array_member).insert(current_pos);
                        }
                    }
                }
            }
            for(auto vague_def:(*ic_with_info).vague_defs)
            {
                if(gens.at(basic_block).find(vague_def)==gens.at(basic_block).end())
                {
                    gens.at(basic_block).insert(make_pair(vague_def,set<ic_pos>()));
                }
                gens.at(basic_block).at(vague_def).insert(current_pos);
                //当模糊定义的变量是数组或者数组取元素的时候，需要进行额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if(vague_def->is_array_var())
                {
                    //模糊定义的数组会生成（gen）所有的该数组的数组取元素的定义
                    if(array_to_array_member_map.find(vague_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(vague_def))
                        {
                            if(gens.at(basic_block).find(array_member)==gens.at(basic_block).end())
                            {
                                gens.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            gens.at(basic_block).at(array_member).insert(current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    belong_array=vague_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        //如果模糊定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义
                        //如果模糊定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (vague_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=vague_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(gens.at(basic_block).find(array_member)==gens.at(basic_block).end())
                            {
                                gens.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            gens.at(basic_block).at(array_member).insert(current_pos);
                        }
                    }
                }
            }
            pos++;
        }
    }
    //最后开始迭代计算每一个基本块的到达-定义信息
    for(auto basic_block:func->basic_blocks)
    {
        basic_block->use_def_analysis_info.in.clear();
        basic_block->use_def_analysis_info.out=gens.at(basic_block);
    }
    while(change)
    {
        change=false;
        for(auto basic_block:func->basic_blocks)
        {
            precursors=basic_block->get_precursors();
            basic_block->use_def_analysis_info.in.clear();
            for(auto precursor:precursors)
            {
                basic_block->use_def_analysis_info.in=map_set_union(basic_block->use_def_analysis_info.in,precursor->use_def_analysis_info.out);
            }
            oldout=basic_block->use_def_analysis_info.out;
            basic_block->use_def_analysis_info.out=map_set_union(gens.at(basic_block),map_set_difference(basic_block->use_def_analysis_info.in,kills.at(basic_block)));
            if(oldout!=basic_block->use_def_analysis_info.out)
            {
                change=true;
            }
        }
    }
}

/*
构建ud-链

Parameters
----------
func:已经完成了到达-定义分析的，要构建ud-链的函数流图
*/
void Ic_optimizer::build_ud_chain(struct ic_func_flow_graph * func)
{
    map<struct ic_data *,set<struct ic_data * > > array_to_array_member_map,offset_to_array_member_map;
    map<struct ic_data *,set<ic_pos> > defs;
    size_t pos;
    ic_pos current_pos;
    struct ic_data * belong_array;
    for(auto basic_block:func->basic_blocks)
    {
        pos=0;
        defs=basic_block->use_def_analysis_info.in;
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            current_pos=ic_pos(basic_block,pos);
            for(auto use:(*ic_with_info).uses)
            {
                // if(use->is_array_var())
                // {
                //     continue;
                // }
                if(defs.find(use)!=defs.end())
                {
                    (*ic_with_info).add_to_ud_chain(use,defs.at(use));
                }
            }
            if((*ic_with_info).explicit_def)
            {
                //如果本条语句对某一个变量进行了明确定义，那么它会杀死（kill）该基本块中之前的所有对该变量的明确定义和模糊定义，同时在该点生成（gen）对该变量的定义
                if(defs.find((*ic_with_info).explicit_def)==defs.end())
                {
                    defs.insert(make_pair((*ic_with_info).explicit_def,set<ic_pos>()));
                }
                else
                {
                    defs.at((*ic_with_info).explicit_def).clear();
                }
                defs.at((*ic_with_info).explicit_def).insert(current_pos);
                //明确定义也会杀死（kill）所有以该变量作为偏移的数组取元素的定义
                if(offset_to_array_member_map.find((*ic_with_info).explicit_def)!=offset_to_array_member_map.end())
                {
                    for(auto array_member:offset_to_array_member_map.at((*ic_with_info).explicit_def))
                    {
                        if(defs.find(array_member)!=defs.end())
                        {
                            defs.erase(array_member);
                        }
                    }
                }
                //如果明确定义是数组或者数组取元素，那么需要做额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if((*ic_with_info).explicit_def->is_array_var())
                {
                    //如果明确定义是数组，那么就会杀死之前所有的该数组取元素的定义，然后再在该点生成（gen）所有的该数组取元素的定义
                    if(array_to_array_member_map.find((*ic_with_info).explicit_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at((*ic_with_info).explicit_def))
                        {
                            if(defs.find(array_member)==defs.end())
                            {
                                defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            else
                            {
                                defs.at(array_member).clear();
                            }
                            defs.at(array_member).insert(current_pos);
                        }
                    }
                }
                else if((*ic_with_info).explicit_def->is_array_member())
                {
                    //如果明确定义是数组取元素，那么就判断其偏移量是否是常量
                    belong_array=(*ic_with_info).explicit_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        //如果明确定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义
                        //如果明确定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==(*ic_with_info).explicit_def || ((*ic_with_info).explicit_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=(*ic_with_info).explicit_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(defs.find(array_member)==defs.end())
                            {
                                defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            defs.at(array_member).insert(current_pos);
                        }
                    }
                    //然后将数组取元素的信息记录下来
                    if(array_to_array_member_map.find((*ic_with_info).explicit_def->get_belong_array())==array_to_array_member_map.end())
                    {
                        array_to_array_member_map.insert(make_pair((*ic_with_info).explicit_def->get_belong_array(),set<struct ic_data * >()));
                    }
                    array_to_array_member_map.at((*ic_with_info).explicit_def->get_belong_array()).insert((*ic_with_info).explicit_def);
                    if(offset_to_array_member_map.find((*ic_with_info).explicit_def->get_offset())==offset_to_array_member_map.end())
                    {
                        offset_to_array_member_map.insert(make_pair((*ic_with_info).explicit_def->get_offset(),set<struct ic_data * >()));
                    }
                    offset_to_array_member_map.at((*ic_with_info).explicit_def->get_offset()).insert((*ic_with_info).explicit_def);
                }
            }
            for(auto vague_def:(*ic_with_info).vague_defs)
            {
                //如果本条语句对某一个变量进行了模糊定义，那么它会在该点生成（gen）对该变量的定义
                if(defs.find(vague_def)==defs.end())
                {
                    defs.insert(make_pair(vague_def,set<ic_pos>()));
                }
                defs.at(vague_def).insert(current_pos);
                //如果模糊定义是数组或者数组取元素，那么需要做额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if(vague_def->is_array_var())
                {
                    //如果模糊定义是数组，那么就会在该点生成（gen）所有的该数组取元素的定义
                    if(array_to_array_member_map.find(vague_def)!=array_to_array_member_map.end())
                    {
                        for(auto array_member:array_to_array_member_map.at(vague_def))
                        {
                            if(defs.find(array_member)==defs.end())
                            {
                                defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            defs.at(array_member).insert(current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    //如果模糊定义的是数组取元素，那么就判断其偏移量是否是常量
                    belong_array=vague_def->get_belong_array();
                    if(array_to_array_member_map.find(belong_array)!=array_to_array_member_map.end())
                    {
                        //如果模糊定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义
                        //如果模糊定义的数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (vague_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=vague_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            if(defs.find(array_member)==defs.end())
                            {
                                defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            defs.at(array_member).insert(current_pos);
                        }
                    }
                    //然后将数组取元素的信息记录下来
                    if(array_to_array_member_map.find(vague_def->get_belong_array())==array_to_array_member_map.end())
                    {
                        array_to_array_member_map.insert(make_pair(vague_def->get_belong_array(),set<struct ic_data * >()));
                    }
                    array_to_array_member_map.at(vague_def->get_belong_array()).insert(vague_def);
                    if(offset_to_array_member_map.find(vague_def->get_offset())==offset_to_array_member_map.end())
                    {
                        offset_to_array_member_map.insert(make_pair(vague_def->get_offset(),set<struct ic_data * >()));
                    }
                    offset_to_array_member_map.at(vague_def->get_offset()).insert(vague_def);
                }
            }
            pos++;
        }
    }
}

/*
活跃变量分析

Parameters
----------
func:要分析的函数流图
*/
void Ic_optimizer::live_variable_analysis(struct ic_func_flow_graph * func)
{
    map<struct ic_basic_block *,set<struct ic_data * > > uses,defs;
    set<struct ic_data * > show_up,oldin;
    set<struct ic_basic_block * > successors;
    bool change=true;
    //首先计算出每一个基本块的use和def
    //目前没有考虑数组元素
    for(auto basic_block:func->basic_blocks)
    {
        uses.insert(make_pair(basic_block,set<struct ic_data * >()));
        defs.insert(make_pair(basic_block,set<struct ic_data * >()));
        show_up.clear();
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            for(auto use:(*ic_with_info).uses)
            {
                if(show_up.find(use)==show_up.end())
                {
                    uses.at(basic_block).insert(use);
                    show_up.insert(use);
                }
            }
            if(show_up.find((*ic_with_info).explicit_def)==show_up.end())
            {
                defs.at(basic_block).insert((*ic_with_info).explicit_def);
                show_up.insert((*ic_with_info).explicit_def);
            }
        }
    }
    //然后进行迭代计算，算出活跃变量的信息
    for(auto basic_block:func->basic_blocks)
    {
        basic_block->live_analysis_info.in.clear();
    }
    while(change)
    {
        change=false;
        for(auto basic_block:func->basic_blocks)
        {
            successors=basic_block->get_successors();
            for(auto successor:successors)
            {
                set_union(basic_block->live_analysis_info.out.begin(),basic_block->live_analysis_info.out.end(),successor->live_analysis_info.in.begin(),successor->live_analysis_info.in.end(),inserter(basic_block->live_analysis_info.out,basic_block->live_analysis_info.out.begin()));
            }
            oldin=basic_block->live_analysis_info.in;
            set_difference(basic_block->live_analysis_info.out.begin(),basic_block->live_analysis_info.out.end(),defs.at(basic_block).begin(),defs.at(basic_block).end(),inserter(basic_block->live_analysis_info.in,basic_block->live_analysis_info.in.begin()));
            set_union(basic_block->live_analysis_info.in.begin(),basic_block->live_analysis_info.in.end(),uses.at(basic_block).begin(),uses.at(basic_block).end(),inserter(basic_block->live_analysis_info.in,basic_block->live_analysis_info.in.begin()));
            if(oldin!=basic_block->live_analysis_info.in)
            {
                change=true;
            }
        }
    }
}

/*
构建du-链

Parameters
----------
func:已经完成了活跃变量分析的，要构建du-链的函数流图
*/
void Ic_optimizer::build_du_chain(struct ic_func_flow_graph * func)
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
        //构建ud-链
        build_ud_chain(func);
        //活跃变量分析
        live_variable_analysis(func);
        //构建du-链
        build_du_chain(func);
        //可用表达式分析
        available_expression_analysis(func);
    }
}

/*
全局常量合并

Parameters
----------
func:要优化的函数流图
*/
void Ic_optimizer::globale_constant_folding(struct ic_func_flow_graph * func)
{
    struct quaternion_with_def_use_info pre_ic_with_info;
    for(auto basic_block:func->basic_blocks)
    {
        for(vector<struct quaternion_with_def_use_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            for(auto ud_chain_node:(*ic_with_info).ud_chain)
            {
                if(ud_chain_node.second.size()==1)
                {
                    for(auto pos:ud_chain_node.second)
                    {
                        pre_ic_with_info=ic_func_flow_graph::get_ic_with_info(pos);
                        if(pre_ic_with_info.intermediate_code.op==ic_op::ASSIGN && pre_ic_with_info.intermediate_code.result.second==ud_chain_node.first && ((struct ic_data *)pre_ic_with_info.intermediate_code.arg1.second)->is_const())
                        {
                            (*ic_with_info).replace_used_data(ud_chain_node.first,(struct ic_data *)pre_ic_with_info.intermediate_code.arg1.second);
                        }
                    }
                }
            }
        }
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
        //全局常量合并
        globale_constant_folding(func);
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
                outFile<<endl<<endl<<endl;
                for(auto j:i->basic_blocks)
                {
                    outFile<<"=========================================BASIC_BLOCK:"<<(j)<<"========================================="<<endl;
                    for(auto k:j->ic_sequence)
                    {
                        outFile<<(ic_outputs[k.intermediate_code.op](k.intermediate_code))<<endl;
                    }
                    if(j->jump_next && j->sequential_next)
                    {
                        outFile<<"======================================NEXT:"<<(j->sequential_next)<<","<<(j->jump_next)<<"======================================"<<endl;
                    }
                    else if(j->jump_next)
                    {
                        outFile<<"============================================NEXT:"<<(j->jump_next)<<"============================================"<<endl;
                    }
                    else if(j->sequential_next)
                    {
                        outFile<<"============================================NEXT:"<<(j->sequential_next)<<"============================================"<<endl;
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

//==========================================================================//