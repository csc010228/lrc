/*
 *
 *  semantic_rules.cpp
 *  语义动作
 *  Creator:Sichao Chen
 *  Create time:2021/11/04
 *
*/
#include "semantic_rules.h"
#include<iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsequenced"

//在中间代码中输出的临时变量编号前缀
#define TMP_PREFIX "@"

//作用域输出信息
map<enum ic_scope_type,string> scope_type_output_map={
    {ic_scope_type::GLOBAL,""},
    {ic_scope_type::FUNC,""},
    {ic_scope_type::IF,"if"},
    {ic_scope_type::ELSE,"else"},
    {ic_scope_type::WHILE,"while"},
    {ic_scope_type::ANONYMOUS,""},
    {ic_scope_type::INLINE_FUNC,"<inline>"},
};

//变量类型的输出信息
map<enum language_data_type,string> identifier_type_output_map={
    {language_data_type::VOID,"void"},
    {language_data_type::CHAR,"char"},
    {language_data_type::INT,"int"},
    {language_data_type::FLOAT,"float"},
    {language_data_type::LONG,"long"},
    {language_data_type::DOUBLE,"double"},
    {language_data_type::POINTER,"pointer"}
};

//中间代码的输出信息
map<ic_op,string> ic_op_output_map={
    {ic_op::ASSIGN,"ASSIGN"},              //赋值
    {ic_op::ADD,"ADD"},                    //加
    {ic_op::SUB,"SUB"},                    //减
    {ic_op::MUL,"MUL"},                    //乘
    {ic_op::DIV,"DIV"},                    //除
    {ic_op::MOD,"MOD"},                    //取模
    // {ic_op::L_SHIFT,"L_SHIFT"},            //左移位
    // {ic_op::R_SHIFT,"R_SHIFT"},            //右移位
    {ic_op::NOT,"NOT"},                    //非
    // {ic_op::AND,"AND"},                    //与
    // {ic_op::OR,"OR"},                      //或
    // {ic_op::XOR,"XOR"},                    //异或
    {ic_op::EQ,"EQ"},                      //是否相等
    {ic_op::UEQ,"UEQ"},                    //是否不相等
    {ic_op::GT,"GT"},                      //比较是否大于
    {ic_op::LT,"LT"},                      //比较是否小于
    {ic_op::GE,"GE"},                      //比较是否大于等于
    {ic_op::LE,"LE"},                      //比较是否小于等于
    {ic_op::JMP,"JMP"},                    //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
    {ic_op::IF_JMP,"IF_JMP"},              //结果为true的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
    {ic_op::IF_NOT_JMP,"IF_NOT_JMP"},      //结果为false的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
    {ic_op::VAR_DEFINE,"VAR_DEFINE"},      //变量定义（可以赋初始值，也可以不赋初始值）
    {ic_op::FUNC_DEFINE,"FUNC_DEFINE"},    //函数定义（结果是指向要定义的函数在符号表中的表项的指针）
    {ic_op::END_FUNC_DEFINE,"END_FUNC_DEFINE"},    //结束函数定义（结果是指向要结束定义的函数在符号表中的表项的指针）
    {ic_op::LABEL_DEFINE,"LABEL_DEFINE"},  //定义一个LABEL
    {ic_op::CALL,"CALL"},                  //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
    {ic_op::RET,"RET"},                    //函数返回（只有结果，没有操作数，结果就是返回值）
};

//打印中间代码的操作符
string output_ic_op(ic_op op)
{
    return ic_op_output_map[op];
}

//打印中间代码中的常数类型的操作数
string output_ic_const_operand(enum language_data_type type,OAA const_value)
{
    string res;
    switch(type)
    {
        case language_data_type::INT:
            res=to_string(const_value.int_data);
            break;
        case language_data_type::FLOAT:
            res=to_string(const_value.float_data);
            break;
        default:
            break;
    }
    return res;
}

//打印中间代码中的标签类型的操作数
string output_ic_label_operand(struct ic_label * label)
{
    return label->name;
}

//打印中间代码中的作用域
string output_ic_scope(struct ic_scope * scope)
{
    string res="";
    struct ic_scope * tmp_scope=scope;
    while(!tmp_scope->is_global())
    {
        if(tmp_scope->is_func())
        {
            res=(tmp_scope->func->name+"::"+res);
        }
        else if(tmp_scope->is_inline_func())
        {
            res=(scope_type_output_map.at(tmp_scope->type)+tmp_scope->func->name+"-"+to_string(tmp_scope->index_for_output)+"::"+res);
        }
        else
        {
            res=(scope_type_output_map.at(tmp_scope->type)+"_"+to_string(tmp_scope->index_for_output)+"::"+res);
        }
        tmp_scope=tmp_scope->get_father();
    }
    return res;
}

//打印中间代码中的变量类型的操作数
string output_ic_var_operand(struct ic_data * ic_data,bool print_details,bool print_init_value,bool is_define=false)
{
    string res,tmp;
    stack<size_t> array_init_value_output_stack;
    vector<OAA> * array_const_init_value;
    size_t array_init_value_elements_outputed=0;
    vector<struct ic_data * > * array_var_init_value;
    list<struct ic_data * > * dimensions_len;
    list<struct ic_data * >::iterator array_init_value_output_pointer;
    if(ic_data->is_tmp_var())
    {
        if(is_define)
        {
            res+=("("+identifier_type_output_map.at(ic_data->get_data_type())+")");
        }
        res+=TMP_PREFIX+to_string(ic_data->tmp_index);
    }
    else if(ic_data->is_pure_const())
    {
        res=output_ic_const_operand(ic_data->data_type,ic_data->get_value());
    }
    else if(ic_data->is_array_member())
    {
        res=output_ic_var_operand(((pair<struct ic_data *,struct ic_data * > *)ic_data->get_value().pointer_data)->first,false,false)+"["+output_ic_var_operand(((pair<struct ic_data *,struct ic_data * > * )ic_data->get_value().pointer_data)->second,false,false)+"]";
    }
    else
    {
        res="";
        if(print_details)
        {
            if(ic_data->is_const_var())
            {
                res+="const ";
            }
            res+=identifier_type_output_map[ic_data->data_type]+" "+output_ic_scope(ic_data->get_scope())+ic_data->get_var_name();
            if(ic_data->dimensions_len)
            {
                for(list<struct ic_data * >::iterator i=ic_data->dimensions_len->begin();i!=ic_data->dimensions_len->end();i++)
                {
                    res+="["+output_ic_var_operand(*i,false,false)+"]";
                }
            }
        }
        else
        {
            res+=(output_ic_scope(ic_data->get_scope())+ic_data->get_var_name());
        }
        if(print_init_value)
        {
            if(ic_data->is_array_var())
            {
                dimensions_len=ic_data->dimensions_len;
                if(ic_data->is_const_var() || ic_data->is_global())
                {
                    if(ic_data->get_value().pointer_data)
                    {
                        res+=" = { ";
                        array_init_value_output_pointer=dimensions_len->begin();
                        array_const_init_value=(vector<OAA> *)ic_data->get_value().pointer_data;
                        array_init_value_output_stack.push((size_t)((*(array_init_value_output_pointer++))->get_value().int_data));
                        while(!array_init_value_output_stack.empty())
                        {
                            while(array_init_value_output_pointer!=dimensions_len->end())
                            {
                                res+="{ ";
                                array_init_value_output_stack.push((size_t)((*(array_init_value_output_pointer++))->get_value().int_data));
                            }
                            for(size_t i=array_init_value_output_stack.top();i!=0;i--)
                            {
                                res+=(output_ic_const_operand(language_data_type::INT,array_const_init_value->at(array_init_value_elements_outputed++))+" , ");
                            }
                            array_init_value_output_stack.top()=1;
                            while((--array_init_value_output_stack.top())==0)
                            {
                                array_init_value_output_stack.pop();
                                array_init_value_output_pointer--;
                                res=res.substr(0,res.size()-3);
                                res+=" } , ";
                                if(array_init_value_output_stack.empty())
                                {
                                    break;
                                }
                            }
                        }
                        res=res.substr(0,res.size()-3);
                    }
                }
                else
                {
                    if(ic_data->get_value().bool_data)
                    {
                        res+=" = { 0 , ...... } ";
                    }
                }
            }
            else
            {
                if(ic_data->get_value().pointer_data)
                {
                    res+=" = ";
                    if(ic_data->is_const_var())
                    {
                        res+=output_ic_const_operand(ic_data->get_data_type(),ic_data->get_value().pointer_data);
                    }
                    else
                    {
                        res+=output_ic_var_operand((struct ic_data *)ic_data->get_value().pointer_data,false,false);
                    }
                }
            }
        }
    }
    return res;
}

//打印中间代码中的多个变量类型的操作数
string output_ic_vars_operand(list<struct ic_data * > * vars,string separator,bool print_details)
{
    string res;
    if(vars)
    {
        for(auto i:*vars)
        {
            res+=(output_ic_var_operand(i,print_details,false)+separator);
        }
        res=res.substr(0,res.size()-separator.size());
    }
    return res;
}

//打印中间代码中的函数类型的操作数
string output_ic_func_operand(struct ic_func * func,bool print_details)
{
    string res;
    if(print_details)
    {
        res=identifier_type_output_map[func->return_type]+" "+func->name+" ( "+output_ic_vars_operand(func->f_params," , ",print_details)+" ) ";
    }
    else
    {
        res=func->name;
    }
    return res;
}

//打印中间代码的操作数
string output_ic_operand(pair<ic_operand,void *> operand,bool print_details,bool is_define=false)
{
    struct ic_data * ic_data;
    struct ic_func * func;
    string res;
    switch(operand.first)
    {
        case ic_operand::DATA:
            res=output_ic_var_operand((struct ic_data *)operand.second,print_details,print_details,is_define);
            break;
        case ic_operand::DATAS:
            res=output_ic_vars_operand((list<struct ic_data * > *)operand.second," , ",print_details);
            break;
        case ic_operand::LABEL:
            res=output_ic_label_operand((struct ic_label *)operand.second);
            break;
        case ic_operand::FUNC:
            res=output_ic_func_operand((struct ic_func *)operand.second,print_details);
            break;
        case ic_operand::NONE:
            break;
        default:
            break;
    }
    return res;
}

//输出只有操作码的中间代码
string output_only_op_ic(struct quaternion qua)
{
    return "\t"+output_ic_op(qua.op);
}

//输出没有操作数，只有操作码的中间代码
string output_zero_operand_ic(struct quaternion qua)
{
    return "\t"+output_ic_op(qua.op)+"\t\t"+output_ic_operand(qua.result,false);
}

//输出空代码
string output_nop_ic(struct quaternion qua)
{
    return "\tNOP";
}

//输出单操作数的中间代码
string output_one_operand_ic(struct quaternion qua)
{
    return "\t"+output_ic_op(qua.op)+"\t\t"+output_ic_operand(qua.arg1,false)+" --> "+output_ic_operand(qua.result,false,true);
}

//输出双操作数的中间代码
string output_two_operands_ic(struct quaternion qua)
{
    return "\t"+output_ic_op(qua.op)+"\t\t"+output_ic_operand(qua.arg1,false)+" , "+output_ic_operand(qua.arg2,false)+" --> "+output_ic_operand(qua.result,false,true);
}

//输出变量定义的中间代码
string output_var_define_ic(struct quaternion qua)
{
    return ((struct ic_data *)qua.result.second)->is_global()?(output_ic_operand(qua.result,true)):("\t"+output_ic_operand(qua.result,true));
}

//输出函数定义的中间代码
string output_func_define_ic(struct quaternion qua)
{
    return output_ic_operand(qua.result,true)+" : ";
}

//输出结束函数定义的中间代码
string output_end_func_define_ic(struct quaternion qua)
{
    return "";
}

//输出标签定义的中间代码
string output_label_define_ic(struct quaternion qua)
{
    return output_ic_operand(qua.result,true)+" : ";
}

//输出函数调用的中间代码
string output_call_ic(struct quaternion qua)
{
    string res="\t"+output_ic_op(qua.op)+"\t\t"+output_ic_operand(qua.arg1,false)+" ( "+output_ic_operand(qua.arg2,false)+" ) ";
    if(qua.result.first!=ic_operand::NONE)
    {
        res+=(" --> "+output_ic_operand(qua.result,false,true));
    }
    return res;
}

//输出return语句的中间代码
string output_return_ic(struct quaternion qua)
{
    string res="\t"+output_ic_op(qua.op);
    if(qua.result.first!=ic_operand::NONE)
    {
        res+=("\t\t"+output_ic_operand(qua.result,false));
    }
    return res;
}

//各种类型的中间代码的输出函数
map<ic_op,ic_output> ic_outputs={
    {ic_op::NOP,output_nop_ic},                         //空语句
    {ic_op::ASSIGN,output_one_operand_ic},              //赋值
    {ic_op::ADD,output_two_operands_ic},                    //加
    {ic_op::SUB,output_two_operands_ic},                    //减
    {ic_op::MUL,output_two_operands_ic},                    //乘
    {ic_op::DIV,output_two_operands_ic},                    //除
    {ic_op::MOD,output_two_operands_ic},                    //取模
    // {ic_op::L_SHIFT,output_two_operands_ic},            //左移位
    // {ic_op::R_SHIFT,output_two_operands_ic},            //右移位
    {ic_op::NOT,output_one_operand_ic},                    //非
    // {ic_op::AND,output_two_operands_ic},                    //与
    // {ic_op::OR,output_two_operands_ic},                      //或
    // {ic_op::XOR,output_two_operands_ic},                    //异或
    {ic_op::EQ,output_two_operands_ic},                      //是否相等
    {ic_op::UEQ,output_two_operands_ic},                    //是否不相等
    {ic_op::GT,output_two_operands_ic},                      //比较是否大于
    {ic_op::LT,output_two_operands_ic},                      //比较是否小于
    {ic_op::GE,output_two_operands_ic},                      //比较是否大于等于
    {ic_op::LE,output_two_operands_ic},                      //比较是否小于等于
    {ic_op::JMP,output_zero_operand_ic},                    //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
    {ic_op::IF_JMP,output_one_operand_ic},              //结果为true的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
    {ic_op::IF_NOT_JMP,output_one_operand_ic},      //结果为false的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
    {ic_op::VAR_DEFINE,output_var_define_ic},      //变量定义（可以赋初始值，也可以不赋初始值）
    {ic_op::FUNC_DEFINE,output_func_define_ic},    //函数定义（第一操作数是要定义的函数名，第二个操作数是形参列表，没有结果）
    {ic_op::END_FUNC_DEFINE,output_end_func_define_ic},    //函数定义（第一操作数是要定义的函数名，第二个操作数是形参列表，没有结果）
    {ic_op::LABEL_DEFINE,output_label_define_ic},  //定义一个LABEL
    {ic_op::CALL,output_call_ic},                  //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
    {ic_op::RET,output_return_ic},                    //函数返回（只有结果，没有操作数，结果就是返回值）
};

/*================================================================================================================================================================*/
/*================================================================================================================================================================*/
/*================================================================================================================================================================*/
/*================================================================================================================================================================*/
/*================================================================================================================================================================*/



//===================================== struct quaternion =====================================//

inline struct ic_data * replace_datas_in_var(struct ic_data * var,struct ic_data * source,struct ic_data * destination)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    if(var==source)
    {
        var=destination;
    }
    else if(var->is_array_member())
    {
        if(var->get_offset()==source)
        {
            if(var->is_array_var())
            {
                var=symbol_table->array_member_entry(var->get_belong_array(),var->dimensions_len->size(),destination);
            }
            else
            {
                var=symbol_table->array_member_not_array_var_entry(var->get_belong_array(),destination);
            }
        }
        else if(var->get_belong_array()==source)
        {
            if(var->is_array_var())
            {
                var=symbol_table->array_member_entry(destination,var->dimensions_len->size(),var->get_offset());
            }
            else
            {
                var=symbol_table->array_member_not_array_var_entry(destination,var->get_offset());
            }
        }
    }
    return var;
}

