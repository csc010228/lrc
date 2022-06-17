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
#include"data_flow_analyse.h"
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
    struct ic_data * arg1=nullptr,* arg2=nullptr,* result=nullptr;
    struct ic_func * func=nullptr;
    list<struct ic_data * > * r_params=nullptr;
    set<struct ic_data * > func_def_globals_and_f_params;
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
            func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_f_params(func);
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

//尝试将一个数据放入该中间代码的模糊定义数据
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

//尝试将一个数据放入该中间代码的使用数据
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

//尝试将一个ud-链的数据放入ud-链
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

//尝试将一个du-链的数据放入ud-链
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
    if(explicit_def && explicit_def->is_global() || (explicit_def->is_array_member() && (explicit_def->get_belong_array()->is_f_param() || explicit_def->get_belong_array()->is_global())) || (explicit_def->is_array_var() && explicit_def->is_f_param()))
    {
        return true;
    }
    for(auto vauge_def:vague_defs)
    {
        if(vauge_def && vauge_def->is_global() || (vauge_def->is_array_member() && (vauge_def->get_belong_array()->is_f_param() || vauge_def->get_belong_array()->is_global())) || (vauge_def->is_array_var() && vauge_def->is_f_param()))
        {
            return true;
        }
    }
    return false;
}

//将该条中间代码中使用的某一个数据替换成另一个常量数据
void quaternion_with_info::replace_used_data(struct ic_data * source,struct ic_data * destination)
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
                intermediate_code.arg1.second=symbol_table->array_member_not_array_var_entry(arg1->get_belong_array(),destination);
            }
            if(result->is_array_member() && result->get_offset()==source)
            {
                intermediate_code.result.second=symbol_table->array_member_not_array_var_entry(result->get_belong_array(),destination);
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
                intermediate_code.arg1.second=symbol_table->array_member_not_array_var_entry(arg1->get_belong_array(),destination);
            }
            if(arg2==source)
            {
                intermediate_code.arg2.second=(void *)destination;
            }
            else if(arg2->is_array_member() && arg2->get_offset()==source)
            {
                intermediate_code.arg2.second=symbol_table->array_member_not_array_var_entry(arg2->get_belong_array(),destination);
            }
            if(result->is_array_member() && result->get_offset()==source)
            {
                intermediate_code.result.second=symbol_table->array_member_not_array_var_entry(result->get_belong_array(),destination);
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
                intermediate_code.arg1.second=symbol_table->array_member_not_array_var_entry(arg1->get_belong_array(),destination);
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
                intermediate_code.result.second=symbol_table->array_member_not_array_var_entry(result->get_belong_array(),destination);
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
                    intermediate_code.result.second=symbol_table->array_member_not_array_var_entry(result->get_belong_array(),destination);
                }
            }
            break;
        default:
            break;
    }
    simplify();
}

//==========================================================================//



//===================================== struct ic_basic_block =====================================//

ic_basic_block::ic_basic_block(struct ic_func_flow_graph * belong_func_flow_graph):sequential_next(nullptr),jump_next(nullptr),belong_func_flow_graph(belong_func_flow_graph)
{
    
};

void ic_basic_block::set_sequential_next(struct ic_basic_block * next)
{
    sequential_next=next;
}

void ic_basic_block::set_jump_next(struct ic_basic_block * next)
{
    jump_next=next;
}

void ic_basic_block::add_ic(struct quaternion ic)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    static bool has_next_ic=true;
    struct quaternion_with_info ic_with_def_use_info(ic);
    set<struct ic_data * > all_datas;
    ic_sequence.push_back(ic_with_def_use_info);
    if(ic_with_def_use_info.explicit_def!=nullptr)
    {
        symbol_table->add_func_def_globals_and_f_params(belong_func_flow_graph->func,ic_with_def_use_info.explicit_def);
        all_datas.insert(ic_with_def_use_info.explicit_def);
    }
    for(auto vague_var:ic_with_def_use_info.vague_defs)
    {
        symbol_table->add_func_def_globals_and_f_params(belong_func_flow_graph->func,vague_var);
        all_datas.insert(vague_var);
    }
    for(auto use_var:ic_with_def_use_info.uses)
    {
        symbol_table->add_func_use_globals_and_f_params(belong_func_flow_graph->func,use_var);
        all_datas.insert(use_var);
    }
    if(ic.op==ic_op::CALL)
    {
        symbol_table->add_func_direct_calls(belong_func_flow_graph->func,(struct ic_func *)ic.arg1.second);
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

void ic_basic_block::clear_ic_sequence()
{
    ic_sequence.clear();
    array_to_array_member_map.clear();
    offset_to_array_member_map.clear();
}

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
        build_jumps_between_basic_blocks();
    }
}

