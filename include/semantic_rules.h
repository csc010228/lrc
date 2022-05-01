/*
 *
 *  semantic_rules.h
 *  语义动作
 *  Creator:Sichao Chen
 *  Create time:2021/11/04
 *
*/
#ifndef __SEMANTIC_RULES_H
#define __SEMANTIC_RULES_H

#include <vector>
#include <map>
#include <string>
#include <utility>

#include "symbol_table.h"

using namespace std;

//中间代码的操作符
enum class ic_op
{
    NOP,                        //空
    ASSIGN,                     //赋值
    ADD,                        //加
    SUB,                        //减
    MUL,                        //乘
    DIV,                        //除
    MOD,                        //取模
    // L_SHIFT,                    //左移位
    // R_SHIFT,                    //右移位
    NOT,                        //非
    // AND,                        //与
    // OR,                         //或
    // XOR,                        //异或
    EQ,                         //是否相等
    UEQ,                        //是否不相等
    GT,                         //比较是否大于
    LT,                         //比较是否小于
    GE,                         //比较是否大于等于
    LE,                         //比较是否小于等于
    JMP,                        //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
    IF_JMP,                     //有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码，当条件表达式的结果不是0的时候会进行跳转）
    IF_NOT_JMP,                 //有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码，当条件表达式的结果是0的时候会进行跳转）
    VAR_DEFINE,                 //变量定义（可以赋初始值，也可以不赋初始值）
    LABEL_DEFINE,               //定义一个LABEL
    FUNC_DEFINE,                //函数定义（第一个操作数是形参列表，结果是要定义的函数在符号表中的表项指针）
    END_FUNC_DEFINE,            //结束函数定义（只有结果操作数，是结束定义的函数在符号表中的表项指针）
    CALL,                       //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
    RET,                        //函数返回（只有结果，没有操作数，结果就是返回值）
};

//中间代码的操作数类型
enum class ic_operand
{
    NONE,                               //空
    DATA,                               //数据
    DATAS,                              //多个数据（目前只用在函数调用CALL上）
    LABEL,                              //标签
    FUNC,                               //函数
};

//栈指针
typedef unsigned int stack_index;

//中间代码的四元式表示
struct quaternion
{
    quaternion():op(ic_op::NOP),arg1(make_pair(ic_operand::NONE,nullptr)),arg2(make_pair(ic_operand::NONE,nullptr)),result(make_pair(ic_operand::NONE,nullptr))
    {

    };

    quaternion(enum ic_op op,enum ic_operand arg1_type,void * arg1_data,enum ic_operand arg2_type,void * arg2_data,enum ic_operand result_type,void * result_data):op(op),arg1(make_pair(arg1_type,arg1_data)),arg2(make_pair(arg2_type,arg2_data)),result(make_pair(result_type,result_data))
    {
        
    };

    //查看这条中间代码是否可用
    inline bool is_invalid() const
    {
        return (op==ic_op::NOP);
    }

    enum ic_op op;                     //操作符
    pair<enum ic_operand,void *> arg1;                   //操作数1
    pair<enum ic_operand,void *> arg2;                   //操作数2
    pair<enum ic_operand,void *> result;                 //结果
};

//中间代码的输出函数
typedef string (*ic_output)(struct quaternion);

//语义栈的结点
union semantic_stack_node_member
{
    bool bool_value;
    char char_value;
    int int_value;
    float float_value;
    double double_value;
    long long_value;
    void * pointer_value;
};

//语义动作
typedef map<string,union semantic_stack_node_member> (*semantic_rule)(list<struct quaternion> * intermediate_codes,vector<map<string,union semantic_stack_node_member> > * semantic_stack,stack_index stack_top,Symbol_table * symbol_table);

//语义动作注册函数
typedef void (*semantic_rule_register)(string left_string,vector<string> right_string,string right_next_string);

//定义语义动作函数
#define define_semantic_rule(semantic_rule) \
void semantic_rule##register(string left_string,vector<string> right_string,string right_next_string)\
{\
    string semantic_rule_name=#semantic_rule;\
    if(right_next_string=="")\
    {\
        syntax_symbol_offset_from_stack_top_map[semantic_rule_name][left_string].push_back(right_string.size());\
        stack_top_offset_between_before_and_after_reduce_map[semantic_rule_name]=right_string.size();\
    }\
    else\
    {\
        syntax_symbol_offset_from_stack_top_map[semantic_rule_name][left_string].push_back(right_string.size()+1);\
        syntax_symbol_offset_from_stack_top_map[semantic_rule_name][right_next_string].push_back(0);\
        stack_top_offset_between_before_and_after_reduce_map[semantic_rule_name]=0;\
    }\
    for(vector<string>::iterator i=right_string.begin();i!=right_string.end();i++)\
    {\
        syntax_symbol_offset_from_stack_top_map[semantic_rule_name][(*i)].push_back(right_string.end()-i);\
    }\
}\
map<string,union semantic_stack_node_member> semantic_rule(list<struct quaternion> * intermediate_codes,vector<map<string,union semantic_stack_node_member> > * semantic_stack,stack_index stack_top,Symbol_table * symbol_table)\
{\
    string semantic_rule_name=#semantic_rule;\
    stack_index stack_pos,offset_from_stack_top;\
    union semantic_stack_node_member semantic_stack_node_member_tmp;\
    map<string,union semantic_stack_node_member> result,semantic_stack_node_source;\
    bool error_tag=false;

//结束语义动作函数的定义
#define end_define_semantic_rule \
error_out:\
    if(error_tag)\
    {\
        cout<<"Error while executing semantic rule "<<semantic_rule_name<<"!"<<endl;\
    }\
    return result;\
}