void quaternion::replace_datas(struct ic_data * source,struct ic_data * destination,bool only_use_datas)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    list<struct ic_data * > * r_params,* new_r_params;
    switch(op)
    {
        case ic_op::ASSIGN:
        case ic_op::NOT:
            arg1.second=(void *)replace_datas_in_var((struct ic_data *)arg1.second,source,destination);
            if(!only_use_datas)
            {
                result.second=(void *)replace_datas_in_var((struct ic_data *)result.second,source,destination);
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
            arg1.second=(void *)replace_datas_in_var((struct ic_data *)arg1.second,source,destination);
            arg2.second=(void *)replace_datas_in_var((struct ic_data *)arg2.second,source,destination);
            if(!only_use_datas)
            {
                result.second=(void *)replace_datas_in_var((struct ic_data *)result.second,source,destination);
            }
            break;
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
            arg1.second=(void *)replace_datas_in_var((struct ic_data *)arg1.second,source,destination);
            break;
        case ic_op::CALL:
            r_params=(list<struct ic_data * > *)arg2.second;
            new_r_params=new list<struct ic_data * >;
            for(auto r_param:*r_params)
            {
                new_r_params->push_back(replace_datas_in_var(r_param,source,destination));
            }
            delete r_params;
            arg2.second=(void *)new_r_params;
            if(result.second && !only_use_datas)
            {
                result.second=(void *)replace_datas_in_var((struct ic_data *)result.second,source,destination);
            }
            break;
        case ic_op::RET:
            if(result.second)
            {
                result.second=(void *)replace_datas_in_var((struct ic_data *)result.second,source,destination);
            }
            break;
        case ic_op::VAR_DEFINE:
            if(!only_use_datas)
            {
                result.second=(void *)replace_datas_in_var((struct ic_data *)result.second,source,destination);
            }
            break;
        default:
            break;
    }
}

inline struct ic_data * replace_all_datas_in_var(struct ic_data * var,const map<struct ic_data *,struct ic_data *> & old_and_new_vars_map)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    if(old_and_new_vars_map.find(var)!=old_and_new_vars_map.end())
    {
        var=old_and_new_vars_map.at(var);
    }
    else if(var->is_array_member())
    {
        if(old_and_new_vars_map.find(var->get_offset())!=old_and_new_vars_map.end())
        {
            if(var->is_array_var())
            {
                var=symbol_table->array_member_entry(var->get_belong_array(),var->dimensions_len->size(),old_and_new_vars_map.at(var->get_offset()));
            }
            else
            {
                var=symbol_table->array_member_not_array_var_entry(var->get_belong_array(),old_and_new_vars_map.at(var->get_offset()));
            }
        }
        if(old_and_new_vars_map.find(var->get_belong_array())!=old_and_new_vars_map.end())
        {
            if(var->is_array_var())
            {
                var=symbol_table->array_member_entry(old_and_new_vars_map.at(var->get_belong_array()),var->dimensions_len->size(),var->get_offset());
            }
            else
            {
                var=symbol_table->array_member_not_array_var_entry(old_and_new_vars_map.at(var->get_belong_array()),var->get_offset());
            }
        }
    }
    return var;
}

void quaternion::replace_all_vars(const map<struct ic_data *,struct ic_data *> & old_and_new_vars_map)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    list<struct ic_data * > * r_params,* new_r_params;
    switch(op)
    {
        case ic_op::ASSIGN:
        case ic_op::NOT:
            arg1.second=(void *)replace_all_datas_in_var((struct ic_data *)arg1.second,old_and_new_vars_map);
            result.second=(void *)replace_all_datas_in_var((struct ic_data *)result.second,old_and_new_vars_map);
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
            arg1.second=(void *)replace_all_datas_in_var((struct ic_data *)arg1.second,old_and_new_vars_map);
            arg2.second=(void *)replace_all_datas_in_var((struct ic_data *)arg2.second,old_and_new_vars_map);
            result.second=(void *)replace_all_datas_in_var((struct ic_data *)result.second,old_and_new_vars_map);
            break;
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
            arg1.second=(void *)replace_all_datas_in_var((struct ic_data *)arg1.second,old_and_new_vars_map);
            break;
        case ic_op::CALL:
            r_params=(list<struct ic_data * > *)arg2.second;
            new_r_params=new list<struct ic_data * >;
            for(auto r_param:*r_params)
            {
                new_r_params->push_back(replace_all_datas_in_var(r_param,old_and_new_vars_map));
            }
            delete r_params;
            arg2.second=(void *)new_r_params;
            if(result.second)
            {
                result.second=(void *)replace_all_datas_in_var((struct ic_data *)result.second,old_and_new_vars_map);
            }
            break;
        case ic_op::RET:
            if(result.second)
            {
                result.second=(void *)replace_all_datas_in_var((struct ic_data *)result.second,old_and_new_vars_map);
            }
            break;
        case ic_op::VAR_DEFINE:
            result.second=(void *)replace_all_datas_in_var((struct ic_data *)result.second,old_and_new_vars_map);
            break;
        default:
            break;
    }
}

void quaternion::replace_all_labels(const map<struct ic_label *,struct ic_label *> & old_and_new_labels_map)
{
    struct ic_label * label;
    switch(op)
    {
        case ic_op::JMP:
        case ic_op::IF_JMP:
        case ic_op::IF_NOT_JMP:
        case ic_op::LABEL_DEFINE:
            label=(struct ic_label * )result.second;
            if(old_and_new_labels_map.find(label)!=old_and_new_labels_map.end())
            {
                result.second=(void *)old_and_new_labels_map.at(label);
            }
            break;
        default:
            break;
    }
}

//==========================================================================//



//语法分析时的全局信息
struct// global_info
{
    // global_info():current_func(nullptr)
    // {

    // };

    enum language_data_type data_type;
    struct ic_func * current_func=nullptr;
} global_info;

//每一个语义动作规约完成之前和之后，栈顶的指针的差值+1（规约前栈顶指针减去规约后栈顶指针+1）
map<string,stack_index> stack_top_offset_between_before_and_after_reduce_map;

//每一个语义动作用到的语法符号和它们所在的语义栈中的位置距离栈顶的偏移量
//该map的键是语义动作的名称
//值是另一个map，这个map的键是该语义动作可能会用到的语义符号的名称，值是这个语义符号距离栈顶的偏移量
map<string,map<string,vector<int> > > syntax_symbol_offset_from_stack_top_map;

//ic_data的数据类型转换
#define ic_data_type_conversion(value,to_data_type) \
{\
    struct ic_data * tmp_var;\
    OAA const_value;\
    enum language_data_type from_data_type=(value)->get_data_type();\
    if(from_data_type!=to_data_type)\
    {\
        if((value)->is_const() && !(value)->is_array_var())\
        {\
            const_value=(value)->get_value();\
            const_value.type_conversion(from_data_type,to_data_type);\
            value=def_const(to_data_type,const_value);\
        }\
        else\
        {\
            tmp_var=def_tmp_var(to_data_type);\
            gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,value,ic_operand::DATA,tmp_var);\
            value=tmp_var;\
        }\
    }\
}

/*
在一元运算中计算结果的值

Parameters
----------
op:操作符
arg:操作数

Return
------
返回计算结果
*/
struct ic_data * unary_compute(ic_op op,struct ic_data * arg)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * res=nullptr;
    OAA value;
    enum language_data_type data_type;
    if(!arg || !arg->is_const())
    {
        return res;
    }
    value=arg->get_value();
    data_type=arg->get_data_type();
    switch(op)
    {
        case ic_op::ADD:
            if(data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)value.int_data));
            }
            else if(data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)value.float_data));
            }
            break;
        case ic_op::SUB:
            if(data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)0-value.int_data));
            }
            else if(data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)(0.0-(value.float_data))));
            }
            break;
        case ic_op::NOT:
            if(data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)!value.int_data));
            }
            else if(data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)!value.float_data));
            }
            break;
        default:
            cout<<"Unary compute error!"<<endl;
            break;
    }
    return res;
}

/*
在二元运算中计算结果的值

Parameters
----------
op:操作符
arg1:操作数1
arg2:操作数2

Return
------
返回计算结果
*/
struct ic_data * binary_compute(ic_op op,struct ic_data * arg1,struct ic_data * arg2)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * res=nullptr;
    enum language_data_type arg1_data_type,arg2_data_type;
    OAA arg1_value,arg2_value;
    if(!arg1 || !arg2 || !arg1->is_const() || !arg2->is_const())
    {
        return res;
    }
    arg1_data_type=arg1->data_type;
    arg1_value=arg1->get_value();
    arg2_data_type=arg2->data_type;
    arg2_value=arg2->get_value();
    switch(op)
    {
        case ic_op::ADD:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data+arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.int_data+arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data+arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data+arg2_value.float_data));
            }
            break;
        case ic_op::SUB:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data-arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.int_data-arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data-arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data-arg2_value.float_data));
            }
            break;
        case ic_op::MUL:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data*arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.int_data*arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data*arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data*arg2_value.float_data));
            }
            break;
        case ic_op::DIV:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data/arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.int_data/arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data/arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::FLOAT,((float)arg1_value.float_data/arg2_value.float_data));
            }
            break;
        case ic_op::MOD:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data%arg2_value.int_data));
            }
            break;
        case ic_op::EQ:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data==arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data==arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data==arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data==arg2_value.float_data));
            }
            break;
        case ic_op::UEQ:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data!=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data!=arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data!=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data!=arg2_value.float_data));
            }
            break;
        case ic_op::LT:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data<arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data<arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data<arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data<arg2_value.float_data));
            }
            break;
        case ic_op::GT:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data>arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data>arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data>arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data>arg2_value.float_data));
            }
            break;
        case ic_op::LE:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data<=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data<=arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data<=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data<=arg2_value.float_data));
            }
            break;
        case ic_op::GE:
            if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data>=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::INT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.int_data>=arg2_value.float_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::INT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data>=arg2_value.int_data));
            }
            else if(arg1_data_type==language_data_type::FLOAT && arg2_data_type==language_data_type::FLOAT)
            {
                res=def_const(language_data_type::INT,((int)arg1_value.float_data>=arg2_value.float_data));
            }
            break;
        default:
            cout<<"Binary compute error!"<<endl;
            break;
    }
    return res;
}

/*
进行类型转换
*/
enum language_data_type type_conversion(enum language_data_type type_1,enum language_data_type type_2)
{
    if(type_1==language_data_type::FLOAT || type_2==language_data_type::FLOAT)
    {
        return language_data_type::FLOAT;
    }
    return language_data_type::INT;
}