//构建函数中的基本块之间的跳转关系
void ic_func_flow_graph::build_jumps_between_basic_blocks()
{
    struct ic_basic_block * pre_basic_block;
    bool need_set_sequential_next=false;
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

//构建函数流图中的数组变量和数组元素之间的映射，以及偏移量和数组元素之间的映射
void ic_func_flow_graph::build_array_and_offset_to_array_member_map()
{
    array_to_array_member_map.clear();
    offset_to_array_member_map.clear();
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
函数内联

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::function_inline(struct ic_basic_block * basic_block)
{
    struct ic_func * current_func,* called_func;
    set<struct ic_func * > funcs_called_directly_by_called_func;
    struct ic_func_flow_graph * called_func_flow_graph;
    for(auto ic_with_info:basic_block->ic_sequence)
    {
        if(ic_with_info.intermediate_code.op==ic_op::CALL)
        {
            current_func=basic_block->belong_func_flow_graph->func;
            called_func=(struct ic_func *)ic_with_info.intermediate_code.arg1.second;
            funcs_called_directly_by_called_func=Symbol_table::get_instance()->get_func_direct_calls(called_func);
            if(funcs_called_directly_by_called_func.find(called_func)==funcs_called_directly_by_called_func.end() && funcs_called_directly_by_called_func.find(current_func)==funcs_called_directly_by_called_func.end())
            {
                //只有当被调用的函数不是递归函数，而且也不会直接调用当前的函数的时候才能够进行函数内联
                called_func_flow_graph=intermediate_codes_flow_graph_->get_func_flow_graph(called_func);
                if(!called_func_flow_graph)
                {
                    return;
                }
                // TO-DO
            }
        }
    }
}

/*
DAG相关优化

Parameters
----------
basic_block:要优化的基本块
*/
void Ic_optimizer::DAG_optimize(struct ic_basic_block * basic_block)
{
    DAG * dag;
    list<struct quaternion> basic_block_ic_sequence;
    //根据基本块建立DAG
    //在建立DAG的过程中完成：
    //强度削弱
    //常量合并
    //复制传播
    //局部公共子表达式删除
    dag=new DAG(basic_block);
    //对建立完成的DAG进行优化
    dag->optimize();
    //将DAG重新转换成基本块的中间代码
    basic_block_ic_sequence=dag->to_basic_block();
    delete dag;
    basic_block->clear_ic_sequence();
    for(auto ic:basic_block_ic_sequence)
    {
        basic_block->add_ic(ic);
    }
    //同时查看基本块的跳转情况是否需要改变
    if(basic_block_ic_sequence.back().op==ic_op::JMP && basic_block->jump_next && basic_block->sequential_next)
    {
        basic_block->sequential_next=nullptr;
    }
}

/*
局部优化
*/
void Ic_optimizer::local_optimize()
{
    //先进行函数内联
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            function_inline(basic_block);
        }
    }
    //再进行DAG相关优化
    for(auto func:intermediate_codes_flow_graph_->func_flow_graphs)
    {
        for(auto basic_block:func->basic_blocks)
        {
            DAG_optimize(basic_block);
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
        //准备进行数据流分析
        prepare_before_data_flow_analyse(func);
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
    Symbol_table * symbol_table=Symbol_table::get_instance();
    struct quaternion intermediate_code;
    struct ic_data * arg1,* res;
    OAA tmp;
    bool tag;
    for(auto basic_block:func->basic_blocks)
    {
        for(vector<struct quaternion_with_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            tag=false;
            for(auto use:(*ic_with_info).uses)
            {
                if((*ic_with_info).ud_chain.find(use)!=(*ic_with_info).ud_chain.end() && (*ic_with_info).ud_chain.at(use).size()==1)
                {
                    intermediate_code=func->get_ic_with_info(*((*ic_with_info).ud_chain.at(use).begin())).intermediate_code;
                    arg1=((struct ic_data *)intermediate_code.arg1.second);
                    res=((struct ic_data *)intermediate_code.result.second);
                    if(intermediate_code.op==ic_op::ASSIGN && arg1->is_const() && res==use)
                    {
                        if(use->get_data_type()!=arg1->get_data_type())
                        {
                            tmp=arg1->get_value();
                            tmp.type_conversion(arg1->get_data_type(),use->get_data_type());
                            arg1=symbol_table->const_entry(use->get_data_type(),tmp);
                        }
                        (*ic_with_info).replace_used_data(use,arg1);
                        tag=true;
                    }
                }
            }
            if(tag)
            {
                (*ic_with_info).build_info(false);
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
    for(auto basic_block:func->basic_blocks)
    {
        for(vector<struct quaternion_with_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            if((*ic_with_info).intermediate_code.op!=ic_op::NOP && 
            (*ic_with_info).intermediate_code.op!=ic_op::JMP && 
            (*ic_with_info).intermediate_code.op!=ic_op::IF_JMP && 
            (*ic_with_info).intermediate_code.op!=ic_op::IF_NOT_JMP && 
            (*ic_with_info).intermediate_code.op!=ic_op::VAR_DEFINE && 
            (*ic_with_info).intermediate_code.op!=ic_op::LABEL_DEFINE && 
            (*ic_with_info).intermediate_code.op!=ic_op::FUNC_DEFINE && 
            (*ic_with_info).intermediate_code.op!=ic_op::END_FUNC_DEFINE && 
            (*ic_with_info).intermediate_code.op!=ic_op::CALL && 
            (*ic_with_info).intermediate_code.op!=ic_op::RET && 
            !(*ic_with_info).check_if_def_global_or_f_param_array() && 
            (*ic_with_info).du_chain.empty())
            {
                (*ic_with_info)=quaternion_with_info();
            }
        }
    }
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
    if(need_optimize_)
    {
        //局部优化
        local_optimize();
        //数据流分析
        data_flow_analysis();
        //全局优化
        global_optimize();
    }
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