//定义语义动作表的表项
#define semantic_rules_entry(semantic_rule) {#semantic_rule,semantic_rule}

//定义注册语义动作表的表项
#define semantic_rule_registers_entry(semantic_rule) {#semantic_rule,semantic_rule##register}

//获取语义栈中的某一个语法符号的某一个属性的值
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsequenced"
#define get_syntax_symbol_attribute_by_index(syntax_symbol,attribute,type,index) \
((semantic_stack->at(stack_pos=(stack_top-syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol][index])).find(#attribute)!=semantic_stack->at(stack_pos).end())?\
((semantic_stack->at(stack_pos)[#attribute]).type##_value):\
((semantic_stack->at(stack_pos-1).find(#attribute)!=semantic_stack->at(stack_pos-1).end())?\
((semantic_stack->at(stack_pos-1)[#attribute]).type##_value):\
(((semantic_stack->at(stack_pos-1)[#attribute]).type##_value))))
#pragma clang diagnostic pop

//获取语义栈中的某一个语法符号的某一个属性的值
#define get_syntax_symbol_attribute(syntax_symbol,attribute,type) \
get_syntax_symbol_attribute_by_index(syntax_symbol,attribute,type,0)

//给语义栈中的某一个语法符号的某一个属性赋值
#define set_syntax_symbol_attribute_by_index(syntax_symbol,attribute,type,index,value) \
semantic_stack_node_member_tmp.type##_value=(value);\
offset_from_stack_top=syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol][index];\
stack_pos=stack_top-offset_from_stack_top;\
if(offset_from_stack_top==stack_top_offset_between_before_and_after_reduce_map[semantic_rule_name])\
{\
    result[#attribute]=semantic_stack_node_member_tmp;\
}\
(semantic_stack->at(stack_pos)[#attribute])=semantic_stack_node_member_tmp

//给语义栈中的某一个语法符号的某一个属性赋值
#define set_syntax_symbol_attribute(syntax_symbol,attribute,type,value) \
set_syntax_symbol_attribute_by_index(syntax_symbol,attribute,type,0,value)

//给语义栈中的某一个语法符号的所有属性复制一份给另一个语法符号
#define copy_syntax_symbol_attribute_by_index(syntax_symbol_target,target_index,syntax_symbol_source,source_index) \
semantic_stack_node_source=semantic_stack->at(stack_top-syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol_source][source_index]);\
offset_from_stack_top=syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol_target][target_index];\
stack_pos=stack_top-offset_from_stack_top;\
if(offset_from_stack_top==stack_top_offset_between_before_and_after_reduce_map[semantic_rule_name])\
{\
    result.insert(semantic_stack_node_source.begin(),semantic_stack_node_source.end());\
}\
semantic_stack->at(stack_pos).insert(semantic_stack_node_source.begin(),semantic_stack_node_source.end())

//给语义栈中的某一个语法符号的所有属性复制一份给另一个语法符号
#define copy_syntax_symbol_attributes(syntax_symbol_target,syntax_symbol_source) \
copy_syntax_symbol_attribute_by_index(syntax_symbol_target,0,syntax_symbol_source,0)

//查看语义栈中的某一个语法符号是否有某一个属性
#define check_syntax_symbol_attribute_by_index(syntax_symbol,attribute,index) \
(semantic_stack->at(stack_top-syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol][index]).count(#attribute)!=0)

//查看语义栈中的某一个语法符号是否有某一个属性
#define check_syntax_symbol_attribute(syntax_symbol,attribute) \
check_syntax_symbol_attribute_by_index(syntax_symbol,attribute,0)

//删除语义栈中的某一个语法符号的某一个属性
#define delete_syntax_symbol_attribute_by_index(syntax_symbol,attribute,index) \
(semantic_stack->at(stack_top-syntax_symbol_offset_from_stack_top_map[semantic_rule_name][#syntax_symbol][index]).erase(#attribute))

//删除语义栈中的某一个语法符号的某一个属性
#define delete_syntax_symbol_attribute(syntax_symbol,attribute) \
delete_syntax_symbol_attribute_by_index(syntax_symbol,attribute,0)

//获取语法分析的时候的全局信息
#define get_global_info(info_name) \
global_info.info_name

//设置语法分析的时候的全局信息
#define set_global_info(info_name,info_value) \
global_info.info_name=info_value

//从符号表中获取一个变量的表项
#define var(var_name) \
symbol_table->var_entry(var_name)

//从符号表中获取一个函数的表项
#define func(func_name) \
symbol_table->func_entry(func_name)

//在符号表中定义一个非const的基本变量，并返回其指针
#define def_var(name,type) \
symbol_table->new_var(name,type,nullptr,0,false)

//在符号表中定义一个带有初值的非const的基本变量，并返回其指针
#define def_var_with_init_value(name,type,value) \
symbol_table->new_var(name,type,nullptr,value,false)

//在符号表中定义一个const的基本变量，并返回其指针
#define def_const_var(name,type,value) \
symbol_table->new_var(name,type,nullptr,value,true)

//在符号表中定义一个临时的基本变量，并返回其指针
#define def_tmp_var(type) \
symbol_table->new_tmp_var(type,nullptr,0,false)

//在符号表中定义一个临时的const基本变量，并返回其指针
#define def_const_tmp_var(type,value) \
symbol_table->new_tmp_var(type,nullptr,value,true)

//生成一个常量，并返回其指针
#define def_const(type,value) \
symbol_table->const_entry(type,OAA(value))

//生成一个数组成员，并返回其指针
#define def_array_member(array_var,offset_dimension_lens,offset) \
symbol_table->array_member_entry(array_var,offset_dimension_lens,offset)

//在符号表中定义一个数组变量，并返回其指针
#define def_array(name,type,dimensions_len) \
symbol_table->new_var(name,type,dimensions_len,0,false)

//在符号表中定义一个有初始值的数组变量，并返回其指针
#define def_array_with_init_value(name,type,dimensions_len,value) \
symbol_table->new_var(name,type,dimensions_len,value,false)

//在符号表中定义一个const的数组变量，并返回其指针
#define def_const_array(name,type,dimensions_len,const_array_value) \
symbol_table->new_var(name,type,dimensions_len,const_array_value,true)

//在符号表中定义一个非const的基本变量形参，并返回其在符号表中的表项指针
#define def_f_param(name,type) \
new struct ic_data(name,type,nullptr,false)

//在符号表中定义一个非const的数组变量形参，并返回其指针
#define def_array_f_param(name,type,dimensions_len) \
new struct ic_data(name,type,dimensions_len,false)

//在符号表中定义一个新的函数，返回其指针
#define def_func(name,return_type,f_params) \
symbol_table->new_func(name,return_type,f_params)

//定义一个新的标号
#define def_label() \
(symbol_table->new_label())

//定义一个新的作用域
#define def_scope() \
symbol_table->start_scope()

//结束当前作用域
#define end_def_scope() \
symbol_table->end_scope()

//获取当前的作用域
#define get_current_scope() \
symbol_table->get_current_scope()

//获取当前正在生成的函数
#define get_current_func() \
(symbol_table->get_current_func())

//生成两个操作数的中间代码
//默认生成到当前中间代码序列的最后
#define gen_two_operands_code(operator,arg1_type,arg1_value,arg2_type,arg2_value,result_type,result_value) \
intermediate_codes->push_back(quaternion(operator,arg1_type,(void *)(arg1_value),arg2_type,(void *)(arg2_value),result_type,(void *)(result_value)))

//生成两个操作数，没有结果的中间代码
//默认生成到当前中间代码序列的最后
#define gen_two_operands_without_result_code(operator,arg1_type,arg1_value,arg2_type,arg2_value) \
gen_two_operands_code(operator,arg1_type,arg1_value,arg2_type,arg2_value,ic_operand::NONE,nullptr)

//生成一个操作数的中间代码
//默认生成到当前中间代码序列的最后
#define gen_one_operand_code(operator,arg1_type,arg1_value,result_type,result_value) \
gen_two_operands_code(operator,arg1_type,arg1_value,ic_operand::NONE,nullptr,result_type,result_value)

//生成一个操作数,没有结果的中间代码
//默认生成到当前中间代码序列的最后
#define gen_one_operand_without_result_code(operator,arg1_type,arg1_value) \
gen_two_operands_code(operator,arg1_type,arg1_value,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr)

//生成没有操作数的中间代码
//默认生成到当前中间代码序列的最后
#define gen_zero_operand_code(operator,result_type,result_value) \
gen_two_operands_code(operator,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,result_type,result_value)

//生成只有操作码的中间代码（目前只用于生成无返回值的return）
//默认生成到当前中间代码序列的最后
#define gen_only_op_code(operator) \
gen_two_operands_code(operator,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr)

//获取刚刚生成的那一条中间代码的迭代器(迭代器的大小都是8bytes)
#define generated_code_iterator() \
(--end(*intermediate_codes))

//打印错误信息
#define print_error(error_info) \
cout<<error_info<<endl;\
error_tag=true;\
goto error_out


#endif  //__SEMANTIC_RULES_H