//二元运算的语义规则
//该语义规则要满足arg1_semantic_symbol -> arg1_semantic_symbol op arg2_semantic_symbol
//这个语义动作的定义不适用于具有短路性质的二元运算
#define binary_compute_semantic_rule(semantic_rule_name,op,arg1_semantic_symbol,arg2_semantic_symbol) \
define_semantic_rule(semantic_rule_name)\
    struct ic_data * arg1,* arg2,* res,* tmp_var;\
    bool is_result_const=true;\
    if(check_syntax_symbol_attribute_by_index(arg1_semantic_symbol,value,1))\
    {\
        arg1=(struct ic_data * )get_syntax_symbol_attribute_by_index(arg1_semantic_symbol,value,pointer,1);\
        if(!arg1->is_const())\
        {\
            is_result_const=false;\
        }\
    }\
    else\
    {\
        if(check_syntax_symbol_attribute_by_index(arg1_semantic_symbol,void_return_func_result,1))\
        {\
            print_error("Func has void return type!");\
        }\
        print_error(string(#semantic_rule_name)+" ERROR!");\
    }\
    if(check_syntax_symbol_attribute(arg2_semantic_symbol,value))\
    {\
        arg2=(struct ic_data * )get_syntax_symbol_attribute(arg2_semantic_symbol,value,pointer);\
        if(!arg2->is_const())\
        {\
            is_result_const=false;\
        }\
    }\
    else\
    {\
        if(check_syntax_symbol_attribute_by_index(arg1_semantic_symbol,void_return_func_result,1))\
        {\
            print_error("Func has void return type!");\
        }\
        print_error(string(#semantic_rule_name)+" ERROR!");\
    }\
    if(is_result_const)\
    {\
        set_syntax_symbol_attribute_by_index(arg1_semantic_symbol,value,pointer,0,binary_compute(op,arg1,arg2));\
    }\
    else\
    {\
        if(arg1->get_data_type()==language_data_type::INT && arg2->get_data_type()==language_data_type::FLOAT)\
        {\
            if(arg1->is_const())\
            {\
                arg1=def_const(language_data_type::FLOAT,(float)arg1->get_value().int_data);\
            }\
            else\
            {\
                tmp_var=def_tmp_var(language_data_type::FLOAT);\
                gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,arg1,ic_operand::DATA,tmp_var);\
                arg1=tmp_var;\
            }\
        }\
        else if(arg1->get_data_type()==language_data_type::FLOAT && arg2->get_data_type()==language_data_type::INT)\
        {\
            if(arg2->is_const())\
            {\
                arg2=def_const(language_data_type::FLOAT,(float)arg2->get_value().int_data);\
            }\
            else\
            {\
                tmp_var=def_tmp_var(language_data_type::FLOAT);\
                gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,arg2,ic_operand::DATA,tmp_var);\
                arg2=tmp_var;\
            }\
        }\
        res=def_tmp_var(type_conversion(arg1->get_data_type(),arg2->get_data_type()));\
        gen_two_operands_code(op,ic_operand::DATA,arg1,ic_operand::DATA,arg2,ic_operand::DATA,res);\
        set_syntax_symbol_attribute_by_index(arg1_semantic_symbol,value,pointer,0,res);\
    }\
end_define_semantic_rule

/*
编译单元
*/
define_semantic_rule(___COMP_UNIT_1___)
end_define_semantic_rule

/*
编译单元
*/
define_semantic_rule(___COMP_UNIT_2___)
end_define_semantic_rule

/*
变量声明或者函数定义
*/
define_semantic_rule(___DECL_or_FUNC_DEF_1___)
    copy_syntax_symbol_attributes(DECL_or_FUNC_DEF,DECL);
end_define_semantic_rule

/*
变量声明或者函数定义
*/
define_semantic_rule(___DECL_or_FUNC_DEF_2___)
end_define_semantic_rule

/*
const变量定义
*/
define_semantic_rule(___DECL_1___)
    copy_syntax_symbol_attributes(DECL,CONST_DECL);
end_define_semantic_rule

/*
变量定义
*/
define_semantic_rule(___DECL_2___)
    copy_syntax_symbol_attributes(DECL,VAR_DECL);
end_define_semantic_rule

/*
基本数据类型int
*/
define_semantic_rule(___B_TYPE_1___)
    set_syntax_symbol_attribute(B_TYPE,b_type,int,(int)language_data_type::INT);
    set_global_info(data_type,language_data_type::INT);
    //relate_data_type_to_curretn_func(language_data_type::INT);
end_define_semantic_rule

/*
基本数据类型float
*/
define_semantic_rule(___B_TYPE_2___)
    set_syntax_symbol_attribute(B_TYPE,b_type,int,(int)language_data_type::FLOAT);
    set_global_info(data_type,language_data_type::FLOAT);
    //relate_data_type_to_curretn_func(language_data_type::FLOAT);
end_define_semantic_rule

/*
常量声明
*/
define_semantic_rule(___CONST_DECL___)
    list<struct ic_data * > * const_defs=(list<struct ic_data * > *)get_syntax_symbol_attribute(one_or_more_CONST_DEF_divided_with_comma,const_defs,pointer);
    //生成定义const变量的语句
    for(auto i:*const_defs)
    {
        gen_zero_operand_code(ic_op::VAR_DEFINE,ic_operand::DATA,i);
    }
    delete const_defs;
end_define_semantic_rule

/*
若干个常量声明
*/
define_semantic_rule(___one_or_more_CONST_DEF_divided_with_comma_1___)
    list<struct ic_data * > * const_defs=(list<struct ic_data * > *)get_syntax_symbol_attribute_by_index(one_or_more_CONST_DEF_divided_with_comma,const_defs,pointer,1);
    const_defs->push_front((struct ic_data *)get_syntax_symbol_attribute(CONST_DEF,const_def,pointer));
    set_syntax_symbol_attribute_by_index(one_or_more_CONST_DEF_divided_with_comma,const_defs,pointer,0,const_defs);
end_define_semantic_rule

/*
若干个常量声明
*/
define_semantic_rule(___one_or_more_CONST_DEF_divided_with_comma_2___)
    list<struct ic_data * > * const_defs=new list<struct ic_data * >;
    const_defs->push_front((struct ic_data *)get_syntax_symbol_attribute(CONST_DEF,const_def,pointer));
    set_syntax_symbol_attribute(one_or_more_CONST_DEF_divided_with_comma,const_defs,pointer,const_defs);
end_define_semantic_rule

OAA get_first_in_depth_first_in_const_array(list<pair<bool,void * > > * tree,enum language_data_type array_data_type)
{
    OAA res;
    struct ic_data * ic_data;
    if(tree==nullptr)
    {
        OAA_assign(res,0,array_data_type);
    }
    else if(tree->front().first)
    {
        ic_data=((struct ic_data * )(tree->front().second));
        res=ic_data->get_value();
        res.type_conversion(ic_data->get_data_type(),ic_data->get_data_type());
    }
    else
    {
        res=get_first_in_depth_first_in_const_array((list<pair<bool,void * > > *)(tree->front().second),array_data_type);
    }
    return res;
}

long const_assign_array_to_array_member(list<pair<bool,void * > > * array,vector<OAA> * init_value,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    long push_times=0;
    if(dimensions_len_stack.empty())
    {
        return push_times;
    }
    while(dimensions_len_pointer!=dimensions_len->end())
    {
        dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
        dimensions_len_pointer++;
        push_times++;
    }
    init_value->push_back(get_first_in_depth_first_in_const_array(array,array_data_type));
    dimensions_len_stack.top()--;
    while(dimensions_len_stack.top()==0)
    {
        dimensions_len_stack.pop();
        dimensions_len_pointer--;
        push_times--;
        if(dimensions_len_stack.empty())
        {
            return push_times;
        }
        dimensions_len_stack.top()--;
    }
    return push_times;
}

long const_assign_var_to_array_member(struct ic_data * single_value,vector<OAA> * init_value,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    long push_times=0;
    OAA tmp;
    if(dimensions_len_stack.empty())
    {
        return push_times;
    }
    while(dimensions_len_pointer!=dimensions_len->end())
    {
        dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
        dimensions_len_pointer++;
        push_times++;
    }
    tmp=single_value->get_value();
    tmp.type_conversion(single_value->get_data_type(),array_data_type);
    init_value->push_back(tmp);
    dimensions_len_stack.top()--;
    while(dimensions_len_stack.top()==0)
    {
        dimensions_len_stack.pop();
        dimensions_len_pointer--;
        push_times--;
        if(dimensions_len_stack.empty())
        {
            return push_times;
        }
        dimensions_len_stack.top()--;
    }
    return push_times;
}

long const_array_init_assignment(vector<OAA> * init_value,stack<list<pair<bool,void * > > * > & init_values_stack,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    long push_times=0;
    if(init_values_stack.empty() || init_values_stack.top()->size()==0 || (init_values_stack.top()->front().first==false && init_values_stack.top()->front().second==nullptr))
    {
        if(!init_values_stack.empty() && !init_values_stack.top()->empty() && (init_values_stack.top()->front().first==false && init_values_stack.top()->front().second==nullptr))
        {
            init_values_stack.top()->pop_front();
        }
        if(!init_values_stack.empty() && init_values_stack.top()->size()==0)
        {
            init_values_stack.pop();
        }
        do
        {
            push_times+=const_assign_var_to_array_member(def_const(array_data_type,0),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
        } while (push_times>0);
    }
    else if(dimensions_len_pointer==dimensions_len->end())
    {
        while(push_times>=0 && !init_values_stack.top()->empty())
        {
            if(init_values_stack.top()->front().first)
            {
                push_times+=const_assign_var_to_array_member(((struct ic_data *)(init_values_stack.top()->front().second)),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            else
            {
                push_times+=const_assign_array_to_array_member((list<pair<bool,void * > > *)(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            init_values_stack.top()->pop_front();
        }
        init_values_stack.pop();
    }
    else
    {
        while(push_times>=0 && init_values_stack.top()->size()!=0)
        {
            if(init_values_stack.top()->front().first)
            {
                push_times+=const_assign_var_to_array_member(((struct ic_data *)(init_values_stack.top()->front().second)),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            else
            {
                if(dimensions_len_pointer!=dimensions_len->end())
                {
                    init_values_stack.push((list<pair<bool,void * > > * )(init_values_stack.top()->front().second));
                    dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
                    dimensions_len_pointer++;
                    push_times++;
                    push_times+=const_array_init_assignment(init_value,init_values_stack,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
                }
                else
                {
                    push_times+=const_assign_array_to_array_member((list<pair<bool,void * > > * )(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
                }
            }
            init_values_stack.top()->pop_front();
        }
        init_values_stack.pop();
    }
    while(push_times>=0)
    {
        push_times+=const_assign_var_to_array_member(def_const(array_data_type,0),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
    }
    return push_times;
}

void delete_const_array_assignment(list<pair<bool,void * > > * array_assignment)
{
    if(array_assignment)
    {
        for(auto i:(*array_assignment))
        {
            if(i.first==false)
            {
                delete_const_array_assignment((list<pair<bool,void * > > *)i.second);
            }
        }
        delete array_assignment;
    }
}

/*
const变量定义
*/
define_semantic_rule(___CONST_DEF___)
    struct ic_data * const_var;
    list<struct ic_data * > * dimensions_len;
    list<struct ic_data * >::iterator dimensions_len_pointer;
    stack<size_t> dimensions_len_stack;
    stack<list<pair<bool,void * > > * > init_values_stack;
    vector<OAA> * array_init_value;
    list<pair<bool,void * > > * array_assignment;
    if(check_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len))
    {
        //定义的const变量是一个数组变量
        if(check_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values))
        {
            array_init_value=new vector<OAA>;
            dimensions_len=(list<struct ic_data * > *)get_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,pointer);
            dimensions_len_pointer=dimensions_len->begin();
            dimensions_len_stack.push(1);
            array_assignment=(list<pair<bool,void * > > *)get_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values,pointer);
            init_values_stack.push(array_assignment);
            const_array_init_assignment(array_init_value,init_values_stack,dimensions_len_stack,dimensions_len,dimensions_len_pointer,get_global_info(data_type));
            delete_const_array_assignment(array_assignment);
            const_var=def_const_array((*(string *)get_syntax_symbol_attribute(id,id_string,pointer)),get_global_info(data_type),dimensions_len,array_init_value);
        }
        else
        {
            print_error("Array initializer must be an initializer list");
        }
    }
    else
    {
        //定义的const变量是一个基本变量
        if(check_syntax_symbol_attribute(CONST_INIT_VAL,value))
        {
            const_var=def_const_var((*(string *)get_syntax_symbol_attribute(id,id_string,pointer)),get_global_info(data_type),((struct ic_data *)get_syntax_symbol_attribute(CONST_INIT_VAL,value,pointer))->get_value());
        }
        else
        {
            print_error("Const ic_data assign error while defined!");
        }
    }
    set_syntax_symbol_attribute(CONST_DEF,const_def,pointer,const_var);
end_define_semantic_rule

/*
数组维度定义
*/
define_semantic_rule(___zero_or_more_CONST_EXP_surrounded_by_bracket_1___)
    //什么也不用做
end_define_semantic_rule

/*
数组维度定义
*/
define_semantic_rule(___zero_or_more_CONST_EXP_surrounded_by_bracket_2___)
    list<struct ic_data * > * dimensions_len;
    struct ic_data * dimension_len_var;
    if(check_syntax_symbol_attribute_by_index(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,1))
    {
        dimensions_len=(list<struct ic_data * > *)get_syntax_symbol_attribute_by_index(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,pointer,1);
    }
    else
    {
        dimensions_len=new list<struct ic_data * >;
    }
    dimension_len_var=(struct ic_data *)get_syntax_symbol_attribute(CONST_EXP,value,pointer);
    //数组的维度定义一定要是int类型的常数
    ic_data_type_conversion(dimension_len_var,language_data_type::INT);
    dimensions_len->push_front(dimension_len_var);
    set_syntax_symbol_attribute_by_index(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,pointer,0,dimensions_len);
end_define_semantic_rule

/*
常量初始值
*/
define_semantic_rule(___CONST_INIT_VAL_1___)
    //需要将这些初始值转换成对应的数据类型
    struct ic_data * init_value_var=(struct ic_data *)get_syntax_symbol_attribute(CONST_EXP,value,pointer);
    enum language_data_type data_type=get_global_info(data_type);
    ic_data_type_conversion(init_value_var,data_type);
    set_syntax_symbol_attribute(CONST_INIT_VAL,value,pointer,init_value_var);
    //copy_syntax_symbol_attributes(CONST_INIT_VAL,CONST_EXP);
end_define_semantic_rule

/*
常量初始值
*/
define_semantic_rule(___CONST_INIT_VAL_2___)
    list<pair<bool,void * > > * not_leaf=new list<pair<bool,void * > >;
    list<pair<bool,void * > > * tmp;
    enum language_data_type data_type=get_global_info(data_type);
    if(check_syntax_symbol_attribute(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values))
    {
        //此时这个pair的first代表second是不是一个叶子结点（true为是），叶子结点就是一个struct ic_data *，非叶子结点就是一个list<pair<bool,void * > > *
        not_leaf->push_back(make_pair(false,(void *)get_syntax_symbol_attribute(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values,pointer)));
    }
    else
    {
        //如果pair<bool,void * >的first为false，但是second为nullptr，那么说明此时这个非叶子结点是一个空的{}
        tmp=new list<pair<bool,void * > >;
        switch(data_type)
        {
            case language_data_type::INT:
                tmp->push_back(make_pair(true,(void *)def_const(data_type,(int)0)));
                break;
            case language_data_type::FLOAT:
                tmp->push_back(make_pair(true,(void *)def_const(data_type,(float)0)));
                break;
            default:
                break;
        }
        not_leaf->push_back(make_pair(false,(void *)tmp));
    }
    set_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values,pointer,not_leaf);
end_define_semantic_rule

/*
常量序列
*/
define_semantic_rule(___zero_or_more_CONST_INIT_VAL_divided_with_comma_1___)
    //什么也不用做
end_define_semantic_rule

/*
常量序列
*/
define_semantic_rule(___zero_or_more_CONST_INIT_VAL_divided_with_comma_2___)
    list<pair<bool,void * > > * const_init_values;
    if(check_syntax_symbol_attribute(CONST_INIT_VAL,value))
    {
        const_init_values=new list<pair<bool,void * > >;
        const_init_values->push_front(make_pair(true,(void *)get_syntax_symbol_attribute(CONST_INIT_VAL,value,pointer)));
        set_syntax_symbol_attribute(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values,pointer,const_init_values);
    }
    else if(check_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values))
    {
        copy_syntax_symbol_attributes(zero_or_more_CONST_INIT_VAL_divided_with_comma,CONST_INIT_VAL);
    }
    else
    {
        print_error("___zero_or_more_CONST_INIT_VAL_divided_with_comma_2___ error!");
    }
end_define_semantic_rule

/*
常量序列
*/
define_semantic_rule(___zero_or_more_CONST_INIT_VAL_divided_with_comma_3___)
    list<pair<bool,void * > > * const_init_values,* another_const_init_values;
    if(check_syntax_symbol_attribute_by_index(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values,1))
    {
        const_init_values=(list<pair<bool,void * > > *)get_syntax_symbol_attribute_by_index(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values,pointer,1);
    }
    else
    {
        const_init_values=new list<pair<bool,void * > >;
    }
    if(check_syntax_symbol_attribute(CONST_INIT_VAL,value))
    {
        const_init_values->push_front(make_pair(true,(void * )get_syntax_symbol_attribute(CONST_INIT_VAL,value,pointer)));
    }
    else if(check_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values))
    {
        another_const_init_values=(list<pair<bool,void * > > *)get_syntax_symbol_attribute(CONST_INIT_VAL,const_init_values,pointer);
        const_init_values->insert(const_init_values->begin(),another_const_init_values->begin(),another_const_init_values->end());
        delete another_const_init_values;
    }
    else
    {
        print_error("___zero_or_more_CONST_INIT_VAL_divided_with_comma_3___ error!");
    }
    set_syntax_symbol_attribute_by_index(zero_or_more_CONST_INIT_VAL_divided_with_comma,const_init_values,pointer,0,const_init_values);
end_define_semantic_rule

/*
变量声明
*/
define_semantic_rule(___VAR_DECL___)
    list<struct ic_data * > * var_defs=(list<struct ic_data * > *)get_syntax_symbol_attribute(one_or_more_VAR_DEF_divided_with_comma,var_defs,pointer);
    vector<struct ic_data * > * init_values;
    vector<OAA> * new_init_values;
    size_t array_member_index,array_get_member_dimension_num;
    //生成定义变量的语句
    if(!(get_current_scope()->is_global()))
    {
        for(auto i : *var_defs)
        {
            gen_zero_operand_code(ic_op::VAR_DEFINE,ic_operand::DATA,i);
            if(i->is_const_var())
            {
                continue;
            }
            if(i->is_array_var())
            {
                init_values=(vector<struct ic_data * > * )i->get_value().pointer_data;
                if(init_values)
                {
                    array_member_index=0;
                    array_get_member_dimension_num=i->dimensions_len->size();
                    switch(i->get_data_type())
                    {
                        case language_data_type::INT:
                            for(auto j:*init_values)
                            {
                                if(!(j->is_const() && !j->is_array_var() && j->get_value().int_data==0))
                                {
                                    gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,j,ic_operand::DATA,def_array_member(i,array_get_member_dimension_num,def_const(language_data_type::INT,(int)array_member_index)));
                                }
                                array_member_index++;
                            }
                            break;
                        case language_data_type::FLOAT:
                            for(auto j:*init_values)
                            {
                                if(!(j->is_const() && !j->is_array_var() && j->get_value().float_data==0))
                                {
                                    gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,j,ic_operand::DATA,def_array_member(i,array_get_member_dimension_num,def_const(language_data_type::INT,(int)array_member_index)));
                                }
                                array_member_index++;
                            }
                            break;
                        default:
                            break;
                    }
                    delete init_values;
                    i->set_value(OAA(true));
                }
                else
                {
                    i->set_value(OAA(false));
                }
            }
            else
            {
                if(i->get_value().pointer_data)
                {
                    gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,i->get_value().pointer_data,ic_operand::DATA,i);
                }
                i->set_value(OAA((void *)nullptr));
            }
        }
    }
    else
    {
        for(auto i : *var_defs)
        {
            if(i->is_array_var() && !i->is_const_var() && i->get_value().pointer_data)
            {
                init_values=(vector<struct ic_data * > * )i->get_value().pointer_data;
                new_init_values=new vector<OAA>;
                for(auto i:*init_values)
                {
                    new_init_values->push_back(i->get_value().int_data);
                }
                i->set_value(OAA((void *)new_init_values));
                delete init_values;
            }
            gen_zero_operand_code(ic_op::VAR_DEFINE,ic_operand::DATA,i);
        }
    }
    delete var_defs;
end_define_semantic_rule

/*
若干个变量定义
*/
define_semantic_rule(___one_or_more_VAR_DEF_divided_with_comma_1___)
    list<struct ic_data * > * var_defs=new list<struct ic_data * >;
    var_defs->push_front((struct ic_data *)get_syntax_symbol_attribute(VAR_DEF,var_def,pointer));
    set_syntax_symbol_attribute(one_or_more_VAR_DEF_divided_with_comma,var_defs,pointer,var_defs);
end_define_semantic_rule

/*
若干个变量定义
*/
define_semantic_rule(___one_or_more_VAR_DEF_divided_with_comma_2___)
    list<struct ic_data * > * var_defs=(list<struct ic_data * > *)get_syntax_symbol_attribute_by_index(one_or_more_VAR_DEF_divided_with_comma,var_defs,pointer,1);
    var_defs->push_front((struct ic_data *)get_syntax_symbol_attribute(VAR_DEF,var_def,pointer));
    set_syntax_symbol_attribute_by_index(one_or_more_VAR_DEF_divided_with_comma,var_defs,pointer,0,var_defs);
end_define_semantic_rule

/*
变量定义
*/
define_semantic_rule(___VAR_DEF_1___)
    struct ic_data * ic_data;
    if(check_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len))
    {
        //如果定义的变量是一个数组
        ic_data=def_array(*(string *)get_syntax_symbol_attribute(id,id_string,pointer),get_global_info(data_type),(list<struct ic_data * > * )get_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,pointer));
    }
    else
    {
        //如果定义的变量是一个普通变量
        ic_data=def_var(*(string *)get_syntax_symbol_attribute(id,id_string,pointer),get_global_info(data_type));
    }
    set_syntax_symbol_attribute(VAR_DEF,var_def,pointer,ic_data);
end_define_semantic_rule

struct ic_data * get_first_in_depth_first(list<pair<bool,void * > > * tree,enum language_data_type array_data_type)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * res;
    if(tree==nullptr)
    {
        res=def_const(array_data_type,0);
    }
    else if(tree->front().first)
    {
        res=(struct ic_data *)(tree->front().second);
    }
    else
    {
        res=get_first_in_depth_first((list<pair<bool,void * > > *)(tree->front().second),array_data_type);
    }
    return res;
}

long assign_array_to_array_member(list<pair<bool,void * > > * array,vector<struct ic_data * > * init_value,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    long push_times=0;
    if(dimensions_len_stack.empty())
    {
        return push_times;
    }
    while(dimensions_len_pointer!=dimensions_len->end())
    {
        dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
        dimensions_len_pointer++;
        push_times++;
    }
    init_value->push_back(get_first_in_depth_first(array,array_data_type));
    dimensions_len_stack.top()--;
    while(dimensions_len_stack.top()==0)
    {
        dimensions_len_stack.pop();
        dimensions_len_pointer--;
        push_times--;
        if(dimensions_len_stack.empty())
        {
            return push_times;
        }
        dimensions_len_stack.top()--;
    }
    return push_times;
}

long assign_var_to_array_member(struct ic_data * single_value,vector<struct ic_data * > * init_value,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    long push_times=0;
    if(dimensions_len_stack.empty())
    {
        return push_times;
    }
    while(dimensions_len_pointer!=dimensions_len->end())
    {
        dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
        dimensions_len_pointer++;
        push_times++;
    }
    init_value->push_back(single_value);
    dimensions_len_stack.top()--;
    while(dimensions_len_stack.top()==0)
    {
        dimensions_len_stack.pop();
        dimensions_len_pointer--;
        push_times--;
        if(dimensions_len_stack.empty())
        {
            return push_times;
        }
        dimensions_len_stack.top()--;
    }
    return push_times;
}

long array_init_assignment(vector<struct ic_data * > * init_value,stack<list<pair<bool,void * > > * > & init_values_stack,stack<size_t> & dimensions_len_stack,list<struct ic_data * > * dimensions_len,list<struct ic_data * >::iterator & dimensions_len_pointer,enum language_data_type array_data_type)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    long push_times=0;
    if(init_values_stack.empty() || init_values_stack.top()->size()==0 || (init_values_stack.top()->front().first==false && init_values_stack.top()->front().second==nullptr))
    {
        if(!init_values_stack.empty() && !init_values_stack.top()->empty() && (init_values_stack.top()->front().first==false && init_values_stack.top()->front().second==nullptr))
        {
            init_values_stack.top()->pop_front();
        }
        if(!init_values_stack.empty() && init_values_stack.top()->size()==0)
        {
            init_values_stack.pop();
        }
        do
        {
            push_times+=assign_var_to_array_member(def_const(array_data_type,0),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
        } while (push_times>0);
    }
    else if(dimensions_len_pointer==dimensions_len->end())
    {
        while(push_times>=0 && !init_values_stack.top()->empty())
        {
            if(init_values_stack.top()->front().first)
            {
                push_times+=assign_var_to_array_member((struct ic_data *)(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            else
            {
                push_times+=assign_array_to_array_member((list<pair<bool,void * > > *)(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            init_values_stack.top()->pop_front();
        }
        init_values_stack.pop();
    }
    else
    {
        while(push_times>=0 && init_values_stack.top()->size()!=0)
        {
            if(init_values_stack.top()->front().first)
            {
                push_times+=assign_var_to_array_member((struct ic_data *)(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
            }
            else
            {
                if(dimensions_len_pointer!=dimensions_len->end())
                {
                    init_values_stack.push((list<pair<bool,void * > > * )(init_values_stack.top()->front().second));
                    dimensions_len_stack.push((*dimensions_len_pointer)->get_value().int_data);
                    dimensions_len_pointer++;
                    push_times++;
                    push_times+=array_init_assignment(init_value,init_values_stack,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
                }
                else
                {
                    push_times+=assign_array_to_array_member((list<pair<bool,void * > > * )(init_values_stack.top()->front().second),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
                }
            }
            init_values_stack.top()->pop_front();
        }
        init_values_stack.pop();
    }
    while(push_times>=0)
    {
        push_times+=assign_var_to_array_member(def_const(array_data_type,0),init_value,dimensions_len_stack,dimensions_len,dimensions_len_pointer,array_data_type);
    }
    return push_times;
}

void delete_array_assignment(list<pair<bool,void * > > * array_assignment)
{
    if(array_assignment)
    {
        for(auto i:(*array_assignment))
        {
            if(i.first==false)
            {
                delete_array_assignment((list<pair<bool,void * > > *)i.second);
            }
        }
        delete array_assignment;
    }
}

/*
变量定义
*/
define_semantic_rule(___VAR_DEF_2___)
    struct ic_data * ic_data;
    list<struct ic_data * > * dimensions_len;
    list<struct ic_data * >::iterator dimensions_len_pointer;
    stack<size_t> dimensions_len_stack;
    stack<list<pair<bool,void * > > * > init_values_stack;
    list<pair<bool,void * > > *  array_assignment;
    vector<struct ic_data * > * array_init_value;
    if(check_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len))
    {
        //定义的变量是一个数组变量
        if(check_syntax_symbol_attribute(INIT_VAL,init_values))
        {
            array_init_value=new vector<struct ic_data * >;
            dimensions_len=(list<struct ic_data * > *)get_syntax_symbol_attribute(zero_or_more_CONST_EXP_surrounded_by_bracket,dimensions_len,pointer);
            dimensions_len_pointer=dimensions_len->begin();
            dimensions_len_stack.push(1);
            array_assignment=(list<pair<bool,void * > > *)get_syntax_symbol_attribute(INIT_VAL,init_values,pointer);
            init_values_stack.push(array_assignment);
            array_init_assignment(array_init_value,init_values_stack,dimensions_len_stack,dimensions_len,dimensions_len_pointer,get_global_info(data_type));
            delete_array_assignment(array_assignment);
            ic_data=def_array_with_init_value((*(string *)get_syntax_symbol_attribute(id,id_string,pointer)),get_global_info(data_type),dimensions_len,array_init_value);
        }
        else
        {
            print_error("Array initializer must be an initializer list");
        }
    }
    else
    {
        //定义的变量是一个普通变量
        if(check_syntax_symbol_attribute(INIT_VAL,value))
        {
            ic_data=def_var_with_init_value(*(string *)get_syntax_symbol_attribute(id,id_string,pointer),get_global_info(data_type),get_syntax_symbol_attribute(INIT_VAL,value,pointer));
        }
        else if(check_syntax_symbol_attribute(INIT_VAL,init_values))
        {
            print_error("Var is a normal ic_data while assigning with arrays!");
        }
        else
        {
            print_error("___VAR_DEF_2___ error!");
        }
    }
    set_syntax_symbol_attribute(VAR_DEF,var_def,pointer,ic_data);
end_define_semantic_rule

/*
变量初值
*/
define_semantic_rule(___INIT_VAL_1___)
    //需要将这些初始值转换成对应的数据类型
    struct ic_data * init_value_var=(struct ic_data *)get_syntax_symbol_attribute(EXP,value,pointer);
    enum language_data_type data_type=get_global_info(data_type);
    ic_data_type_conversion(init_value_var,data_type);
    set_syntax_symbol_attribute(INIT_VAL,value,pointer,init_value_var);
    //copy_syntax_symbol_attributes(INIT_VAL,EXP);
end_define_semantic_rule

/*
变量初值
*/
define_semantic_rule(___INIT_VAL_2___)
    list<pair<bool,void * > > * not_leaf=new list<pair<bool,void * > >;
    list<pair<bool,void * > > * tmp;
    if(check_syntax_symbol_attribute(zero_or_more_INIT_VAL_divided_with_comma,init_values))
    {
        //此时这个pair的first代表second是不是一个叶子结点（true为是），叶子结点就是一个struct ic_data *，非叶子结点就是一个list<pair<bool,void * > > *
        not_leaf->push_back(make_pair(false,(void * )get_syntax_symbol_attribute(zero_or_more_INIT_VAL_divided_with_comma,init_values,pointer)));
    }
    else
    {
        //如果pair<bool,void *>的first为false，但是second为nullptr，那么说明此时这个非叶子结点是一个空的{}
        tmp=new list<pair<bool,void * > >;
        tmp->push_back(make_pair(true,def_const(get_global_info(data_type),0)));
        not_leaf->push_back(make_pair(false,(void *)tmp));
    }
    set_syntax_symbol_attribute(INIT_VAL,init_values,pointer,not_leaf);
end_define_semantic_rule

/*
若干个变量初值
*/
define_semantic_rule(___zero_or_more_INIT_VAL_divided_with_comma_1___)
    //什么也不用做
end_define_semantic_rule

/*
若干个变量初值
*/
define_semantic_rule(___zero_or_more_INIT_VAL_divided_with_comma_2___)
    list<pair<bool,void * > > * init_values;
    if(check_syntax_symbol_attribute(INIT_VAL,value))
    {
        init_values=new list<pair<bool,void * > >;
        init_values->push_front(make_pair(true,(void *)get_syntax_symbol_attribute(INIT_VAL,value,pointer)));
        set_syntax_symbol_attribute(zero_or_more_INIT_VAL_divided_with_comma,init_values,pointer,init_values);
    }
    else if(check_syntax_symbol_attribute(INIT_VAL,init_values))
    {
        copy_syntax_symbol_attributes(zero_or_more_INIT_VAL_divided_with_comma,INIT_VAL);
    }
    else
    {
        print_error("___zero_or_more_INIT_VAL_divided_with_comma_2___ error!");
    }
end_define_semantic_rule

/*
若干个变量初值
*/
define_semantic_rule(___zero_or_more_INIT_VAL_divided_with_comma_3___)
    list<pair<bool,void * > > * init_values,* another_init_values;
    if(check_syntax_symbol_attribute_by_index(zero_or_more_INIT_VAL_divided_with_comma,init_values,1))
    {
        init_values=(list<pair<bool,void * > > *)get_syntax_symbol_attribute_by_index(zero_or_more_INIT_VAL_divided_with_comma,init_values,pointer,1);
    }
    else
    {
        init_values=new list<pair<bool,void * > >;
    }
    if(check_syntax_symbol_attribute(INIT_VAL,value))
    {
        init_values->push_front(make_pair(true,(void * )get_syntax_symbol_attribute(INIT_VAL,value,pointer)));
    }
    else if(check_syntax_symbol_attribute(INIT_VAL,init_values))
    {
        another_init_values=(list<pair<bool,void * > > *)get_syntax_symbol_attribute(INIT_VAL,init_values,pointer);
        init_values->insert(init_values->begin(),another_init_values->begin(),another_init_values->end());
        delete another_init_values;
    }
    else
    {
        print_error("___zero_or_more_INIT_VAL_divided_with_comma_3___ error!");
    }
    set_syntax_symbol_attribute_by_index(zero_or_more_INIT_VAL_divided_with_comma,init_values,pointer,0,init_values);
end_define_semantic_rule

/*
没有形参，返回值类型为基本类型的函数的定义
*/
define_semantic_rule(___FUNC_DEF_1___)
    //在符号表中进行注册
    struct ic_func * func=def_func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)),language_data_type((int)get_syntax_symbol_attribute(B_TYPE,b_type,int)),new list<struct ic_data * >);
    //set_global_info(current_func,func);
    //生成函数定义的中间代码
    gen_zero_operand_code(ic_op::FUNC_DEFINE,ic_operand::FUNC,func);
end_define_semantic_rule

/*
没有形参，返回值类型为基本类型的函数的定义
*/
define_semantic_rule(___FUNC_DEF_2___)
    struct ic_func * func=func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)));
    //检查BLOCK中是否有return
    //检查BLOCK中是否还存在有不匹配的break和continue
    //目前无需实现
    gen_zero_operand_code(ic_op::END_FUNC_DEFINE,ic_operand::FUNC,func);
    //set_global_info(current_func,nullptr);
end_define_semantic_rule

/*
有形参，返回值类型为基本类型的函数的定义
*/
define_semantic_rule(___FUNC_DEF_3___)
    //在符号表中进行注册
    struct ic_func * func=def_func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)),language_data_type((int)get_syntax_symbol_attribute(B_TYPE,b_type,int)),(list<struct ic_data * > *)get_syntax_symbol_attribute(FUNC_F_PARAMS,f_params,pointer));
    //set_global_info(current_func,func);
    //生成函数定义的中间代码
    gen_zero_operand_code(ic_op::FUNC_DEFINE,ic_operand::FUNC,func);
end_define_semantic_rule

/*
有形参，返回值类型为基本类型的函数的定义
*/
define_semantic_rule(___FUNC_DEF_4___)
    struct ic_func * func=func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)));
    //检查BLOCK中是否有return
    //检查BLOCK中是否还存在有不匹配的break和continue
    //目前无需实现
    gen_zero_operand_code(ic_op::END_FUNC_DEFINE,ic_operand::FUNC,func);
    //set_global_info(current_func,nullptr);
end_define_semantic_rule

/*
没有形参，返回值类型为void的函数的定义
*/
define_semantic_rule(___FUNC_DEF_5___)
    //在符号表中进行注册
    struct ic_func * func=def_func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)),language_data_type::VOID,new list<struct ic_data * >);
    //set_global_info(current_func,func);
    //生成函数定义的中间代码
    gen_zero_operand_code(ic_op::FUNC_DEFINE,ic_operand::FUNC,func);
end_define_semantic_rule

/*
没有形参，返回值类型为void的函数的定义
*/
define_semantic_rule(___FUNC_DEF_6___)
    //检查BLOCK中是否有return
    //检查BLOCK中是否还存在有不匹配的break和continue
    //目前无需实现
    //最后显式地增加一条ret
    gen_only_op_code(ic_op::RET);
    gen_zero_operand_code(ic_op::END_FUNC_DEFINE,ic_operand::FUNC,func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer))));
    //set_global_info(current_func,nullptr);
end_define_semantic_rule

/*
有形参，返回值类型为void的函数的定义
*/
define_semantic_rule(___FUNC_DEF_7___)
    //在符号表中进行注册
    struct ic_func * func=def_func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)),language_data_type::VOID,(list<struct ic_data * > *)get_syntax_symbol_attribute(FUNC_F_PARAMS,f_params,pointer));
    //set_global_info(current_func,func);
    //生成函数定义的中间代码
    gen_zero_operand_code(ic_op::FUNC_DEFINE,ic_operand::FUNC,func);
end_define_semantic_rule

/*
有形参，返回值类型为void的函数的定义
*/
define_semantic_rule(___FUNC_DEF_8___)
    //检查BLOCK中是否有return
    //检查BLOCK中是否还存在有不匹配的break和continue
    //目前无需实现
    //最后显式地增加一条ret
    gen_only_op_code(ic_op::RET);
    gen_zero_operand_code(ic_op::END_FUNC_DEFINE,ic_operand::FUNC,func(*((string *)get_syntax_symbol_attribute(id,id_string,pointer))));
    //set_global_info(current_func,nullptr);
end_define_semantic_rule

/*
函数形参列表定义
*/
define_semantic_rule(___FUNC_F_PARAMS___)
    //把语义符号one_or_more_FUNC_F_PARAM_divided_with_comma中存储的形参列表复制给FUNC_F_PARAMS即可
    copy_syntax_symbol_attributes(FUNC_F_PARAMS,one_or_more_FUNC_F_PARAM_divided_with_comma);
end_define_semantic_rule

/*
单个函数形参定义
*/
define_semantic_rule(___one_or_more_FUNC_F_PARAM_divided_with_comma_1___)
    //创建一个函数形参列表
    list<struct ic_data * > * f_params=new list<struct ic_data * >;
    struct ic_data * f_param;
    //这个函数形参列表目前只有一个形参
    f_param=((struct ic_data *)get_syntax_symbol_attribute(FUNC_F_PARAM,f_param,pointer));
    f_params->push_front(f_param);
    //将这个函数形参列表传递下去
    set_syntax_symbol_attribute(one_or_more_FUNC_F_PARAM_divided_with_comma,f_params,pointer,f_params);
end_define_semantic_rule

/*
多个函数形参定义
*/
define_semantic_rule(___one_or_more_FUNC_F_PARAM_divided_with_comma_2___)
    //获取函数形参列表
    list<struct ic_data * > * f_params=(list<struct ic_data * > * )get_syntax_symbol_attribute_by_index(one_or_more_FUNC_F_PARAM_divided_with_comma,f_params,pointer,1);
    struct ic_data * f_param=((struct ic_data *)get_syntax_symbol_attribute(FUNC_F_PARAM,f_param,pointer));
    //检查函数形参中是否有重复的声明
    //目前无需进行
    //将该形参加入函数形参列表
    f_params->push_front(f_param);
    //将这个函数形参列表传递下去
    set_syntax_symbol_attribute_by_index(one_or_more_FUNC_F_PARAM_divided_with_comma,f_params,pointer,0,f_params);
end_define_semantic_rule

/*
基本类型函数形参定义
*/
define_semantic_rule(___FUNC_F_PARAM_1___)
    //在符号表中注册一个函数形参（此时这个形参属于哪一个函数还是不知道的，因为函数定义的产生式还没有被归约出来），并获取该形参在符号表中的位置的指针
    struct ic_data * f_param=def_f_param(*((string *)(get_syntax_symbol_attribute(id,id_string,pointer))),language_data_type((int)get_syntax_symbol_attribute(B_TYPE,b_type,int)));
    if(!f_param)
    {
        //如果获取函数形参失败，那么说明参数定义重复了
        print_error("Func format parameter has been defined!");
    }
    //把这个指针赋值给FUNC_F_PARAM这个语义符号作为属性
    set_syntax_symbol_attribute(FUNC_F_PARAM,f_param,pointer,f_param);
end_define_semantic_rule

/*
数组函数形参定义
*/
define_semantic_rule(___FUNC_F_PARAM_2___)
    struct ic_data * f_param;
    list<struct ic_data * > * dimensions_len;
    if(check_syntax_symbol_attribute(zero_or_more_EXP_surrounded_by_bracket,dimensions_len))
    {
        //函数的数组形参维度大于一维
        dimensions_len=(list<struct ic_data * > *)get_syntax_symbol_attribute(zero_or_more_EXP_surrounded_by_bracket,dimensions_len,pointer);
    }
    else
    {
        //函数的数组形参维度只有一维
        dimensions_len=new list<struct ic_data * >;
    }
    dimensions_len->push_front(def_const(language_data_type::INT,0));
    //在符号表中注册一个函数形参（此时这个形参属于哪一个函数还是不知道的，因为函数定义的产生式还没有被归约出来），并获取该形参在符号表中的位置的指针
    f_param=def_array_f_param(*((string *)(get_syntax_symbol_attribute(id,id_string,pointer))),language_data_type((int)get_syntax_symbol_attribute(B_TYPE,b_type,int)),dimensions_len);
    if(!f_param)
    {
        //如果获取函数形参失败，那么说明参数定义重复了
        print_error("Func format parameter has been defined!");
    }
    //把这个指针赋值给FUNC_F_PARAM这个语义符号作为属性
    set_syntax_symbol_attribute(FUNC_F_PARAM,f_param,pointer,f_param);
end_define_semantic_rule

/*
数组取元素,也有可能是函数的数组形参定义数组维度
*/
define_semantic_rule(___zero_or_more_EXP_surrounded_by_bracket_1___)
end_define_semantic_rule

/*
数组取元素,也有可能是函数的数组形参定义数组维度
*/
define_semantic_rule(___zero_or_more_EXP_surrounded_by_bracket_2___)
    list<struct ic_data * > * dimensions_len;
    struct ic_data * dimension_len_var;
    if(check_syntax_symbol_attribute_by_index(zero_or_more_EXP_surrounded_by_bracket,dimensions_len,1))
    {
        //如果此时第二个zero_or_more_EXP_surrounded_by_bracket语义符号有dimensions_len这个属性的话，就说明之前已经有数组维度了
        dimensions_len=(list<struct ic_data * > *)get_syntax_symbol_attribute_by_index(zero_or_more_EXP_surrounded_by_bracket,dimensions_len,pointer,1);
    }
    else
    {
        //如果之前没有数组维度的话，说明此时是数组的第一个维度
        dimensions_len=new list<struct ic_data * >;
    }
    //把数组维度增加一维，并向下传递
    //数组维度一定要是int类型的
    dimension_len_var=(struct ic_data *)get_syntax_symbol_attribute(EXP,value,pointer);
    ic_data_type_conversion(dimension_len_var,language_data_type::INT);
    dimensions_len->push_front(dimension_len_var);
    set_syntax_symbol_attribute_by_index(zero_or_more_EXP_surrounded_by_bracket,dimensions_len,pointer,0,dimensions_len);
end_define_semantic_rule

/*
语句块产生式
*/
define_semantic_rule(___BLOCK_1___)
    def_scope();
end_define_semantic_rule

/*
语句块产生式
*/
define_semantic_rule(___BLOCK_2___)
    struct ic_scope * block_scope=end_def_scope();
    copy_syntax_symbol_attributes(BLOCK,zero_or_more_BLOCK_ITEM);
    if(block_scope!=nullptr)
    {
        set_syntax_symbol_attribute(BLOCK,block_scope,pointer,block_scope);
    }
end_define_semantic_rule

/*
零个block item
*/
define_semantic_rule(___zero_or_more_BLOCK_ITEM_1___)
end_define_semantic_rule

/*
多个block item
*/
define_semantic_rule(___zero_or_more_BLOCK_ITEM_2___)
    vector<list<struct quaternion>::iterator> * break_backpatching_codes,* break_backpatching_codes_tmp,* continue_backpatching_codes,* continue_backpatching_codes_tmp;
    //合并需要回填的break和continue
    if(check_syntax_symbol_attribute(BLOCK_ITEM,break_backpatching_codes))
    {
        break_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(BLOCK_ITEM,break_backpatching_codes,pointer);
        if(check_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,break_backpatching_codes,1))
        {
            break_backpatching_codes_tmp=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,break_backpatching_codes,pointer,1);
            break_backpatching_codes->insert(break_backpatching_codes->end(),break_backpatching_codes_tmp->begin(),break_backpatching_codes_tmp->end());
            delete break_backpatching_codes_tmp;
        }
    }
    else
    {
        if(check_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,break_backpatching_codes,1))
        {
            break_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,break_backpatching_codes,pointer,1);
        }
        else
        {
            break_backpatching_codes=nullptr;
        }
    }
    if(break_backpatching_codes)
    {
        set_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,break_backpatching_codes,pointer,0,break_backpatching_codes);
    }
    if(check_syntax_symbol_attribute(BLOCK_ITEM,continue_backpatching_codes))
    {
        continue_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(BLOCK_ITEM,continue_backpatching_codes,pointer);
        if(check_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,continue_backpatching_codes,1))
        {
            continue_backpatching_codes_tmp=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,continue_backpatching_codes,pointer,1);
            continue_backpatching_codes->insert(continue_backpatching_codes->end(),continue_backpatching_codes_tmp->begin(),continue_backpatching_codes_tmp->end());
            delete continue_backpatching_codes_tmp;
        }
    }
    else
    {
        if(check_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,continue_backpatching_codes,1))
        {
            continue_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,continue_backpatching_codes,pointer,1);
        }
        else
        {
            continue_backpatching_codes=nullptr;
        }
    }
    if(continue_backpatching_codes)
    {
        set_syntax_symbol_attribute_by_index(zero_or_more_BLOCK_ITEM,continue_backpatching_codes,pointer,0,continue_backpatching_codes);
    }
end_define_semantic_rule

/*
变量声明
*/
define_semantic_rule(___BLOCK_ITEM_1___)
    copy_syntax_symbol_attributes(BLOCK_ITEM,DECL);
end_define_semantic_rule

/*
语句
*/
define_semantic_rule(___BLOCK_ITEM_2___)
    struct ic_scope * block_scope;
    if(check_syntax_symbol_attribute(STMT,block_scope))
    {
        block_scope=(struct ic_scope * )get_syntax_symbol_attribute(STMT,block_scope,pointer);
        block_scope->set_scope_type(ic_scope_type::ANONYMOUS);
        delete_syntax_symbol_attribute(STMT,block_scope);
    }
    copy_syntax_symbol_attributes(BLOCK_ITEM,STMT);
end_define_semantic_rule

/*
一个block作为一条语句
*/
define_semantic_rule(___STMT_1___)
    copy_syntax_symbol_attributes(STMT,BLOCK);
end_define_semantic_rule

/*
赋值语句
*/
define_semantic_rule(___STMT_2___)
    struct ic_data * ic_data=(struct ic_data *)get_syntax_symbol_attribute(L_VAL,value,pointer),* arg1_or_result_value=(struct ic_data * )get_syntax_symbol_attribute(EXP,value,pointer);
    //生成赋值的中间代码
    gen_one_operand_code(ic_op::ASSIGN,ic_operand::DATA,arg1_or_result_value,ic_operand::DATA,ic_data);
end_define_semantic_rule

/*
空语句
*/
define_semantic_rule(___STMT_3___)
    //啥也不干
end_define_semantic_rule

/*
表达式语句
*/
define_semantic_rule(___STMT_4___)
    //啥也不干
end_define_semantic_rule

/*
if语句
*/
define_semantic_rule(___STMT_5___)
    copy_syntax_symbol_attributes(STMT,IF_STMT);
end_define_semantic_rule

/*
while语句
*/
define_semantic_rule(___STMT_6___)
    copy_syntax_symbol_attributes(STMT,WHILE_STMT);
end_define_semantic_rule

/*
跳转
*/
define_semantic_rule(___STMT_7___)
    copy_syntax_symbol_attributes(STMT,JMP_STMT);
end_define_semantic_rule

/*
break语句
*/
define_semantic_rule(___JMP_STMT_1___)
    vector<list<struct quaternion>::iterator> * break_backpatching_codes;
    //生成待回填的无条件跳转
    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
    break_backpatching_codes=new vector<list<struct quaternion>::iterator>;
    break_backpatching_codes->push_back(generated_code_iterator());
    set_syntax_symbol_attribute(JMP_STMT,break_backpatching_codes,pointer,break_backpatching_codes);
end_define_semantic_rule

/*
continue语句
*/
define_semantic_rule(___JMP_STMT_2___)
    vector<list<struct quaternion>::iterator> * continue_backpatching_codes;
    //生成待回填的无条件跳转
    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
    continue_backpatching_codes=new vector<list<struct quaternion>::iterator>;
    continue_backpatching_codes->push_back(generated_code_iterator());
    set_syntax_symbol_attribute(JMP_STMT,continue_backpatching_codes,pointer,continue_backpatching_codes);
end_define_semantic_rule

/*
没有返回值的return语句
*/
define_semantic_rule(___JMP_STMT_3___)
    //生成返回语句
    gen_only_op_code(ic_op::RET);
end_define_semantic_rule

/*
有返回值的return语句
*/
define_semantic_rule(___JMP_STMT_4___)
    struct ic_data * ret_value=(struct ic_data *)get_syntax_symbol_attribute(EXP,value,pointer);
    enum language_data_type func_ret_type=get_current_func()->return_type;
    //检查是否需要类型转换
    ic_data_type_conversion(ret_value,func_ret_type);
    //生成返回语句
    gen_zero_operand_code(ic_op::RET,ic_operand::DATA,ret_value);
end_define_semantic_rule

/*
while语句
*/
define_semantic_rule(___WHILE_STMT_1___)
    struct ic_label * loop_label;
    //生成并定义一个新的while循环跳转标签
    loop_label=def_label();
    gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,loop_label);
    //把循环跳转的标签赋值给WHILE_STMT语义符号作为语义属性
    set_syntax_symbol_attribute(while,loop_label,pointer,loop_label);
end_define_semantic_rule

/*
while语句
*/
define_semantic_rule(___WHILE_STMT_2___)
    struct ic_label * loop_label,* out_label;
    vector<list<struct quaternion>::iterator> * backpatching_codes;
    struct ic_scope * block_scope;
    //跳转到循环到开头
    loop_label=(struct ic_label *)get_syntax_symbol_attribute(while,loop_label,pointer);
    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,loop_label);
    //定义并生成退出while的标签
    out_label=def_label();
    gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,out_label);
    //回填COND
    backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(COND,backpatching_codes,pointer);
    for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
    {
        (*(*i)).result.second=(void *)out_label;
    }
    delete backpatching_codes;
    //回填continue
    if(check_syntax_symbol_attribute(STMT,continue_backpatching_codes))
    {
        backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(STMT,continue_backpatching_codes,pointer);
        for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
        {
            (*(*i)).result.second=(void *)loop_label;
        }
        delete backpatching_codes;
        delete_syntax_symbol_attribute(STMT,continue_backpatching_codes);
    }
    //回填break
    if(check_syntax_symbol_attribute(STMT,break_backpatching_codes))
    {
        backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(STMT,break_backpatching_codes,pointer);
        for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
        {
            (*(*i)).result.second=(void *)out_label;
        }
        delete backpatching_codes;
        delete_syntax_symbol_attribute(STMT,break_backpatching_codes);
    }
    //把作用域设置为while作用域
    if(check_syntax_symbol_attribute(STMT,block_scope))
    {
        block_scope=(struct ic_scope * )get_syntax_symbol_attribute(STMT,block_scope,pointer);
        block_scope->set_scope_type(ic_scope_type::WHILE);
        delete_syntax_symbol_attribute(STMT,block_scope);
    }
    //把STMT的语义属性复制一份给WHILE_STMT，这里主要是为了复制在该while语句块中定义的变量
    copy_syntax_symbol_attributes(WHILE_STMT,STMT);
end_define_semantic_rule

/*
if语句中的if-else语句
*/
define_semantic_rule(___IF_STMT_1___)
    vector<list<struct quaternion>::iterator> * backpatching_codes;
    list<struct quaternion>::iterator ic_generated;
    struct ic_label * false_label;
    struct ic_scope * block_scope;
    //生成一个无条件跳转的语句，跳转到整个if-else语句的结尾
    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
    ic_generated=generated_code_iterator();
    //将其加入需要回填的中间代码中
    //生成并定义一个新的标签
    false_label=def_label();
    gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,false_label);
    if(check_syntax_symbol_attribute(COND,backpatching_codes))
    {
        backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(COND,backpatching_codes,pointer);
    }
    else
    {
        print_error("___IF_STMT_1___ error!");
    }
    //进行回填
    for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
    {
        (*(*i)).result.second=(void *)false_label;
    }
    backpatching_codes->clear();
    backpatching_codes->push_back(ic_generated);
    //下面的两行优先选择第二行，因为第一行（已被注释掉）无法在windows g++中使用（虽然第一行的可读性更好）
    //set_syntax_symbol_attribute(else,backpatching_codes,pointer,backpatching_codes);
    set_syntax_symbol_attribute_by_index(STMT,backpatching_codes,pointer,0,backpatching_codes);
    //把作用域设置为if作用域
    if(check_syntax_symbol_attribute_by_index(STMT,block_scope,0))
    {
        block_scope=(struct ic_scope * )get_syntax_symbol_attribute_by_index(STMT,block_scope,pointer,0);
        block_scope->set_scope_type(ic_scope_type::IF);
    }
end_define_semantic_rule

/*
if语句中的if-else语句
*/
define_semantic_rule(___IF_STMT_2___)
    vector<list<struct quaternion>::iterator> * backpatching_codes,* continue_backpatching_codes,* break_backpatching_codes,* tmp_backpatching_codes;
    struct ic_label * true_label;
    struct ic_scope * block_scope;
    //生成并定义一个退出整个if-else语句的标签
    true_label=def_label();
    gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,true_label);
    //进行回填
    //下面的三行优先选择第二和第三行，因为第一行（已被注释掉）无法在windows g++中使用（虽然第一行的可读性更好）
    //backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(else,backpatching_codes,pointer);
    backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,backpatching_codes,pointer,0);
    delete_syntax_symbol_attribute_by_index(STMT,backpatching_codes,0);
    for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
    {
        (*(*i)).result.second=(void *)true_label;
    }
    delete backpatching_codes;
    //删除相关的语义属性
    //把两个STMT中的那些需要进行回填的中间代码(continue和break)赋值给IF_STMT
    if(check_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,0))
    {
        continue_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,pointer,0);
        if(check_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,1))
        {
            tmp_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,pointer,1);
            continue_backpatching_codes->insert(continue_backpatching_codes->end(),tmp_backpatching_codes->begin(),tmp_backpatching_codes->end());
            delete tmp_backpatching_codes;
        }
    }
    else
    {
        if(check_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,1))
        {
            continue_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,continue_backpatching_codes,pointer,1);
        }
        else
        {
            continue_backpatching_codes=nullptr;
        }
    }
    if(continue_backpatching_codes)
    {
        set_syntax_symbol_attribute(IF_STMT,continue_backpatching_codes,pointer,continue_backpatching_codes);
    }
    if(check_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,0))
    {
        break_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,pointer,0);
        if(check_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,1))
        {
            tmp_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,pointer,1);
            break_backpatching_codes->insert(break_backpatching_codes->end(),tmp_backpatching_codes->begin(),tmp_backpatching_codes->end());
            delete tmp_backpatching_codes;
        }
    }
    else
    {
        if(check_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,1))
        {
            break_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(STMT,break_backpatching_codes,pointer,1);
        }
        else
        {
            break_backpatching_codes=nullptr;
        }
    }
    if(break_backpatching_codes)
    {
        set_syntax_symbol_attribute(IF_STMT,break_backpatching_codes,pointer,break_backpatching_codes);
    }
    //把作用域设置为else作用域
    if(check_syntax_symbol_attribute_by_index(STMT,block_scope,1))
    {
        block_scope=(struct ic_scope * )get_syntax_symbol_attribute_by_index(STMT,block_scope,pointer,1);
        block_scope->set_scope_type(ic_scope_type::ELSE);
    }
end_define_semantic_rule

/*
if语句中只有if的语句
*/
define_semantic_rule(___IF_STMT_3___)
    vector<list<struct quaternion>::iterator> * backpatching_codes;
    struct ic_label * label;
    struct ic_scope * block_scope;
    //生成并定义一个新的标签
    label=def_label();
    gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,label);
    if(check_syntax_symbol_attribute(COND,backpatching_codes))
    {
        backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(COND,backpatching_codes,pointer);
    }
    else
    {
        print_error("___IF_STMT_3___ error!");
    }
    //进行回填
    for(vector<list<struct quaternion>::iterator>::iterator i=backpatching_codes->begin();i!=backpatching_codes->end();i++)
    {
        (*(*i)).result.second=(void *)label;
    }
    delete backpatching_codes;
    //把作用域设置为if作用域
    if(check_syntax_symbol_attribute(STMT,block_scope))
    {
        block_scope=(struct ic_scope * )get_syntax_symbol_attribute(STMT,block_scope,pointer);
        block_scope->set_scope_type(ic_scope_type::IF);
        delete_syntax_symbol_attribute(STMT,block_scope);
    }
    //把STMT的语义属性复制一份给IF_STMT，这里主要是为了复制需要进行回填的中间代码(continue和break)以及在该if语句块中定义的变量
    copy_syntax_symbol_attributes(IF_STMT,STMT);
end_define_semantic_rule

/*
表达式中的加法表达式
*/
define_semantic_rule(___EXP___)
    copy_syntax_symbol_attributes(EXP,ADD_EXP);
end_define_semantic_rule

/*
条件表达式
这里我们暂时规定条件表达式中需要进行回填的跳转中间代码一定是结果为false的时候的跳转
*/
define_semantic_rule(___COND___)
    vector<list<struct quaternion>::iterator> * backpatching_codes=nullptr,* new_or_backpatching_codes,* old_or_backpatching_codes;
    struct ic_label * true_label;
    struct ic_data * ic_data;
    if(check_syntax_symbol_attribute(L_OR_EXP,and_backpatching_codes))
    {
        set_syntax_symbol_attribute(COND,backpatching_codes,pointer,get_syntax_symbol_attribute(L_OR_EXP,and_backpatching_codes,pointer));
    }
    else if(check_syntax_symbol_attribute(L_OR_EXP,or_backpatching_codes))
    {
        //生成一条待跳转的OR表达式为false的时候的无条件跳转语句
        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
        new_or_backpatching_codes=new vector<list<struct quaternion>::iterator>;
        new_or_backpatching_codes->push_back(generated_code_iterator());
        //生成并定义一个当OR表达式中的某一项是true的时候跳转到的标签
        true_label=def_label();
        gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,true_label);
        old_or_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(L_OR_EXP,or_backpatching_codes,pointer);
        //回填
        for(vector<list<struct quaternion>::iterator>::iterator i=old_or_backpatching_codes->begin();i!=old_or_backpatching_codes->end();i++)
        {
            (*(*i)).result.second=(void *)true_label;
        }
        delete old_or_backpatching_codes;
        //把新的待回填的中间代码赋值给COND
        set_syntax_symbol_attribute(COND,backpatching_codes,pointer,new_or_backpatching_codes);
    }
    else
    {
        backpatching_codes=new vector<list<struct quaternion>::iterator>;
        ic_data=(struct ic_data *)get_syntax_symbol_attribute(L_OR_EXP,value,pointer);
        if(ic_data->is_const() && !ic_data->is_array_var())
        {
            switch(ic_data->get_data_type())
            {
                case language_data_type::INT:
                    if(!ic_data->get_value().int_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                case language_data_type::FLOAT:
                    if(!ic_data->get_value().float_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            gen_one_operand_code(ic_op::IF_NOT_JMP,ic_operand::DATA,ic_data,ic_operand::LABEL,0);
            backpatching_codes->push_back(generated_code_iterator());
        }
        set_syntax_symbol_attribute(COND,backpatching_codes,pointer,backpatching_codes);
    }
end_define_semantic_rule

/*
左值表达式
*/
define_semantic_rule(___L_VAL___)
    struct ic_data * array_var,* array_member,* arg_0_for_MLA,* arg_1_for_MLA,* res_for_MLA,* arg_0_for_MUL,* arg_1_for_MUL,* res_for_MUL,* tmp_res_for_MLA;
    list<struct ic_data * > * dimensions_len,* array_get_member_offset;
    size_t define_dimension_num,array_get_member_dimension_num,difference_dimension_num;
    list<struct ic_data * >::reverse_iterator array_get_value_pointer;
    if(!check_syntax_symbol_attribute(zero_or_more_EXP_surrounded_by_bracket,dimensions_len))
    {
        //如果此时的左值是一个变量
        set_syntax_symbol_attribute(L_VAL,value,pointer,var(*((string *)get_syntax_symbol_attribute(id,id_string,pointer))));
    }
    else
    {
        //如果此时的左值是一个数组取元素
        array_var=var(*((string *)get_syntax_symbol_attribute(id,id_string,pointer)));
        dimensions_len=array_var->dimensions_len;
        array_get_member_offset=(list<struct ic_data * > * )get_syntax_symbol_attribute(zero_or_more_EXP_surrounded_by_bracket,dimensions_len,pointer);
        if(!array_var->is_array_var())
        {
            //如果数组取元素的变量并不是数组变量，那就报错
            print_error("Var is not an array!");
        }
        //获取数组定义的时候的维度数
        define_dimension_num=dimensions_len->size();
        //获取数组取元素的时候的维度数量
        array_get_member_dimension_num=array_get_member_offset->size();
        //如果数组使用的时候维度数量比定义的时候的维度数量更多，那么直接出错
        if(array_get_member_dimension_num>define_dimension_num)
        {
            print_error("Array dimensions is not matched while getting array value!");
        }
        difference_dimension_num=define_dimension_num-array_get_member_dimension_num;
        //开始计算数组取元素的偏移量
        res_for_MLA=def_const(language_data_type::INT,0);
        res_for_MUL=def_const(language_data_type::INT,1);
        array_get_value_pointer=array_get_member_offset->rbegin();
        for(list<struct ic_data * >::reverse_iterator i=dimensions_len->rbegin();i!=dimensions_len->rend();i++)
        {
            if(difference_dimension_num!=0)
            {
                difference_dimension_num--;
            }
            else
            {
                if((*array_get_value_pointer)->is_const() && res_for_MUL->is_const())
                {
                    tmp_res_for_MLA=def_const(language_data_type::INT,(*array_get_value_pointer)->get_value().int_data*res_for_MUL->get_value().int_data);
                }
                else
                {
                    tmp_res_for_MLA=def_tmp_var(language_data_type::INT);
                    gen_two_operands_code(ic_op::MUL,ic_operand::DATA,(*array_get_value_pointer),ic_operand::DATA,res_for_MUL,ic_operand::DATA,tmp_res_for_MLA);
                }

                if(tmp_res_for_MLA->is_const() && res_for_MLA->is_const())
                {
                    res_for_MLA=def_const(language_data_type::INT,tmp_res_for_MLA->get_value().int_data+res_for_MLA->get_value().int_data);
                }
                else
                {
                    arg_0_for_MLA=tmp_res_for_MLA;
                    arg_1_for_MLA=res_for_MLA;
                    res_for_MLA=def_tmp_var(language_data_type::INT);
                    gen_two_operands_code(ic_op::ADD,ic_operand::DATA,arg_0_for_MLA,ic_operand::DATA,arg_1_for_MLA,ic_operand::DATA,res_for_MLA);
                }
                array_get_value_pointer++;
            }

            if(res_for_MUL->is_const() && (*i)->is_const())
            {
                res_for_MUL=def_const(language_data_type::INT,(*i)->get_value().int_data*res_for_MUL->get_value().int_data);
            }
            else
            {
                arg_0_for_MUL=res_for_MUL;
                arg_1_for_MUL=(*i);
                res_for_MUL=def_tmp_var(language_data_type::INT);
                gen_two_operands_code(ic_op::MUL,ic_operand::DATA,arg_0_for_MUL,ic_operand::DATA,arg_1_for_MUL,ic_operand::DATA,res_for_MUL);
            }
        }
        array_member=def_array_member(array_var,array_get_member_dimension_num,res_for_MLA);
        set_syntax_symbol_attribute(L_VAL,value,pointer,array_member);
    }
end_define_semantic_rule

/*
基本表达式中的表达式
*/
define_semantic_rule(___PRIMARY_EXP_1___)
    copy_syntax_symbol_attributes(PRIMARY_EXP,EXP);
end_define_semantic_rule

/*
基本表达式中的左值
*/
define_semantic_rule(___PRIMARY_EXP_2___)
    copy_syntax_symbol_attributes(PRIMARY_EXP,L_VAL);
end_define_semantic_rule

/*
基本表达式中的数字
*/
define_semantic_rule(___PRIMARY_EXP_3___)
    copy_syntax_symbol_attributes(PRIMARY_EXP,NUMBER);
end_define_semantic_rule

/*
int常数
*/
define_semantic_rule(___NUMBER_1___)
    set_syntax_symbol_attribute(NUMBER,value,pointer,def_const(language_data_type::INT,get_syntax_symbol_attribute(const_int,int_value,int)));
end_define_semantic_rule

/*
float常数
*/
define_semantic_rule(___NUMBER_2___)
    set_syntax_symbol_attribute(NUMBER,value,pointer,def_const(language_data_type::FLOAT,get_syntax_symbol_attribute(const_float,float_value,float)));
end_define_semantic_rule

/*
一元表达式中的基本表达式
*/
define_semantic_rule(___UNARY_EXP_1___)
    //只需要把PRIMARY_EXP中的所有属性都复制一份给UNARY_EXP即可
    copy_syntax_symbol_attributes(UNARY_EXP,PRIMARY_EXP);
end_define_semantic_rule

/*
一元表达式中的没有实参的函数调用
*/
define_semantic_rule(___UNARY_EXP_2___)
    struct ic_data * tmp_var;
    struct ic_func * func=func(*(string*)get_syntax_symbol_attribute(id,id_string,pointer));
    //这里目前不需要检查要调用的函数是否存在，因为可能有函数位于动态共享库中
    list<struct ic_data * > * r_params=new list<struct ic_data * >;
    if(func->return_type==language_data_type::VOID)
    {
        gen_two_operands_without_result_code(ic_op::CALL,ic_operand::FUNC,func,ic_operand::DATAS,r_params);
        //把该函数调用没有返回值的信息传递给UNARY_EXP
        set_syntax_symbol_attribute(UNARY_EXP,void_return_func_result,bool,true);
    }
    else
    {
        //生成临时变量来存储函数调用的结果
        tmp_var=def_tmp_var(func->return_type);
        gen_two_operands_code(ic_op::CALL,ic_operand::FUNC,func,ic_operand::DATAS,r_params,ic_operand::DATA,tmp_var);
        //把这个接收函数返回值的临时变量传递给UNARY_EXP
        set_syntax_symbol_attribute(UNARY_EXP,value,pointer,tmp_var);
    }
end_define_semantic_rule

/*
一元表达式中的有实参的函数调用
*/
define_semantic_rule(___UNARY_EXP_3___)
    struct ic_data * tmp_var;
    struct ic_func * func=func(*(string*)get_syntax_symbol_attribute(id,id_string,pointer));
    list<struct ic_data * > * r_params=(list<struct ic_data * > * )get_syntax_symbol_attribute(FUNC_R_PARAMS,r_params,pointer);
    list<struct ic_data * >::iterator r_param_pointer=r_params->begin();
    list<struct ic_data * > f_params=*(func->f_params);
    //检查函数形参类型和实参类型是否一致，如果不一致的话需要进行转换
    for(auto i:f_params)
    {
        if(!i->is_array_var())
        {
            ic_data_type_conversion(*r_param_pointer,i->get_data_type());
        }
        r_param_pointer++;
    }
    //生成函数调用语句
    if(func->return_type==language_data_type::VOID)
    {
        gen_two_operands_without_result_code(ic_op::CALL,ic_operand::FUNC,func,ic_operand::DATAS,r_params);
        //把该函数调用没有返回值的信息传递给UNARY_EXP
        set_syntax_symbol_attribute(UNARY_EXP,void_return_func_result,bool,true);
    }
    else
    {
        //生成临时变量来存储函数调用的结果
        tmp_var=def_tmp_var(func->return_type);
        gen_two_operands_code(ic_op::CALL,ic_operand::FUNC,func,ic_operand::DATAS,r_params,ic_operand::DATA,tmp_var);
        //把这个接收函数返回值的临时变量传递给UNARY_EXP
        set_syntax_symbol_attribute(UNARY_EXP,value,pointer,tmp_var);
    }
end_define_semantic_rule

/*
一元表达式中的前面有一元运算符的一元表达式
*/
define_semantic_rule(___UNARY_EXP_4___)
    //生成一个临时变量来接收一元表达式的返回值
    struct ic_data * res,* arg;
    enum language_data_type right_unary_exp_type;
    if(ic_op(get_syntax_symbol_attribute(UNARY_OP,unary_op,int))==ic_op::ADD)
    {
        copy_syntax_symbol_attribute_by_index(UNARY_EXP,0,UNARY_EXP,1);
    }
    else
    {
        arg=((struct ic_data *)get_syntax_symbol_attribute_by_index(UNARY_EXP,value,pointer,1));
        right_unary_exp_type=arg->data_type;
        if(!arg->is_const())
        {
            //结果不是一个常数
            res=def_tmp_var(right_unary_exp_type);
            //生成中间代码
            switch(ic_op(get_syntax_symbol_attribute(UNARY_OP,unary_op,int)))
            {
                case ic_op::SUB:
                    gen_two_operands_code(ic_op::SUB,ic_operand::DATA,def_const(right_unary_exp_type,0),ic_operand::DATA,arg,ic_operand::DATA,res);
                    break;
                case ic_op::NOT:
                    gen_one_operand_code(ic_op::NOT,ic_operand::DATA,arg,ic_operand::DATA,res);
                    break;
                default:
                    print_error("___UNARY_EXP_4___ ERROR!");
                    break;
            }
            //把这个接收一元表达式返回值的临时变量传递给UNARY_EXP
            set_syntax_symbol_attribute_by_index(UNARY_EXP,value,pointer,0,res);
        }
        else
        {
            //结果是一个常数
            set_syntax_symbol_attribute_by_index(UNARY_EXP,value,pointer,0,(void *)unary_compute(ic_op(get_syntax_symbol_attribute(UNARY_OP,unary_op,int)),arg));
        }
    }
end_define_semantic_rule

/*
单目运算符加号
*/
define_semantic_rule(___UNARY_OP_1___)
    set_syntax_symbol_attribute(UNARY_OP,unary_op,int,(int)ic_op::ADD);
end_define_semantic_rule

/*
单目运算符减号
*/
define_semantic_rule(___UNARY_OP_2___)
    set_syntax_symbol_attribute(UNARY_OP,unary_op,int,(int)ic_op::SUB);
end_define_semantic_rule

/*
单目运算符取非
*/
define_semantic_rule(___UNARY_OP_3___)
    set_syntax_symbol_attribute(UNARY_OP,unary_op,int,(int)ic_op::NOT);
end_define_semantic_rule

/*
单个函数实参
*/
define_semantic_rule(___FUNC_R_PARAMS_1___)
    //设置函数实参列表，将第一个实参加入其中
    list<struct ic_data * > * r_params=new list<struct ic_data * >;
    r_params->push_front((struct ic_data *)get_syntax_symbol_attribute(EXP,value,pointer));
    //将实参列表传递给FUNC_R_PARAMS
    set_syntax_symbol_attribute(FUNC_R_PARAMS,r_params,pointer,r_params);
end_define_semantic_rule

/*
多个函数实参
*/
define_semantic_rule(___FUNC_R_PARAMS_2___)
    //获取产生式右部的实参列表
    list<struct ic_data * > * r_params=(list<struct ic_data * > *)get_syntax_symbol_attribute_by_index(FUNC_R_PARAMS,r_params,pointer,1);
    //将新的实参加入实参列表
    r_params->push_front((struct ic_data *)get_syntax_symbol_attribute(EXP,value,pointer));
    //将实参列表传递左部的FUNC_R_PARAMS
    set_syntax_symbol_attribute_by_index(FUNC_R_PARAMS,r_params,pointer,0,r_params);
end_define_semantic_rule

/*
乘除模表达式中的直接赋值
*/
define_semantic_rule(___MUL_EXP_1___)
    copy_syntax_symbol_attributes(MUL_EXP,UNARY_EXP);
end_define_semantic_rule

/*
乘除模表达式中的乘法
*/
binary_compute_semantic_rule(___MUL_EXP_2___,ic_op::MUL,MUL_EXP,UNARY_EXP)

/*
乘除模表达式中的除法
*/
binary_compute_semantic_rule(___MUL_EXP_3___,ic_op::DIV,MUL_EXP,UNARY_EXP)

/*
乘除模表达式中的模
*/
binary_compute_semantic_rule(___MUL_EXP_4___,ic_op::MOD,MUL_EXP,UNARY_EXP)

/*
加减表达式中的直接赋值
*/
define_semantic_rule(___ADD_EXP_1___)
    copy_syntax_symbol_attributes(ADD_EXP,MUL_EXP);
end_define_semantic_rule

/*
加减表达式中的加
*/
binary_compute_semantic_rule(___ADD_EXP_2___,ic_op::ADD,ADD_EXP,MUL_EXP)

/*
加减表达式中的减
*/
binary_compute_semantic_rule(___ADD_EXP_3___,ic_op::SUB,ADD_EXP,MUL_EXP)

/*
关系表达式中的直接赋值
*/
define_semantic_rule(___REL_EXP_1___)
    copy_syntax_symbol_attributes(REL_EXP,ADD_EXP);
end_define_semantic_rule

/*
关系表达式中的小于
*/
binary_compute_semantic_rule(___REL_EXP_2___,ic_op::LT,REL_EXP,ADD_EXP)

/*
关系表达式中的大于
*/
binary_compute_semantic_rule(___REL_EXP_3___,ic_op::GT,REL_EXP,ADD_EXP)

/*
关系表达式中的小于等于
*/
binary_compute_semantic_rule(___REL_EXP_4___,ic_op::LE,REL_EXP,ADD_EXP)

/*
关系表达式中的大于等于
*/
binary_compute_semantic_rule(___REL_EXP_5___,ic_op::GE,REL_EXP,ADD_EXP)

/*
判等表达式中的直接赋值
*/
define_semantic_rule(___EQ_EXP_1___)
    copy_syntax_symbol_attributes(EQ_EXP,REL_EXP);
end_define_semantic_rule

/*
判等表达式中的判断相等
*/
binary_compute_semantic_rule(___EQ_EXP_2___,ic_op::EQ,EQ_EXP,REL_EXP)

/*
判等表达式中的判断不相等
*/
binary_compute_semantic_rule(___EQ_EXP_3___,ic_op::UEQ,EQ_EXP,REL_EXP)

/*
逻辑与表达式中的直接赋值
*/
define_semantic_rule(___L_AND_EXP_1___)
    copy_syntax_symbol_attributes(L_AND_EXP,EQ_EXP);
end_define_semantic_rule

/*
逻辑与表达式中的逻辑与，具有短路机制
*/
define_semantic_rule(___L_AND_EXP_2___)
    vector<list<struct quaternion>::iterator> * backpatching_codes;
    struct ic_data * ic_data;
    if(check_syntax_symbol_attribute_by_index(L_AND_EXP,and_backpatching_codes,1))
    {
        backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(L_AND_EXP,and_backpatching_codes,pointer,1);
    }
    else
    {
        backpatching_codes=new vector<list<struct quaternion>::iterator>;
        ic_data=(struct ic_data *)get_syntax_symbol_attribute_by_index(L_AND_EXP,value,pointer,1);
        if(ic_data->is_const() && !ic_data->is_array_var())
        {
            switch(ic_data->get_data_type())
            {
                case language_data_type::INT:
                    //生成一条等待回填的无条件跳转的中间代码
                    if(!ic_data->get_value().int_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
                        backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                case language_data_type::FLOAT:
                    //生成一条等待回填的无条件跳转的中间代码
                    if(!ic_data->get_value().float_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
                        backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            //生成一条等待回填的有条件跳转的中间代码
            gen_one_operand_code(ic_op::IF_NOT_JMP,ic_operand::DATA,ic_data,ic_operand::LABEL,0);
            //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
            backpatching_codes->push_back(generated_code_iterator());
        }
    }
    set_syntax_symbol_attribute(EQ_EXP,and_backpatching_codes,pointer,backpatching_codes);
end_define_semantic_rule

/*
逻辑与表达式中的逻辑与，具有短路机制
*/
define_semantic_rule(___L_AND_EXP_3___)
    vector<list<struct quaternion>::iterator> * backpatching_codes;
    struct ic_data * ic_data;
    backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(EQ_EXP,and_backpatching_codes,pointer);
    ic_data=(struct ic_data *)get_syntax_symbol_attribute(EQ_EXP,value,pointer);
    if(ic_data->is_const() && !ic_data->is_array_var())
    {
        switch(ic_data->get_data_type())
        {
            case language_data_type::INT:
                //生成一条等待回填的无条件跳转的中间代码
                if(!ic_data->get_value().int_data)
                {
                    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                    //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
                    backpatching_codes->push_back(generated_code_iterator());
                }
                break;
            case language_data_type::FLOAT:
                //生成一条等待回填的无条件跳转的中间代码
                if(!ic_data->get_value().float_data)
                {
                    gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                    //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
                    backpatching_codes->push_back(generated_code_iterator());
                }
                break;
            default:
                break;
        }
    }
    else
    {
        //生成一条等待回填的有条件跳转的中间代码
        gen_one_operand_code(ic_op::IF_NOT_JMP,ic_operand::DATA,ic_data,ic_operand::LABEL,0);
        //获取刚刚生成的跳转中间代码的迭代器,并将它加入需要进行回填的中间代码
        backpatching_codes->push_back(generated_code_iterator());
    }
    set_syntax_symbol_attribute_by_index(L_AND_EXP,and_backpatching_codes,pointer,0,backpatching_codes);
end_define_semantic_rule

/*
逻辑或表达式中的直接赋值
*/
define_semantic_rule(___L_OR_EXP_1___)
    copy_syntax_symbol_attributes(L_OR_EXP,L_AND_EXP);
end_define_semantic_rule

/*
逻辑与表达式中的逻辑或，具有短路机制
*/
define_semantic_rule(___L_OR_EXP_2___)
    vector<list<struct quaternion>::iterator> * or_backpatching_codes,* and_backpatching_codes;
    struct ic_label * false_label;
    struct ic_data * ic_data;
    if(check_syntax_symbol_attribute_by_index(L_OR_EXP,or_backpatching_codes,1))
    {
        or_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(L_OR_EXP,or_backpatching_codes,pointer,1);
    }
    else
    {
        or_backpatching_codes=new vector<list<struct quaternion>::iterator>;
        if(check_syntax_symbol_attribute_by_index(L_OR_EXP,and_backpatching_codes,1))
        {
            //产生一个当AND表达式中的所有的AND都是true的时候等待回填的跳转
            gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
            or_backpatching_codes->push_back(generated_code_iterator());
            //产生一个当AND表达式中某一个AND是false的时候应该跳转到的标签
            false_label=def_label();
            //生成当AND表达式中某一个AND是false的时候应该跳转到的标签定义的中间代码
            gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,false_label);
            //进行回填
            and_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute_by_index(L_OR_EXP,and_backpatching_codes,pointer,1);
            for(vector<list<struct quaternion>::iterator>::iterator i=and_backpatching_codes->begin();i!=and_backpatching_codes->end();i++)
            {
                (*(*i)).result.second=(void *)false_label;
            }
            delete and_backpatching_codes;
        }
        else if(check_syntax_symbol_attribute_by_index(L_OR_EXP,value,1))
        {
            ic_data=(struct ic_data * )get_syntax_symbol_attribute_by_index(L_OR_EXP,value,pointer,1);
            if(ic_data->is_const() && !ic_data->is_array_var())
            {
                switch(ic_data->get_data_type())
                {
                    case language_data_type::INT:
                        //生成一条等待回填的无条件跳转的中间代码
                        if(ic_data->get_value().int_data)
                        {
                            gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                            or_backpatching_codes->push_back(generated_code_iterator());
                        }
                        break;
                    case language_data_type::FLOAT:
                        //生成一条等待回填的无条件跳转的中间代码
                        if(ic_data->get_value().float_data)
                        {
                            gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                            or_backpatching_codes->push_back(generated_code_iterator());
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                //生成一条等待回填的有条件跳转的中间代码
                gen_one_operand_code(ic_op::IF_JMP,ic_operand::DATA,ic_data,ic_operand::LABEL,0);
                or_backpatching_codes->push_back(generated_code_iterator());
            }
        }
        else
        {
            print_error("___L_OR_EXP_2___ error!");
        }
    }
    set_syntax_symbol_attribute(L_AND_EXP,or_backpatching_codes,pointer,or_backpatching_codes);
end_define_semantic_rule

/*
逻辑与表达式中的逻辑或，具有短路机制
*/
define_semantic_rule(___L_OR_EXP_3___)
    vector<list<struct quaternion>::iterator> * and_backpatching_codes,* or_backpatching_codes;
    struct ic_label * false_label;
    struct ic_data * ic_data;
    or_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(L_AND_EXP,or_backpatching_codes,pointer);
    //先判断是否需要对产生式右部的L_AND_EXP进行回填
    if(check_syntax_symbol_attribute(L_AND_EXP,and_backpatching_codes))
    {
        //产生一个当AND表达式中的所有的AND都是true的时候等待回填的跳转
        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
        or_backpatching_codes->push_back(generated_code_iterator());
        //产生一个当AND表达式中某一个AND是false的时候应该跳转到的标签
        false_label=def_label();
        //生成当AND表达式中某一个AND是false的时候应该跳转到的标签定义的中间代码
        gen_zero_operand_code(ic_op::LABEL_DEFINE,ic_operand::LABEL,false_label);
        //进行回填
        and_backpatching_codes=(vector<list<struct quaternion>::iterator> *)get_syntax_symbol_attribute(L_AND_EXP,and_backpatching_codes,pointer);
        for(vector<list<struct quaternion>::iterator>::iterator i=and_backpatching_codes->begin();i!=and_backpatching_codes->end();i++)
        {
            (*(*i)).result.second=(void *)false_label;
        }
        delete and_backpatching_codes;
    }
    else if(check_syntax_symbol_attribute(L_AND_EXP,value))
    {
        ic_data=(struct ic_data *)get_syntax_symbol_attribute(L_AND_EXP,value,pointer);
        if(ic_data->is_const() && !ic_data->is_array_var())
        {
            switch(ic_data->get_data_type())
            {
                case language_data_type::INT:
                    //生成一条等待回填的无条件跳转的中间代码
                    if(ic_data->get_value().int_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        or_backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                case language_data_type::FLOAT:
                    //生成一条等待回填的无条件跳转的中间代码
                    if(ic_data->get_value().float_data)
                    {
                        gen_zero_operand_code(ic_op::JMP,ic_operand::LABEL,0);
                        or_backpatching_codes->push_back(generated_code_iterator());
                    }
                    break;
                default:
                    break;
            }
        }
        else
        {
            //生成一条等待回填的有条件跳转的中间代码
            gen_one_operand_code(ic_op::IF_JMP,ic_operand::DATA,ic_data,ic_operand::LABEL,0);
            or_backpatching_codes->push_back(generated_code_iterator());
        }
    }
    else
    {
        print_error("___L_OR_EXP_3___ error!");
    }
    set_syntax_symbol_attribute_by_index(L_OR_EXP,or_backpatching_codes,pointer,0,or_backpatching_codes);
end_define_semantic_rule

/*
常量表达式
*/
define_semantic_rule(___CONST_EXP___)
    //把ADD_EXP中的所有属性复制一份给CONST_EXP
    copy_syntax_symbol_attributes(CONST_EXP,ADD_EXP);
end_define_semantic_rule






//语义动作注册函数表
map<string,semantic_rule_register> semantic_rule_registers={
    semantic_rule_registers_entry(___COMP_UNIT_1___),
    semantic_rule_registers_entry(___COMP_UNIT_2___),
    semantic_rule_registers_entry(___DECL_or_FUNC_DEF_1___),
    semantic_rule_registers_entry(___DECL_or_FUNC_DEF_2___),
    semantic_rule_registers_entry(___DECL_1___),
    semantic_rule_registers_entry(___DECL_2___),
    semantic_rule_registers_entry(___B_TYPE_1___),
    semantic_rule_registers_entry(___B_TYPE_2___),
    semantic_rule_registers_entry(___CONST_DECL___),
    semantic_rule_registers_entry(___one_or_more_CONST_DEF_divided_with_comma_1___),
    semantic_rule_registers_entry(___one_or_more_CONST_DEF_divided_with_comma_2___),
    semantic_rule_registers_entry(___CONST_DEF___),
    semantic_rule_registers_entry(___zero_or_more_CONST_EXP_surrounded_by_bracket_1___),
    semantic_rule_registers_entry(___zero_or_more_CONST_EXP_surrounded_by_bracket_2___),
    semantic_rule_registers_entry(___CONST_INIT_VAL_1___),
    semantic_rule_registers_entry(___CONST_INIT_VAL_2___),
    semantic_rule_registers_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_1___),
    semantic_rule_registers_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_2___),
    semantic_rule_registers_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_3___),
    semantic_rule_registers_entry(___VAR_DECL___),
    semantic_rule_registers_entry(___one_or_more_VAR_DEF_divided_with_comma_1___),
    semantic_rule_registers_entry(___one_or_more_VAR_DEF_divided_with_comma_2___),
    semantic_rule_registers_entry(___VAR_DEF_1___),
    semantic_rule_registers_entry(___VAR_DEF_2___),
    semantic_rule_registers_entry(___INIT_VAL_1___),
    semantic_rule_registers_entry(___INIT_VAL_2___),
    semantic_rule_registers_entry(___zero_or_more_INIT_VAL_divided_with_comma_1___),
    semantic_rule_registers_entry(___zero_or_more_INIT_VAL_divided_with_comma_2___),
    semantic_rule_registers_entry(___zero_or_more_INIT_VAL_divided_with_comma_3___),
    semantic_rule_registers_entry(___FUNC_F_PARAMS___),
    semantic_rule_registers_entry(___one_or_more_FUNC_F_PARAM_divided_with_comma_1___),
    semantic_rule_registers_entry(___one_or_more_FUNC_F_PARAM_divided_with_comma_2___),
    semantic_rule_registers_entry(___FUNC_F_PARAM_1___),
    semantic_rule_registers_entry(___FUNC_F_PARAM_2___),
    semantic_rule_registers_entry(___zero_or_more_EXP_surrounded_by_bracket_1___),
    semantic_rule_registers_entry(___zero_or_more_EXP_surrounded_by_bracket_2___),
    semantic_rule_registers_entry(___FUNC_DEF_1___),
    semantic_rule_registers_entry(___FUNC_DEF_2___),
    semantic_rule_registers_entry(___FUNC_DEF_3___),
    semantic_rule_registers_entry(___FUNC_DEF_4___),
    semantic_rule_registers_entry(___FUNC_DEF_5___),
    semantic_rule_registers_entry(___FUNC_DEF_6___),
    semantic_rule_registers_entry(___FUNC_DEF_7___),
    semantic_rule_registers_entry(___FUNC_DEF_8___),
    semantic_rule_registers_entry(___BLOCK_1___),
    semantic_rule_registers_entry(___BLOCK_2___),
    semantic_rule_registers_entry(___zero_or_more_BLOCK_ITEM_1___),
    semantic_rule_registers_entry(___zero_or_more_BLOCK_ITEM_2___),
    semantic_rule_registers_entry(___BLOCK_ITEM_1___),
    semantic_rule_registers_entry(___BLOCK_ITEM_2___),
    semantic_rule_registers_entry(___STMT_1___),
    semantic_rule_registers_entry(___STMT_2___),
    semantic_rule_registers_entry(___STMT_3___),
    semantic_rule_registers_entry(___STMT_4___),
    semantic_rule_registers_entry(___STMT_5___),
    semantic_rule_registers_entry(___STMT_6___),
    semantic_rule_registers_entry(___STMT_7___),
    semantic_rule_registers_entry(___JMP_STMT_1___),
    semantic_rule_registers_entry(___JMP_STMT_2___),
    semantic_rule_registers_entry(___JMP_STMT_3___),
    semantic_rule_registers_entry(___JMP_STMT_4___),
    semantic_rule_registers_entry(___WHILE_STMT_1___),
    semantic_rule_registers_entry(___WHILE_STMT_2___),
    semantic_rule_registers_entry(___IF_STMT_1___),
    semantic_rule_registers_entry(___IF_STMT_2___),
    semantic_rule_registers_entry(___IF_STMT_3___),
    semantic_rule_registers_entry(___EXP___),
    semantic_rule_registers_entry(___COND___),
    semantic_rule_registers_entry(___L_VAL___),
    semantic_rule_registers_entry(___PRIMARY_EXP_1___),
    semantic_rule_registers_entry(___PRIMARY_EXP_2___),
    semantic_rule_registers_entry(___PRIMARY_EXP_3___),
    semantic_rule_registers_entry(___NUMBER_1___),
    semantic_rule_registers_entry(___NUMBER_2___),
    semantic_rule_registers_entry(___UNARY_EXP_1___),
    semantic_rule_registers_entry(___UNARY_EXP_2___),
    semantic_rule_registers_entry(___UNARY_EXP_3___),
    semantic_rule_registers_entry(___UNARY_EXP_4___),
    semantic_rule_registers_entry(___UNARY_OP_1___),
    semantic_rule_registers_entry(___UNARY_OP_2___),
    semantic_rule_registers_entry(___UNARY_OP_3___),
    semantic_rule_registers_entry(___FUNC_R_PARAMS_1___),
    semantic_rule_registers_entry(___FUNC_R_PARAMS_2___),
    semantic_rule_registers_entry(___MUL_EXP_1___),
    semantic_rule_registers_entry(___MUL_EXP_2___),
    semantic_rule_registers_entry(___MUL_EXP_3___),
    semantic_rule_registers_entry(___MUL_EXP_4___),
    semantic_rule_registers_entry(___ADD_EXP_1___),
    semantic_rule_registers_entry(___ADD_EXP_2___),
    semantic_rule_registers_entry(___ADD_EXP_3___),
    semantic_rule_registers_entry(___REL_EXP_1___),
    semantic_rule_registers_entry(___REL_EXP_2___),
    semantic_rule_registers_entry(___REL_EXP_3___),
    semantic_rule_registers_entry(___REL_EXP_4___),
    semantic_rule_registers_entry(___REL_EXP_5___),
    semantic_rule_registers_entry(___EQ_EXP_1___),
    semantic_rule_registers_entry(___EQ_EXP_2___),
    semantic_rule_registers_entry(___EQ_EXP_3___),
    semantic_rule_registers_entry(___L_AND_EXP_1___),
    semantic_rule_registers_entry(___L_AND_EXP_2___),
    semantic_rule_registers_entry(___L_AND_EXP_3___),
    semantic_rule_registers_entry(___L_OR_EXP_1___),
    semantic_rule_registers_entry(___L_OR_EXP_2___),
    semantic_rule_registers_entry(___L_OR_EXP_3___),
    semantic_rule_registers_entry(___CONST_EXP___),
};

//语义动作函数表
map<string,semantic_rule> semantic_rules={
    semantic_rules_entry(___COMP_UNIT_1___),
    semantic_rules_entry(___COMP_UNIT_2___),
    semantic_rules_entry(___DECL_or_FUNC_DEF_1___),
    semantic_rules_entry(___DECL_or_FUNC_DEF_2___),
    semantic_rules_entry(___DECL_1___),
    semantic_rules_entry(___DECL_2___),
    semantic_rules_entry(___B_TYPE_1___),
    semantic_rules_entry(___B_TYPE_2___),
    semantic_rules_entry(___CONST_DECL___),
    semantic_rules_entry(___one_or_more_CONST_DEF_divided_with_comma_1___),
    semantic_rules_entry(___one_or_more_CONST_DEF_divided_with_comma_2___),
    semantic_rules_entry(___CONST_DEF___),
    semantic_rules_entry(___zero_or_more_CONST_EXP_surrounded_by_bracket_1___),
    semantic_rules_entry(___zero_or_more_CONST_EXP_surrounded_by_bracket_2___),
    semantic_rules_entry(___CONST_INIT_VAL_1___),
    semantic_rules_entry(___CONST_INIT_VAL_2___),
    semantic_rules_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_1___),
    semantic_rules_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_2___),
    semantic_rules_entry(___zero_or_more_CONST_INIT_VAL_divided_with_comma_3___),
    semantic_rules_entry(___VAR_DECL___),
    semantic_rules_entry(___one_or_more_VAR_DEF_divided_with_comma_1___),
    semantic_rules_entry(___one_or_more_VAR_DEF_divided_with_comma_2___),
    semantic_rules_entry(___VAR_DEF_1___),
    semantic_rules_entry(___VAR_DEF_2___),
    semantic_rules_entry(___INIT_VAL_1___),
    semantic_rules_entry(___INIT_VAL_2___),
    semantic_rules_entry(___zero_or_more_INIT_VAL_divided_with_comma_1___),
    semantic_rules_entry(___zero_or_more_INIT_VAL_divided_with_comma_2___),
    semantic_rules_entry(___zero_or_more_INIT_VAL_divided_with_comma_3___),
    semantic_rules_entry(___FUNC_F_PARAMS___),
    semantic_rules_entry(___one_or_more_FUNC_F_PARAM_divided_with_comma_1___),
    semantic_rules_entry(___one_or_more_FUNC_F_PARAM_divided_with_comma_2___),
    semantic_rules_entry(___FUNC_F_PARAM_1___),
    semantic_rules_entry(___FUNC_F_PARAM_2___),
    semantic_rules_entry(___zero_or_more_EXP_surrounded_by_bracket_1___),
    semantic_rules_entry(___zero_or_more_EXP_surrounded_by_bracket_2___),
    semantic_rules_entry(___FUNC_DEF_1___),
    semantic_rules_entry(___FUNC_DEF_2___),
    semantic_rules_entry(___FUNC_DEF_3___),
    semantic_rules_entry(___FUNC_DEF_4___),
    semantic_rules_entry(___FUNC_DEF_5___),
    semantic_rules_entry(___FUNC_DEF_6___),
    semantic_rules_entry(___FUNC_DEF_7___),
    semantic_rules_entry(___FUNC_DEF_8___),
    semantic_rules_entry(___BLOCK_1___),
    semantic_rules_entry(___BLOCK_2___),
    semantic_rules_entry(___zero_or_more_BLOCK_ITEM_1___),
    semantic_rules_entry(___zero_or_more_BLOCK_ITEM_2___),
    semantic_rules_entry(___BLOCK_ITEM_1___),
    semantic_rules_entry(___BLOCK_ITEM_2___),
    semantic_rules_entry(___STMT_1___),
    semantic_rules_entry(___STMT_2___),
    semantic_rules_entry(___STMT_3___),
    semantic_rules_entry(___STMT_4___),
    semantic_rules_entry(___STMT_5___),
    semantic_rules_entry(___STMT_6___),
    semantic_rules_entry(___STMT_7___),
    semantic_rules_entry(___JMP_STMT_1___),
    semantic_rules_entry(___JMP_STMT_2___),
    semantic_rules_entry(___JMP_STMT_3___),
    semantic_rules_entry(___JMP_STMT_4___),
    semantic_rules_entry(___WHILE_STMT_1___),
    semantic_rules_entry(___WHILE_STMT_2___),
    semantic_rules_entry(___IF_STMT_1___),
    semantic_rules_entry(___IF_STMT_2___),
    semantic_rules_entry(___IF_STMT_3___),
    semantic_rules_entry(___EXP___),
    semantic_rules_entry(___COND___),
    semantic_rules_entry(___L_VAL___),
    semantic_rules_entry(___PRIMARY_EXP_1___),
    semantic_rules_entry(___PRIMARY_EXP_2___),
    semantic_rules_entry(___PRIMARY_EXP_3___),
    semantic_rules_entry(___NUMBER_1___),
    semantic_rules_entry(___NUMBER_2___),
    semantic_rules_entry(___UNARY_EXP_1___),
    semantic_rules_entry(___UNARY_EXP_2___),
    semantic_rules_entry(___UNARY_EXP_3___),
    semantic_rules_entry(___UNARY_EXP_4___),
    semantic_rules_entry(___UNARY_OP_1___),
    semantic_rules_entry(___UNARY_OP_2___),
    semantic_rules_entry(___UNARY_OP_3___),
    semantic_rules_entry(___FUNC_R_PARAMS_1___),
    semantic_rules_entry(___FUNC_R_PARAMS_2___),
    semantic_rules_entry(___MUL_EXP_1___),
    semantic_rules_entry(___MUL_EXP_2___),
    semantic_rules_entry(___MUL_EXP_3___),
    semantic_rules_entry(___MUL_EXP_4___),
    semantic_rules_entry(___ADD_EXP_1___),
    semantic_rules_entry(___ADD_EXP_2___),
    semantic_rules_entry(___ADD_EXP_3___),
    semantic_rules_entry(___REL_EXP_1___),
    semantic_rules_entry(___REL_EXP_2___),
    semantic_rules_entry(___REL_EXP_3___),
    semantic_rules_entry(___REL_EXP_4___),
    semantic_rules_entry(___REL_EXP_5___),
    semantic_rules_entry(___EQ_EXP_1___),
    semantic_rules_entry(___EQ_EXP_2___),
    semantic_rules_entry(___EQ_EXP_3___),
    semantic_rules_entry(___L_AND_EXP_1___),
    semantic_rules_entry(___L_AND_EXP_2___),
    semantic_rules_entry(___L_AND_EXP_3___),
    semantic_rules_entry(___L_OR_EXP_1___),
    semantic_rules_entry(___L_OR_EXP_2___),
    semantic_rules_entry(___L_OR_EXP_3___),
    semantic_rules_entry(___CONST_EXP___),
};

#pragma clang diagnostic pop
