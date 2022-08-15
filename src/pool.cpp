/*
 *
 *  pool.cpp
 *  全局公共子表达式删除
 *  Creator:Zhengzhou Tu
 *  Create time:2022/8/9
 *
*/
#include"pool.h"
#include "semantic_rules.h"
#include<stdio.h>
#include<stdarg.h>
#include<algorithm>

// //在中间代码中输出的临时变量编号前缀
// #define TMP_PREFIX "@"

// //作用域输出信息
// map<enum ic_scope_type,string> pool_scope_type_output_map={
//     {ic_scope_type::GLOBAL,""},
//     {ic_scope_type::FUNC,""},
//     {ic_scope_type::IF,"if"},
//     {ic_scope_type::ELSE,"else"},
//     {ic_scope_type::WHILE,"while"},
//     {ic_scope_type::ANONYMOUS,""},
//     {ic_scope_type::INLINE_FUNC,"<inline>"},
// };

// //变量类型的输出信息
// map<enum language_data_type,string> pool_identifier_type_output_map={
//     {language_data_type::VOID,"void"},
//     {language_data_type::CHAR,"char"},
//     {language_data_type::INT,"int"},
//     {language_data_type::FLOAT,"float"},
//     {language_data_type::LONG,"long"},
//     {language_data_type::DOUBLE,"double"},
//     {language_data_type::POINTER,"pointer"}
// };

// //中间代码的输出信息
// map<ic_op,string> pool_ic_op_output_map={
//     {ic_op::ASSIGN,"ASSIGN"},              //赋值
//     {ic_op::ADD,"ADD"},                    //加
//     {ic_op::SUB,"SUB"},                    //减
//     {ic_op::MUL,"MUL"},                    //乘
//     {ic_op::DIV,"DIV"},                    //除
//     {ic_op::MOD,"MOD"},                    //取模
//     // {ic_op::L_SHIFT,"L_SHIFT"},            //左移位
//     // {ic_op::R_SHIFT,"R_SHIFT"},            //右移位
//     {ic_op::NOT,"NOT"},                    //非
//     // {ic_op::AND,"AND"},                    //与
//     // {ic_op::OR,"OR"},                      //或
//     // {ic_op::XOR,"XOR"},                    //异或
//     {ic_op::EQ,"EQ"},                      //是否相等
//     {ic_op::UEQ,"UEQ"},                    //是否不相等
//     {ic_op::GT,"GT"},                      //比较是否大于
//     {ic_op::LT,"LT"},                      //比较是否小于
//     {ic_op::GE,"GE"},                      //比较是否大于等于
//     {ic_op::LE,"LE"},                      //比较是否小于等于
//     {ic_op::JMP,"JMP"},                    //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
//     {ic_op::IF_JMP,"IF_JMP"},              //结果为true的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
//     {ic_op::IF_NOT_JMP,"IF_NOT_JMP"},      //结果为false的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
//     {ic_op::VAR_DEFINE,"VAR_DEFINE"},      //变量定义（可以赋初始值，也可以不赋初始值）
//     {ic_op::FUNC_DEFINE,"FUNC_DEFINE"},    //函数定义（结果是指向要定义的函数在符号表中的表项的指针）
//     {ic_op::END_FUNC_DEFINE,"END_FUNC_DEFINE"},    //结束函数定义（结果是指向要结束定义的函数在符号表中的表项的指针）
//     {ic_op::LABEL_DEFINE,"LABEL_DEFINE"},  //定义一个LABEL
//     {ic_op::CALL,"CALL"},                  //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
//     {ic_op::RET,"RET"},                    //函数返回（只有结果，没有操作数，结果就是返回值）
// };



// //打印中间代码的操作符
// string pool_output_ic_op(ic_op op)
// {
//     return pool_ic_op_output_map[op];
// }

// //打印中间代码中的常数类型的操作数
// string pool_output_ic_const_operand(enum language_data_type type,OAA const_value)
// {
//     string res;
//     switch(type)
//     {
//         case language_data_type::INT:
//             res=to_string(const_value.int_data);
//             break;
//         case language_data_type::FLOAT:
//             res=to_string(const_value.float_data);
//             break;
//         default:
//             break;
//     }
//     return res;
// }

// //打印中间代码中的标签类型的操作数
// string pool_output_ic_label_operand(struct ic_label * label)
// {
//     return label->name;
// }

// //打印中间代码中的作用域
// string pool_output_ic_scope(struct ic_scope * scope)
// {
//     string res="";
//     struct ic_scope * tmp_scope=scope;
//     while(!tmp_scope->is_global())
//     {
//         if(tmp_scope->is_func())
//         {
//             res=(tmp_scope->func->name+"::"+res);
//         }
//         else if(tmp_scope->is_inline_func())
//         {
//             res=(pool_scope_type_output_map.at(tmp_scope->type)+tmp_scope->func->name+"-"+to_string(tmp_scope->index_for_output)+"::"+res);
//         }
//         else
//         {
//             res=(pool_scope_type_output_map.at(tmp_scope->type)+"_"+to_string(tmp_scope->index_for_output)+"::"+res);
//         }
//         tmp_scope=tmp_scope->get_father();
//     }
//     return res;
// }

// //打印中间代码中的变量类型的操作数
// string pool_output_ic_var_operand(struct ic_data * ic_data,bool print_details,bool print_init_value,bool is_define=false)
// {
//     string res,tmp;
//     stack<size_t> array_init_value_output_stack;
//     vector<OAA> * array_const_init_value;
//     size_t array_init_value_elements_outputed=0;
//     vector<struct ic_data * > * array_var_init_value;
//     list<struct ic_data * > * dimensions_len;
//     list<struct ic_data * >::iterator array_init_value_output_pointer;
//     if(ic_data->is_tmp_var())
//     {
//         if(is_define)
//         {
//             res+=("("+pool_identifier_type_output_map.at(ic_data->get_data_type())+")");
//         }
//         res+=TMP_PREFIX+to_string(ic_data->tmp_index);
//     }
//     else if(ic_data->is_pure_const())
//     {
//         res=pool_output_ic_const_operand(ic_data->data_type,ic_data->get_value());
//     }
//     else if(ic_data->is_array_member())
//     {
//         res=pool_output_ic_var_operand(((pair<struct ic_data *,struct ic_data * > *)ic_data->get_value().pointer_data)->first,false,false)+"["+pool_output_ic_var_operand(((pair<struct ic_data *,struct ic_data * > * )ic_data->get_value().pointer_data)->second,false,false)+"]";
//     }
//     else
//     {
//         res="";
//         if(print_details)
//         {
//             if(ic_data->is_const_var())
//             {
//                 res+="const ";
//             }
//             res+=pool_identifier_type_output_map[ic_data->data_type]+" "+pool_output_ic_scope(ic_data->get_scope())+ic_data->get_var_name();
//             if(ic_data->dimensions_len)
//             {
//                 for(list<struct ic_data * >::iterator i=ic_data->dimensions_len->begin();i!=ic_data->dimensions_len->end();i++)
//                 {
//                     res+="["+pool_output_ic_var_operand(*i,false,false)+"]";
//                 }
//             }
//         }
//         else
//         {
//             res+=(pool_output_ic_scope(ic_data->get_scope())+ic_data->get_var_name());
//         }
//         if(print_init_value)
//         {
//             if(ic_data->is_array_var())
//             {
//                 dimensions_len=ic_data->dimensions_len;
//                 if(ic_data->is_const_var() || ic_data->is_global())
//                 {
//                     if(ic_data->get_value().pointer_data)
//                     {
//                         res+=" = { ";
//                         array_init_value_output_pointer=dimensions_len->begin();
//                         array_const_init_value=(vector<OAA> *)ic_data->get_value().pointer_data;
//                         array_init_value_output_stack.push((size_t)((*(array_init_value_output_pointer++))->get_value().int_data));
//                         while(!array_init_value_output_stack.empty())
//                         {
//                             while(array_init_value_output_pointer!=dimensions_len->end())
//                             {
//                                 res+="{ ";
//                                 array_init_value_output_stack.push((size_t)((*(array_init_value_output_pointer++))->get_value().int_data));
//                             }
//                             for(size_t i=array_init_value_output_stack.top();i!=0;i--)
//                             {
//                                 res+=(pool_output_ic_const_operand(language_data_type::INT,array_const_init_value->at(array_init_value_elements_outputed++))+" , ");
//                             }
//                             array_init_value_output_stack.top()=1;
//                             while((--array_init_value_output_stack.top())==0)
//                             {
//                                 array_init_value_output_stack.pop();
//                                 array_init_value_output_pointer--;
//                                 res=res.substr(0,res.size()-3);
//                                 res+=" } , ";
//                                 if(array_init_value_output_stack.empty())
//                                 {
//                                     break;
//                                 }
//                             }
//                         }
//                         res=res.substr(0,res.size()-3);
//                     }
//                 }
//                 else
//                 {
//                     if(ic_data->get_value().bool_data)
//                     {
//                         res+=" = { 0 , ...... } ";
//                     }
//                 }
//             }
//             else
//             {
//                 if(ic_data->get_value().pointer_data)
//                 {
//                     res+=" = ";
//                     if(ic_data->is_const_var())
//                     {
//                         res+=pool_output_ic_const_operand(ic_data->get_data_type(),ic_data->get_value().pointer_data);
//                     }
//                     else
//                     {
//                         res+=pool_output_ic_var_operand((struct ic_data *)ic_data->get_value().pointer_data,false,false);
//                     }
//                 }
//             }
//         }
//     }
//     return res;
// }

// //打印中间代码中的多个变量类型的操作数
// string pool_output_ic_vars_operand(list<struct ic_data * > * vars,string separator,bool print_details)
// {
//     string res;
//     if(vars)
//     {
//         for(auto i:*vars)
//         {
//             res+=(pool_output_ic_var_operand(i,print_details,false)+separator);
//         }
//         res=res.substr(0,res.size()-separator.size());
//     }
//     return res;
// }

// //打印中间代码中的函数类型的操作数
// string pool_output_ic_func_operand(struct ic_func * func,bool print_details)
// {
//     string res;
//     if(print_details)
//     {
//         res=pool_identifier_type_output_map[func->return_type]+" "+func->name+" ( "+pool_output_ic_vars_operand(func->f_params," , ",print_details)+" ) ";
//     }
//     else
//     {
//         res=func->name;
//     }
//     return res;
// }

// //打印中间代码的操作数
// string pool_output_ic_operand(pair<ic_operand,void *> operand,bool print_details,bool is_define=false)
// {
//     struct ic_data * ic_data;
//     struct ic_func * func;
//     string res;
//     switch(operand.first)
//     {
//         case ic_operand::DATA:
//             res=pool_output_ic_var_operand((struct ic_data *)operand.second,print_details,print_details,is_define);
//             break;
//         case ic_operand::DATAS:
//             res=pool_output_ic_vars_operand((list<struct ic_data * > *)operand.second," , ",print_details);
//             break;
//         case ic_operand::LABEL:
//             res=pool_output_ic_label_operand((struct ic_label *)operand.second);
//             break;
//         case ic_operand::FUNC:
//             res=pool_output_ic_func_operand((struct ic_func *)operand.second,print_details);
//             break;
//         case ic_operand::NONE:
//             break;
//         default:
//             break;
//     }
//     return res;
// }

// //输出只有操作码的中间代码
// string pool_output_only_op_ic(struct quaternion qua)
// {
//     return "\t"+pool_output_ic_op(qua.op);
// }

// //输出没有操作数，只有操作码的中间代码
// string pool_output_zero_operand_ic(struct quaternion qua)
// {
//     return "\t"+pool_output_ic_op(qua.op)+"\t\t"+pool_output_ic_operand(qua.result,false);
// }

// //输出空代码
// string pool_output_nop_ic(struct quaternion qua)
// {
//     return "\tNOP";
// }

// //输出单操作数的中间代码
// string pool_output_one_operand_ic(struct quaternion qua)
// {
//     return "\t"+pool_output_ic_op(qua.op)+"\t\t"+pool_output_ic_operand(qua.arg1,false)+" --> "+pool_output_ic_operand(qua.result,false,true);
// }

// //输出双操作数的中间代码
// string pool_output_two_operands_ic(struct quaternion qua)
// {
//     return "\t"+pool_output_ic_op(qua.op)+"\t\t"+pool_output_ic_operand(qua.arg1,false)+" , "+pool_output_ic_operand(qua.arg2,false)+" --> "+pool_output_ic_operand(qua.result,false,true);
// }

// //输出变量定义的中间代码
// string pool_output_var_define_ic(struct quaternion qua)
// {
//     return ((struct ic_data *)qua.result.second)->is_global()?(pool_output_ic_operand(qua.result,true)):("\t"+pool_output_ic_operand(qua.result,true));
// }

// //输出函数定义的中间代码
// string pool_output_func_define_ic(struct quaternion qua)
// {
//     return pool_output_ic_operand(qua.result,true)+" : ";
// }

// //输出结束函数定义的中间代码
// string pool_output_end_func_define_ic(struct quaternion qua)
// {
//     return "";
// }

// //输出标签定义的中间代码
// string pool_output_label_define_ic(struct quaternion qua)
// {
//     return pool_output_ic_operand(qua.result,true)+" : ";
// }

// //输出函数调用的中间代码
// string pool_output_call_ic(struct quaternion qua)
// {
//     string res="\t"+pool_output_ic_op(qua.op)+"\t\t"+pool_output_ic_operand(qua.arg1,false)+" ( "+pool_output_ic_operand(qua.arg2,false)+" ) ";
//     if(qua.result.first!=ic_operand::NONE)
//     {
//         res+=(" --> "+pool_output_ic_operand(qua.result,false,true));
//     }
//     return res;
// }

// //输出return语句的中间代码
// string pool_output_return_ic(struct quaternion qua)
// {
//     string res="\t"+pool_output_ic_op(qua.op);
//     if(qua.result.first!=ic_operand::NONE)
//     {
//         res+=("\t\t"+pool_output_ic_operand(qua.result,false));
//     }
//     return res;
// }

// //各种类型的中间代码的输出函数
// map<ic_op,ic_output> pool_ic_outputs={
//     {ic_op::NOP,pool_output_nop_ic},                         //空语句
//     {ic_op::ASSIGN,pool_output_one_operand_ic},              //赋值
//     {ic_op::ADD,pool_output_two_operands_ic},                    //加
//     {ic_op::SUB,pool_output_two_operands_ic},                    //减
//     {ic_op::MUL,pool_output_two_operands_ic},                    //乘
//     {ic_op::DIV,pool_output_two_operands_ic},                    //除
//     {ic_op::MOD,pool_output_two_operands_ic},                    //取模
//     // {ic_op::L_SHIFT,pool_output_two_operands_ic},            //左移位
//     // {ic_op::R_SHIFT,pool_output_two_operands_ic},            //右移位
//     {ic_op::NOT,pool_output_one_operand_ic},                    //非
//     // {ic_op::AND,pool_output_two_operands_ic},                    //与
//     // {ic_op::OR,pool_output_two_operands_ic},                      //或
//     // {ic_op::XOR,pool_output_two_operands_ic},                    //异或
//     {ic_op::EQ,pool_output_two_operands_ic},                      //是否相等
//     {ic_op::UEQ,pool_output_two_operands_ic},                    //是否不相等
//     {ic_op::GT,pool_output_two_operands_ic},                      //比较是否大于
//     {ic_op::LT,pool_output_two_operands_ic},                      //比较是否小于
//     {ic_op::GE,pool_output_two_operands_ic},                      //比较是否大于等于
//     {ic_op::LE,pool_output_two_operands_ic},                      //比较是否小于等于
//     {ic_op::JMP,pool_output_zero_operand_ic},                    //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
//     {ic_op::IF_JMP,pool_output_one_operand_ic},              //结果为true的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
//     {ic_op::IF_NOT_JMP,pool_output_one_operand_ic},      //结果为false的时候进行有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码）
//     {ic_op::VAR_DEFINE,pool_output_var_define_ic},      //变量定义（可以赋初始值，也可以不赋初始值）
//     {ic_op::FUNC_DEFINE,pool_output_func_define_ic},    //函数定义（第一操作数是要定义的函数名，第二个操作数是形参列表，没有结果）
//     {ic_op::END_FUNC_DEFINE,pool_output_end_func_define_ic},    //函数定义（第一操作数是要定义的函数名，第二个操作数是形参列表，没有结果）
//     {ic_op::LABEL_DEFINE,pool_output_label_define_ic},  //定义一个LABEL
//     {ic_op::CALL,pool_output_call_ic},                  //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
//     {ic_op::RET,pool_output_return_ic},                    //函数返回（只有结果，没有操作数，结果就是返回值）
// };

/*
    池元素的函数们
*/
// 为 变量 和 数组成员创建 池元素
pool_item::pool_item(pool* related_pool, ic_data* arg):related_pool(related_pool){
    if( arg->is_array_member() ){
        this->type = pool_item_type::ARRAY_MEMBER;
    }else{
        this->type = pool_item_type::VAR_OR_TMP;
    }
    this->arg1 = arg;
    this->arg2 = nullptr;
    this->arg1_pi = this;
    this->arg1_pi = nullptr;
    this->op = ic_op::NOP;
    this->my_class = nullptr;
}

// 为表达式创建池元素
pool_item::pool_item(pool* related_pool, ic_op op, ic_data* arg1, ic_data* arg2):related_pool(related_pool){

    this->type = pool_item_type::EXPRESSION;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg1_pi = related_pool->get_pi_of_non_exp(arg1);
    this->arg2_pi = related_pool->get_pi_of_non_exp(arg2);
    this->op = op;
    this->my_class = nullptr;
}

// 设置等价类
void pool_item::set_class(eq_class* eqc){
    this->my_class = eqc;
}

// 设置操作数 1 的池元素
void pool_item::set_arg1_pi(pool_item* pi){
    this->arg1_pi = pi;
} 

// 设置操作数 2 的池元素
void pool_item::set_arg2_pi(pool_item* pi){
    this->arg2_pi = pi;
}

// 判断池元素是不是 变量、临时变量、数组变量或者常量
bool pool_item::is_var(){
    return this->type == VAR_OR_TMP;
}

// 判断池元素是不是数组成员
bool pool_item::is_am(){
    return this->type == ARRAY_MEMBER;
}

// 判断池元素是不是表达式
bool pool_item::is_exp(){
    return this->type == EXPRESSION;
}

// 返回所属等价类指针
eq_class* pool_item::get_class(){
    return this->my_class;
}
// 返回池元素的类型
pool_item_type pool_item::get_type(){
    return this->type;
}
// 返回池元素的操作符
ic_op pool_item::get_op(){
    return this->op;
}
ic_data* pool_item::get_arg1(){
    return this->arg1;
}
ic_data* pool_item::get_arg2(){
    return this->arg2;
}
pool_item* pool_item::get_arg1_pi(){
    return this->arg1_pi;
}
pool_item* pool_item::get_arg2_pi(){
    return this->arg2_pi;
}

/*
    等价类的函数们
*/
// 创建一个等价类并将其放入池中.
eq_class::eq_class(pool* p):related_pool(p){
    this->class_items = {};
    p->add_eq_class(this);
}

bool eq_class::if_pi_in_eqc(pool_item* pi){
    return std::find(this->class_items.begin(), this->class_items.end(), pi) != this->class_items.end();
}

// 向等价类添加池元素
void eq_class::add_class_item(pool_item* pi){
    
    if( !this->if_pi_in_eqc(pi) ){
        // printf("in func < eq_class::add_class_item > class: %lx\n", pi->get_class());
        // print_pi(pi);
        this->class_items.push_back(pi);
    }
}

// 从等价类移除池元素
void eq_class::rm_class_item(pool_item* pi){
    if( pi != nullptr ){
        // printf("in func < eq_class::rm_class_item > class: %lx\n", pi->get_class());
        // print_pi(pi);
    }
    this->class_items.remove(pi);   // 删除值为 pi 的元素
}     

// 返回等价类里池元素的个数
int eq_class::get_class_size(){
    return this->class_items.size();
}                           

// 获取等价类里的池元素
list< pool_item* > eq_class::get_class_items(){
    return this->class_items;
}

// 在池里找到代价最小的等价池元素, 没有找到则返回 nullptr
// 此处实现了一个简单的版本, 可以扩充
pool_item* eq_class::get_cheapest_eq_pi(){

    // printf("in func < eq_class::get_cheapest_eq_pi >\n");
    // for( auto item : this->class_items ){
    //     print_pi(item);
    // }
    // for( auto item : this->class_items ){
    //     if( item->is_var() ){
    //         return item;
    //     }
    // }

    for( auto item = this->class_items.rbegin(); item != this->class_items.rend(); ++item ){
        if( (*item)->is_var() ){
            return *item;
        }
    }
    return nullptr;
}

// 判断一个等价类是否有保留的价值
// 一个等价类, 如果同时具有 var 类型 和 array_member/expression 类型, 则它是值得保留的
bool eq_class::worth_keeping(){
    bool var_flag = false, am_exp_flag = false;
    for( auto pi: this->class_items ){
        if( pi->is_var() ){
            var_flag = true;
        }
        if( pi->is_exp() || pi->is_am() ){
            am_exp_flag = true;
        }
        if( var_flag && am_exp_flag ){
            break;
        }
    }
    return var_flag && am_exp_flag;
}

/*
    池的函数们
*/
// 创建一个池, 池的关联基本块为传入的基本块
pool::pool(ic_basic_block* blk){
    this->new_pool = true;
    this->related_blk = blk;
    this->pool_items = {};
    this->eq_classes = {};
    this->non_exp_to_pi = {};
    this->exp_to_pi = {};
    this->array_member_to_class = {};
    this->exp_to_class = {};
    this->use_info = {};
}

// 复制一个池, 复制得到的池为旧池, 池的 关联基本块 为 传入池的关联基本块
pool::pool(pool* source){
    
    this->new_pool = false;
    this->related_blk = source->related_blk;
    this->pool_items = {};
    this->eq_classes = {};
    this->non_exp_to_pi = {};
    this->exp_to_pi = {};
    this->array_member_to_class = {};
    this->exp_to_class = {};
    this->use_info = {};

    if( source->get_pi_count() == 0 ){
        return;
    }

    map< pool_item*, pool_item* > pi_map;
    map< eq_class* , eq_class*  > eqc_map;
    pi_map[nullptr] = nullptr;
    eqc_map[nullptr] = nullptr;

    // printf("1  重新创建所有的池元素\n");
    pool_item* pi;
    for( auto old_pi: source->pool_items ){
        if( old_pi->is_exp() ){
            pi = new pool_item(this, old_pi->get_op(), old_pi->get_arg1(), old_pi->get_arg2());
        }else{
            pi = new pool_item(this, old_pi->get_arg1()); 
        }
        pi_map[old_pi] = pi;
        this->pool_items.insert(pi);
    }

    // printf("2  重新创建所有等价类\n");
    eq_class* eqc;
    for( auto old_eqc: source->eq_classes ){
        eqc = new eq_class(this);
        eqc_map[old_eqc] = eqc;
        this->eq_classes.insert(eqc);
    }

    // printf("9  完善 池元素的信息, 需要修改的有 arg1_pi, arg2_pi 和 my_class\n");
    for( auto pi: source->pool_items ){
        pi_map[pi]->set_arg1_pi(this->get_pi_of_non_exp(pi->get_arg1()));
        pi_map[pi]->set_arg2_pi(this->get_pi_of_non_exp(pi->get_arg2()));
        pi_map[pi]->set_class(eqc_map[pi->get_class()]);
    }

    // printf("10  完善 等价类 的信息, 需要修改的有 class_items\n");
    for( auto eqc: source->eq_classes ){
        for( auto pi: eqc->get_class_items() ){
            eqc_map[eqc]->add_class_item(pi_map[pi]);
        }
    }

    // printf("3  复制 non_exp_to_pi 和 exp_to_pi\n");
    for( auto item: source->non_exp_to_pi ){
        this->non_exp_to_pi[item.first] = pi_map[item.second];
    }
    for( auto item: source->exp_to_pi ){
        this->exp_to_pi[item.first] = pi_map[item.second];
    }

    // printf("6  复制 array_member_to_class\n");
    for( auto item: source->array_member_to_class ){
        ic_data* arg = get<0>(item.first);
        eq_class* offset_eqc = eqc_map[get<1>(item.first)];
        this->array_member_to_class[tuple(arg, offset_eqc)] = eqc_map[item.second];   
    }

    // printf("7  复制 exp_to_class\n");
    for( auto item: source->exp_to_class ){
        ic_op op = get<0>(item.first);
        eq_class* arg1_eqc = eqc_map[get<1>(item.first)];
        eq_class* arg2_eqc = eqc_map[get<2>(item.first)];
        this->exp_to_class[tuple(op, arg1_eqc, arg2_eqc)] = eqc_map[item.second];
    }

    // printf("8  复制使用信息\n");
    for( auto item: source->use_info ){
        ic_data* arg = item.first;
        this->use_info[arg] = new set<pool_item*>;
        for( auto pi: *(item.second) ){
            this->use_info[arg]->insert(pi_map[pi]);
        }
    }

    // printf("pool_test: pool_copy\n");
    // this->pool_test();
    // printf("PASS\n");

}

// 删去一个池
pool::~pool(){
    for( auto pi:this->pool_items ){
        delete pi;
    }
    for( auto eqc:this->eq_classes ){
        delete eqc;
    }
}

// 标记一个池为旧池
void::pool::set_pool_old(){
    this->new_pool = false;
}

// 返回池内池元素的个数
int pool::get_pi_count(){
    return this->pool_items.size();
}

// 返回池内等价类的个数
int pool::get_eqc_count(){
    return this->eq_classes.size();
}

// 判断是否是新池, 新池指没有进行过迭代的池
bool pool::is_pool_new(){
    return this->new_pool;
}

bool pool::if_pi_in_pool(pool_item* pi){
    return std::find(this->pool_items.begin(), this->pool_items.end(), pi) != this->pool_items.end();
}

bool pool::if_eqc_in_pool(eq_class* eqc){
    return std::find(this->eq_classes.begin(), this->eq_classes.end(), eqc) != this->eq_classes.end();
}

// 将池元素切换到新的等价类
void pool::switch_class(pool_item* pi, eq_class* new_eqc){

    pi->get_class()->rm_class_item(pi);
    pi->set_class(new_eqc);
    pi->get_class()->add_class_item(pi);

    if( pi->is_am() ){
        ic_data* array_var = pi->get_arg1()->get_belong_array();
        eq_class* offset_eqc = this->get_eqc_of_var(pi->get_arg1()->get_offset());
        this->array_member_to_class[tuple(array_var, offset_eqc)] = pi->get_class();
    }
}    

// 得到一个非表达式 ic_data* 对应的池元素, 若没有则返回空
pool_item* pool::get_pi_of_non_exp(ic_data* arg){
    if( this->non_exp_to_pi.find(arg) != this->non_exp_to_pi.end() ){
        return this->non_exp_to_pi[arg];
    }
    return nullptr;
}

// 得到一个表达式 对应的池元素, 若没有则返回空
pool_item* pool::get_pi_of_exp(ic_op op, ic_data* arg1, ic_data* arg2){
    if( this->exp_to_pi.find(tuple(op, arg1, arg2)) != this->exp_to_pi.end() ){
        return this->exp_to_pi.at(tuple(op, arg1, arg2));
    }
    return nullptr;
}

// 得到一个变量对应的等价类, 若没有则返回 null
eq_class* pool::get_eqc_of_var(ic_data* arg){

    // printf("\t\t\t\t\t\tin func < pool::get_eqc_of_var >\n");
    if( arg != nullptr && !arg->is_array_member()){
        pool_item* pi = this->get_pi_of_non_exp(arg);
        if( pi != nullptr ){
            return pi->get_class();
        }
    }
    return nullptr;
}

// 获得一个数组成员的等价类, 传入偏移量的等价类作为参数
// 没有找到返回 nullptr
eq_class* pool::get_eqc_of_am_by_offset_eqc(ic_data* array_var, eq_class* offset_eqc){
    // printf("\t\t\t\t in func < get_eqc_of_am_by_offset_eqc >\n");
    if( array_var != nullptr && array_var->is_array_var() ){
        auto key = tuple(array_var, offset_eqc);
        if( this->array_member_to_class.find(key) != this->array_member_to_class.end() ){
            return this->array_member_to_class.at(key);
        }
    }
    return nullptr;
}

// 得到一个数组元素对应的等价类, 若没有则返回 null
eq_class* pool::get_eqc_of_am_by_ic_data(ic_data* arg){
    // printf("\t\t\t\t\t\toffset eqc: %lu\n", offset_eqc);
    if( arg != nullptr && arg->is_array_member() ){
        ic_data* belong_array = arg->get_belong_array();
        eq_class* offset_eqc = this->get_eqc_of_var(arg->get_offset());
        if( offset_eqc != nullptr ){
            return get_eqc_of_am_by_offset_eqc(belong_array, offset_eqc);
        }
    }
    return nullptr;
}

// 得到一个非表达式对应的等价类, 若没有则返回 null
eq_class* pool::get_eqc_of_non_exp_by_ic_data(ic_data* arg){
    // printf("\t\t\t\t\tin func < pool::get_eqc_of_non_exp_by_ic_data >\n");
    if( arg != nullptr ){
        if( arg->is_array_member() )
            return this->get_eqc_of_am_by_ic_data(arg); 
        else
            return this->get_eqc_of_var(arg);
    }
    return nullptr;
} 

// 得到一个表达式对应的等价类, 若没有则返回 nullptr
eq_class* pool::get_eqc_of_exp_by_ic_data(ic_op op, ic_data* arg1, ic_data* arg2){
    eq_class* arg1_eqc = this->get_eqc_of_non_exp_by_ic_data(arg1);
    eq_class* arg2_eqc = this->get_eqc_of_non_exp_by_ic_data(arg2);
    return this->get_eqc_of_exp_by_ic_data_eqc(op, arg1_eqc, arg2_eqc);
}

// 给定操作数的等价类, 返回表达式所在的等价类, 若没有则返回 nullptr
eq_class* pool::get_eqc_of_exp_by_ic_data_eqc(ic_op op, eq_class* arg1_eqc, eq_class* arg2_eqc){
    auto key = tuple(op, arg1_eqc, arg2_eqc);
    if( this->exp_to_class.find(key) != this->exp_to_class.end() ){
        return this->exp_to_class.at(key);
    }
    return nullptr;
}


// 添加使用信息, 将 pi 添加到 arg 的 use_info 里
int pool::add_use_info(ic_data* arg, pool_item* pi){
    if( arg == nullptr || pi == nullptr ){
        return -1;
    }
    // printf("in func < pool::add_use_info(ic_data* arg, pool_item* pi) >\n");
    // string arg_string = pool_output_ic_operand(pair(ic_operand::DATA, arg), false, true).c_str();

    if( this->use_info.find(arg) == this->use_info.end() ){
        this->use_info[arg] = new set<pool_item*>;
    }
    if( this->use_info[arg]->find(pi) == this->use_info[arg]->end()){
        this->use_info[arg]->insert(pi);
    }

    return 1;
}

// 添加使用信息, 将(数组成员)操作数的 pi 添加到 偏移量的 use_info 里 
int pool::add_use_info(pool_item* operand_pi){
    if( operand_pi == nullptr || operand_pi->is_exp()){
        return -1;
    }
    // printf("in func < pool::add_use_info(pool_item* operand_pi) >\n");
    if( operand_pi->is_am() ){
        this->add_use_info(operand_pi->get_arg1()->get_offset(), operand_pi);
        this->add_use_info(operand_pi->get_arg1()->get_belong_array(), operand_pi);
    }
    return 1;
}

// 添加使用信息, 将表达式的 pi 添加到 操作数 和 数组操作数偏移量 的 use_info 里
int pool::add_use_info(pool_item* operand_pi, pool_item* exp_pi){
    if( operand_pi == nullptr || exp_pi == nullptr || operand_pi->is_exp() || !exp_pi->is_exp() ){
        return -1;
    }
    this->add_use_info(operand_pi->get_arg1(), exp_pi);
    if( operand_pi->is_am() ){
        this->add_use_info(this->get_pi_of_non_exp(operand_pi->get_arg1()->get_offset()), exp_pi);
    }
    return 0;
}

// 向池中添加一个池元素. 池元素已经创建完毕, 添加到 池元素集合 和 对应等价类集合 即可
void pool::add_pool_item(pool_item* pi, bool print=false){

    // if( print ){
    //     printf("\t\t\t\t\t\tADD: ");
    //     print_pi(pi);
    // }

    this->pool_items.insert(pi);
    pi->get_class()->add_class_item(pi);

    if(pi->is_var()){   // var 类型的添加, 不会增加 use_info
        this->non_exp_to_pi[pi->get_arg1()] = pi;
    }
    if(pi->is_am()){
        add_use_info(pi);
        this->non_exp_to_pi[pi->get_arg1()] = pi;
        ic_data* array_var = pi->get_arg1()->get_belong_array();
        eq_class* offset_eqc = this->get_eqc_of_var(pi->get_arg1()->get_offset());
        this->array_member_to_class[tuple(array_var, offset_eqc)] = pi->get_class();
    }
    if(pi->is_exp()){
        add_use_info(pi->get_arg1_pi(), pi);
        add_use_info(pi->get_arg2_pi(), pi);
        this->exp_to_pi[tuple(pi->get_op(), pi->get_arg1(), pi->get_arg2())] = pi;
        eq_class *arg1_eqc = (pi->get_arg1_pi() == nullptr) ? nullptr : pi->get_arg1_pi()->get_class();
        eq_class *arg2_eqc = (pi->get_arg2_pi() == nullptr) ? nullptr : pi->get_arg2_pi()->get_class();
        this->exp_to_class[tuple(pi->get_op(), arg1_eqc, arg2_eqc)] = pi->get_class();
    }
}

// 去除 在 non_exp_to_pi 或 exp_to_pi 中的映射
void pool::clear_map_to_pi(pool_item* pi){
    if( pi != nullptr ){
        if(pi->is_var() || pi->is_am()){
            this->non_exp_to_pi.erase(pi->get_arg1());
        }
        if(pi->is_exp()){
            this->exp_to_pi.erase(tuple(pi->get_op(), pi->get_arg1(), pi->get_arg2()));
        }
    }
}

// 去除使用信息
int pool::rm_use_info(ic_data* arg, pool_item* pi){
    if( this->use_info.find(arg) != this->use_info.end() ){
        if( this->use_info[arg]->find(pi) != this->use_info[arg]->end() ){
            this->use_info[arg]->erase(pi);
        }
    }
    return 0;
}

// 去除使用信息
void pool::rm_pi_from_use_info(ic_data* arg, pool_item* pi){
    if( arg == nullptr || pi == nullptr ){
        return;
    }
    auto use_info_ptr = this->get_use_info(arg);
    if( use_info_ptr != nullptr ){
        use_info_ptr->erase(pi);
    }
}

// 去除使用信息
void pool::rm_pi_from_use_info(pool_item* operand_pi, pool_item* exp_pi){
    if( operand_pi == nullptr || exp_pi == nullptr ){
        return;
    }
    rm_pi_from_use_info(operand_pi->get_arg1(), exp_pi);
    if( operand_pi->is_am() ){
        rm_pi_from_use_info(operand_pi->get_arg1()->get_offset(), exp_pi);
    }
}

// 去除使用信息
void pool::rm_pi_from_use_info(pool_item* pi){
    if( pi == nullptr || pi->is_var() ){
        return;
    }
    if( pi->is_am() ){
        rm_pi_from_use_info((ic_data*)(pi->get_arg1()->get_belong_array()), pi);
        rm_pi_from_use_info((ic_data*)(pi->get_arg1()->get_offset()), pi);
    }
    if( pi->is_exp() ){
        rm_pi_from_use_info((pool_item*)(pi->get_arg1_pi()), pi);
        rm_pi_from_use_info((pool_item*)(pi->get_arg2_pi()), pi);
    }
}

// 从池中删去一个池元素. 将池元素从 池元素集合 和 对应等价类集合 移除
void pool::rm_pool_item(pool_item* pi, bool print = false){

    // 一个 表达式 可能在多个操作数的 use_info 里
    // 一个 数组 在其 偏移量 和 array_var 的 use_info 里
    // 综上, 可能存在 pi 被多次释放的情况
    // 为了避免多次释放, 我们需要检查 pi 是不是在 pool 里
    if( pi != nullptr && (this->pool_items.find(pi) != this->pool_items.end()) ){
        // if( print ){
        //     printf("\t\t\t\t\t\tREMOVE: ");
        //     print_pi(pi);
        // }

        // 从 pool_items 移除, 并去除 在 non_exp_to_pi 或 exp_to_pi 中的映射 和 在 use_info 里的信息
        this->pool_items.erase(pi);
        this->clear_map_to_pi(pi);
        this->rm_pi_from_use_info(pi);

        // 从所在等价类移除
        pi->get_class()->rm_class_item(pi);

        if( pi->is_am() ){
            this->use_info[pi->get_arg1()->get_belong_array()]->erase(pi);
            this->array_member_to_class.erase(tuple(pi->get_arg1()->get_belong_array(), this->get_eqc_of_var(pi->get_arg1()->get_offset())));  
        }

        delete pi;
    }
}

// 为表达式创建一个池元素
eq_class* pool::create_pool_item_for_exp(ic_op op, ic_data* arg1, ic_data* arg2){

    // printf("\t\t\t\t in func < pool::create_pool_item_for_exp >\n");

    if( this->get_pi_of_exp(op, arg1, arg2) != nullptr ){   // 这个表达式在池里
        return this->get_pi_of_exp(op, arg1, arg2)->get_class();
    }
    else{  
        // 没有在池里
        pool_item* new_pi = new pool_item(this, op, arg1, arg2);
        eq_class* eqc = get_eqc_of_exp_by_ic_data(op, arg1, arg2);  // 是否属于某一存在的等价类
        eq_class* new_eqc = (eqc == nullptr) ? new eq_class(this) : eqc;

        new_pi->set_class( new_eqc );
        this->add_pool_item(new_pi, false);
        return new_pi->get_class();
    }
}

// 为 result 创建 池元素
// 如果 result 是数组元素, 会优先为 偏移量 创建池元素, 再为 result 创建
bool pool::create_pool_item_for_result(eq_class* new_class, ic_data* result, bool language_data_type_same = true){

    // printf("\t\t\t\t in func < pool::create_pool_item_for_result >\n");
    if( result != nullptr ){
        if( result->is_array_member() ){
            this->create_pool_items_by_ic_data(result->get_offset());
        }

        pool_item* result_pi = this->get_pi_of_non_exp(result);     // 尝试找 result 的池元素
        bool class_change = true;

        if( (result_pi != nullptr) && language_data_type_same ){
            eq_class* result_old_eqc = get_eqc_of_non_exp_by_ic_data(result);   // 尝试找 result 以前的等价类
            class_change = (new_class != result_old_eqc);
            if( class_change ){ // 被创建了, 且等价类不同, 切换等价类, 加入新的等价类
                this->switch_class(result_pi, new_class);
            }
        }
        else{                   // 没有被创建 或者 表达式与result类型不一致
            result_pi = new pool_item(this, result);
            result_pi->set_class(new_class);
            this->add_pool_item(result_pi, false);
        }

        // printf("\t\t\t\t\t created: %d, class_change: %d\n", result_created, class_change);
        // printf("\t\t\t\tout func < pool::create_pool_item_for_result >\n");
        return  class_change;
    }else{
        exit(-1);
    }
}

// 返回 ic_data 所在的等价类. 如果 ic_data 本身没有在池中, 创建 pool_item 并放入池中
// 如果 传入参数 为 nullptr, 会返回 nullptr
eq_class* pool::create_pool_items_by_ic_data( ic_data* arg ){

    // printf("\t\t\t\t in func < pool::create_pool_items_by_ic_data >\n");

    if( arg != nullptr ){
        pool_item* pi = this->get_pi_of_non_exp(arg);
        if( pi != nullptr ){  // 在池里
            return pi->get_class();
        }
        else{
            if( arg->is_array_member() ){
                this->create_pool_items_by_ic_data(arg->get_offset());
            }
            pool_item* new_pi = new pool_item(this, arg);    // 创建一个新的池元素
            eq_class* eqc = this->get_eqc_of_non_exp_by_ic_data(arg);
            eq_class* new_eqc = (eqc == nullptr) ? new eq_class(this) : eqc;
            new_pi->set_class( new_eqc );
            this->add_pool_item(new_pi, false);
            return new_pi->get_class();
        }    
    }
    return nullptr;
}

// 返回表达式所在的等价类. 如果 表达式 本身没有在池中, 创建 pool_item 并放入池中
// 可能返回 null 值
eq_class* pool::create_pool_items_for_right( quaternion_with_info ic ){

    // printf("\t\t\t in func < pool::create_pool_items_by_ic >\n");

    ic_op op = ic.intermediate_code.op;
    ic_data* arg1;
    ic_data* arg2;
    eq_class* arg1_eqc;
    eq_class* arg2_eqc;

    eq_class* ret_eqc = nullptr;
    switch (op)
    {
        case ic_op::ASSIGN:
        case ic_op::ADD:
        case ic_op::SUB:
        case ic_op::MUL:
        case ic_op::DIV:
        case ic_op::MOD:
        case ic_op::NOT:
            arg1 = (ic_data*)ic.intermediate_code.arg1.second;
            arg2 = (ic_data*)ic.intermediate_code.arg2.second;
            arg1_eqc = this->create_pool_items_by_ic_data((ic_data*)arg1);
            arg2_eqc = this->create_pool_items_by_ic_data((ic_data*)arg2);

            if( op == ic_op::ASSIGN ){
                ret_eqc = arg1_eqc;
            }else{  // 如果不是 ASSIGN
                ret_eqc = this->create_pool_item_for_exp(op, arg1, arg2);
            }
            break;
        default:
            ret_eqc = nullptr;
            break;
    }
    return ret_eqc;
}

// 向池中添加一个等价类
void pool::add_eq_class(eq_class* eqc){
    this->eq_classes.insert(eqc);
}

// 从池中移除一个等价类
void pool::rm_eq_class(eq_class* eqc){
    if( eqc != nullptr && this->if_eqc_in_pool(eqc) ){
        this->eq_classes.erase(eqc);    // 避免反复删除
        delete eqc;
    }
}       

// 移除一个 ic_data 的所有使用信息
int pool::rm_use_infos(ic_data* arg){
    if( this->use_info.find(arg) != this->use_info.end() ){
        this->use_info[arg]->clear();
    }
    return 0;
}

// 获取使用链 或者 数组的使用信息, 如果没有会返回 null
set< pool_item* >* pool::get_use_info(ic_data* arg){
    if( this->use_info.find(arg) != this->use_info.end() ){
        // for( auto pi: this->use_info[arg])
        return this->use_info[arg]; 
    }
    return nullptr;
}

// 杀死使用某个 ic_data 的所有数组成员或者表达式
void pool::kill_uses_of_arg(ic_data* arg){

    // printf("\t\t\t\tin func < pool::kill_uses_of_arg >\n");
    if( arg != nullptr ){
        if( this->get_use_info(arg) != nullptr ){
            set< pool_item* > use_info_copy = *(this->get_use_info(arg));
            for( auto pi: use_info_copy){
                rm_pool_item(pi);
            }
        }
    }
    // printf("\t\t\t\tout func < pool::kill_uses_of_arg >\n");
}

// 判断两个普通变量的关系
pool_item_relation pool::get_relation(ic_data* arg1, ic_data* arg2){

    pool_item* arg1_pi = this->get_pi_of_non_exp(arg1);
    pool_item* arg2_pi = this->get_pi_of_non_exp(arg2);
    if( arg1 == nullptr || arg2 == nullptr || arg1_pi == nullptr || arg2_pi == nullptr ){
        // printf("%lx, %lx, %lx, %lx\n", arg1, arg2, arg1_pi, arg2_pi);
        printf("Error: in function < pool::get_relation >\n");
        exit(-1);
    }

    eq_class *eqc1 = arg1_pi->get_class();
    eq_class *eqc2 = arg2_pi->get_class();
    if( eqc1 == eqc2 ){
        return SAME;    // i 与 j 在同一个等价类.
    }else{
        return NOT_SURE;
    }
}

void pool::deal_call_op(quaternion_with_info ic){
    // 如果 vague_def 里定义了一个 变量, 那我们将该变量移入一个新的等价类, 并将之前的使用全部删除
    // 如果 vague_def 里定义了一个 数组元素, 那我们将该数组相关的所有 数组元素或数组变量 移入新的等价类, 并将之前的使用全部删除
    static Symbol_table * symbol_table=Symbol_table::get_instance();

    ic_func* func = (struct ic_func *)ic.intermediate_code.arg1.second;
    set< ic_data* > func_def_globals_and_f_params = symbol_table->get_func_def_globals_and_f_params(func);
    list< ic_data* >* r_params = (list<struct ic_data * > *)ic.intermediate_code.arg2.second;

    // 如果使用到数组, 将该数组的 所有元素及其使用 从池中移除, 可以优化, 减小 移除范围
    if( r_params != nullptr ){
        for(auto r_param: *r_params){
            ic_data* array_var = nullptr;
            if( (r_param->is_array_var() && this->get_use_info(r_param) != nullptr) ){
                array_var = r_param;
            }
            if( (r_param->is_array_member() && this->get_use_info(r_param->get_belong_array()) != nullptr) ){
                array_var = r_param->get_belong_array();
            }
                
            if( array_var != nullptr ){
                set< pool_item* > use_info_copy = *(this->get_use_info(array_var));
                for( auto pi: use_info_copy){
                    kill_uses_of_arg(pi->get_arg1());
                    rm_pool_item(pi);  
                }   
                this->rm_use_infos(array_var);
            }
        }
    }

    // 如果修改了全局变量, 将 其 与 其使用 从池中移除
    for(auto vague_var:func_def_globals_and_f_params){
        if(vague_var->is_global()){
            if( vague_var->is_array_var() && this->get_use_info(vague_var) != nullptr ){      // 如果是全局数组

                set< pool_item* > use_info_copy = *(this->get_use_info(vague_var));
                for( auto pi: use_info_copy){
                    kill_uses_of_arg(pi->get_arg1());  
                    rm_pool_item(pi);   
                }   
                this->rm_use_infos(vague_var);

            }else{
                kill_uses_of_arg(vague_var);
            }
        }
    }

    // 将 result 从原等价类移到新的等价类
    ic_data* result = (ic_data*)ic.intermediate_code.result.second;
    pool_item* result_pi = this->get_pi_of_non_exp(result);
    if( result_pi != nullptr ){
        this->switch_class(result_pi, new eq_class(this));
        this->kill_uses_of_arg(result);                         // 处理赋值杀死的 pool_item
        if( result->is_array_member() ){
            this->deal_assign_to_array_member(result);          // 处理对数组成员赋值的影响
        }
    }
}

// 处理返回指令的影响, 影响就是没有影响
void pool::deal_ret_op(quaternion_with_info ic){
    // 处理返回指令的影响: 影响就是没有影响
}

void pool::deal_assign_op(quaternion_with_info ic){

    ic_op op = ic.intermediate_code.op;
    if( op != ic_op::ASSIGN ){
        return;
    }
    ic_data* arg1 = (ic_data*)ic.intermediate_code.arg1.second;
    ic_data* result = (ic_data*)ic.intermediate_code.result.second;
    eq_class *result_new_eqc = create_pool_items_for_right( ic );
    bool language_data_type_same;
    bool class_change;

    if( result_new_eqc != nullptr ){
        if( arg1->data_type != result->data_type ){
            language_data_type_same = false;   // 将其设置为 nullptr 后调用 create_pool_item_for_result() 函数, 可以总是为 result 创建一个新的等价类
        }
        class_change = this->create_pool_item_for_result(result_new_eqc, result, language_data_type_same); 
        if( class_change ){                     // 如果 created_flag == true && 新旧等价类不同
            this->kill_uses_of_arg(result);     // 处理赋值杀死的 pool_item
        }
        if( result->is_array_member() ){
            this->deal_assign_to_array_member(result);          // 处理对数组成员赋值的影响
        }
    }
}

// 处理对数组成员赋值的影响
void pool::deal_assign_to_array_member(ic_data* result){

    if( result == nullptr || !result->is_array_member() ){
        return;
    }
    // printf("\t\t\t\tin func < deal_assign_to_array_member >\n");

    ic_data* array_var = result->get_belong_array();
    ic_data* offset1 = result->get_offset();
    ic_data* offset2;
    pool_item_relation relation;
    // 会杀死很多池元素
    if( this->get_use_info(array_var) != nullptr )
    {
        set<pool_item*> use_info_copy = *(this->get_use_info(array_var));
        for( auto pi: use_info_copy )
        {
            if( !(this->if_pi_in_pool(pi)) || pi->get_arg1() == result ){
                continue;       // pi 不在池里, 或者 pi 是 result
            }
 
            offset2 = pi->get_arg1()->get_offset();
            relation = this->get_relation(offset1, offset2);

            if( relation == pool_item_relation::DIFFERENT ){
                continue;
            }
            else{   // 杀掉一些表达式
                kill_uses_of_arg(pi->get_arg1());
                if( relation == pool_item_relation::SAME ){     // 将 a[j] 移入 a[i] 的等价类
                    this->switch_class(pi, this->get_pi_of_non_exp(result)->get_class());   
                }
                if( relation == pool_item_relation::NOT_SURE ){ // 将 a[j] 从 PN_out 中移除     
                    this->rm_pool_item(pi);      
                } 
            }  
        }
    }
    // printf("\t\t\t\tout func < deal_assign_to_array_member >\n");
}

// 单步迭代. 函数内会进行 op 检查
void pool::single_interation(quaternion_with_info ic){

    // printf("\t\tsuccess , get in func < pool::single_interation >\n");
    ic_op op = ic.intermediate_code.op;
    // printf("%s\n", pool_ic_outputs[op](ic.intermediate_code).c_str());
    ic_data* result;
    eq_class *result_new_eqc;
    bool class_change;

    switch (op)
    {
        case ic_op::CALL:
            this->deal_call_op( ic );
            break;
        case ic_op::ASSIGN:
            this->deal_assign_op( ic );
            break;
        case ic_op::ADD:
        case ic_op::SUB:
        case ic_op::MUL:
        case ic_op::DIV:
        case ic_op::MOD:
        case ic_op::NOT:
            result = (ic_data*)ic.intermediate_code.result.second;
            result_new_eqc = create_pool_items_for_right( ic ); // 为表达式创建池元素, 并返回表达式的等价类
            if( result_new_eqc != nullptr ){
                class_change = this->create_pool_item_for_result(result_new_eqc, result); 
                if( class_change ){   // 如果 created_flag == true && 新旧等价类不同
                    this->kill_uses_of_arg(result);      // 处理赋值杀死的 pool_item
                }
                if( result->is_array_member() ){
                    this->deal_assign_to_array_member(result);          // 处理对数组成员赋值的影响
                }
            }
            break;
        default:
            break;
    }
    // printf("\t\tsuccess , get out func < pool::single_interation > with size %lu\n", this->pool_items.size());
    return;
}

bool pool::pool_same_judge(pool* new_p){

    if( this->new_pool ){
        return false;
    }

    map< eq_class*, eq_class* > p1_to_p2;
    pool_item* pi_in_newp;

    bool ret = true;
    auto pool_items_copy = this->pool_items;
    for( auto pi: pool_items_copy ){
        if( pi->is_am() || pi->is_var() ){
            pi_in_newp = new_p->get_pi_of_non_exp(pi->get_arg1());
        }
        if( pi->is_exp() ){
            pi_in_newp = new_p->get_pi_of_exp(pi->get_op(), pi->get_arg1(), pi->get_arg2());
        }

        if( pi_in_newp == nullptr ){    // 没有找到
            ret = false;
            break;
        }else{
            if( p1_to_p2.find(pi->get_class()) == p1_to_p2.end() ){     // 第一次碰到
                p1_to_p2[pi->get_class()] = pi_in_newp->get_class();
            }else{
                if(p1_to_p2[pi->get_class()] != pi_in_newp->get_class()){   // p1 中的等价类 a 映射到 p2 中的等价类 b, 现在等价类 a 又映射到等价类 c, 一对多, 说明不相同
                    ret = false;
                    break;
                }
            }
        }
    }
    // printf("judge : %d\n", ret);
    return ret;
}

// 从池里去掉一些不需要的池元素
void pool::pool_simplify(){
    // printf("Success in func < pool::pool_simplify() >\n");

    if( this->pool_items.size() == 0 ){
        return;
    }

    // int del_count = 0;
    // int tot_count = 0;

    map< eq_class*, bool > if_reserve;
    for( auto eqc: this->eq_classes ){
        if_reserve[eqc] = false;
    }

    for( auto eqc: this->eq_classes ){
        if( eqc->worth_keeping() ){     // 判断一个等价类是否有保留的价值
            if_reserve[eqc] = true;
            for( auto pi: eqc->get_class_items() ){
                if( pi->is_am() ){
                    if_reserve[this->get_eqc_of_var(pi->get_arg1()->get_offset())] = true;  // 数组成员偏移量所在的等价类需要保留
                }
                if( pi->is_exp() ){
                    if_reserve[this->get_eqc_of_non_exp_by_ic_data(pi->get_arg1())] = true; // 操作数 1 所在的等价类需要保留
                    if( operands_need(pi->get_op()) == 2 ){
                        if_reserve[this->get_eqc_of_non_exp_by_ic_data(pi->get_arg2())] = true; // 操作数 2 所在的等价类需要保留
                    }
                }
            }
        }
    }

    // 删除不值得保留的等价类里的元素, 并删除该等价类
    auto eqc_copy = this->eq_classes;
    for( auto eqc: eqc_copy ){
        // tot_count += eqc->get_class_size();
        if( !if_reserve[eqc] ){
            auto pi_copy = eqc->get_class_items();
            // del_count += eqc->get_class_size();
            for( auto pi: pi_copy ){
                this->rm_pool_item(pi);
            }
            this->rm_eq_class(eqc);
        }
    } 
    // printf("Total: %d\n", tot_count);
    // printf("Del: %d\n", del_count);
}

// 为一个基本块, 执行迭代, 并返回得到的出口池
pool* pool::interation(){

    // printf("\tsuccess , get in func < pool::interation > %lx\n", this->related_blk );
    // printf("\t\tpn_out 初始大小: %d\n", this->get_pi_count());

    pool* pn_out = new pool(this);
    for( auto ic:this->related_blk->ic_sequence ){
        pn_out->single_interation(ic);
    }
    pn_out->pool_simplify();        // 从池里去掉一些不需要的池元素
 
    // printf("\tend < pool::interation >\n");
    return pn_out;
}

// 不进行 数组成员 和 表达式 的扩展, this 和 p2 都是旧池
pool* pool::normal_intersection( pool* p2){

    map< eq_class*, set<eq_class*> > p1_to_p2;
    map< tuple< eq_class*, eq_class* >, eq_class* > p1_p2_to_pn;
    eq_class *eqc_in_p1, *eqc_in_p2, *eqc_in_pn;

    pool* pn = new pool(this->related_blk);     // 创建一个新的池用以存储交运算的结果
    pn->set_pool_old();                         // 标记为旧池

    for( auto pi: this->pool_items )
    {
        if( pi->is_var() )      // 先处理变量
        {
            eqc_in_p1 = this->get_eqc_of_var(pi->get_arg1());
            eqc_in_p2 = p2->get_eqc_of_var(pi->get_arg1());
            if( eqc_in_p2 )     // p2 里有这个东西
            {   
                if( p1_p2_to_pn.find(tuple(eqc_in_p1, eqc_in_p2)) != p1_p2_to_pn.end() ){   
                    eqc_in_pn = p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)];   // 之前存过这个映射, 它对应 pn 里的等价类 eqc_in_pn
                }else{
                    eqc_in_pn = new eq_class(pn);                           // 之前没有保存这个映射, 为 tuple(eqc_in_p1, eqc_in_p2) 在 pn 里新建等价类
                    pn->add_eq_class(eqc_in_pn);
                    p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)] = eqc_in_pn;
                }
                pool_item* new_pi = new pool_item(pn, pi->get_arg1());  
                new_pi->set_class(eqc_in_pn);   // 设置所属的等价类
                pn->add_pool_item(new_pi);
            }
        }
    }

    // 对 数组元素, 处理方法类似
    for( auto pi: this->pool_items ){
        if( pi->is_am() ){              // 然后处理数组成员
            eqc_in_p1 = this->get_eqc_of_am_by_ic_data(pi->get_arg1());
            eqc_in_p2 = p2->get_eqc_of_am_by_ic_data(pi->get_arg1());
            if( eqc_in_p2 )     // p2 里有这个东西
            {   
                if( p1_p2_to_pn.find(tuple(eqc_in_p1, eqc_in_p2)) != p1_p2_to_pn.end() ){
                    eqc_in_pn = p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)];
                }else{
                    eqc_in_pn = new eq_class(pn);
                    pn->add_eq_class(eqc_in_pn);
                    p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)] = eqc_in_pn;
                }
                pool_item* new_pi = new pool_item(pn, pi->get_arg1());
                new_pi->set_class(eqc_in_pn);
                pn->add_pool_item(new_pi);
            }
        }
    }

    // 对 表达式, 处理方法类似
    for( auto pi: this->pool_items ){
        if( pi->is_exp() ){         // 最后处理表达式
            eqc_in_p1 = this->get_eqc_of_exp_by_ic_data(pi->get_op(), pi->get_arg1(), pi->get_arg2());
            eqc_in_p2 = p2->get_eqc_of_exp_by_ic_data(pi->get_op(), pi->get_arg1(), pi->get_arg2());
            if( eqc_in_p2 )         // p2 里有这个东西
            {   
                if( p1_p2_to_pn.find(tuple(eqc_in_p1, eqc_in_p2)) != p1_p2_to_pn.end() ){
                    eqc_in_pn = p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)];
                }else{
                    eqc_in_pn = new eq_class(pn);
                    pn->add_eq_class(eqc_in_pn);
                    p1_p2_to_pn[tuple(eqc_in_p1, eqc_in_p2)] = eqc_in_pn;
                }
                pool_item* new_pi = new pool_item(pn, pi->get_op(), pi->get_arg1(), pi->get_arg2());
                new_pi->set_class(eqc_in_pn);
                pn->add_pool_item(new_pi);
            }
        }
    }
    return pn;      // 返回交运算的结果
}


// 执行池的交运算, 并返回交运算的结果
pool* pool::pool_intersection( pool* p2 ){

    // printf("\tsuccess , get in func < pool::pool_intersection >\n");
    pool* pn;
    if( p2->is_pool_new() ){
        p2->set_pool_old();     // p2必须是旧池, 才能够进行交运算
    }
    if( this->new_pool ){       // 如果是一个新池, 复制一份 pn_out 作为 pn_in
        pn = p2->pool_intersection(p2);         // 自己交自己
        pn->related_blk = this->related_blk;    // 修改绑定的基本块
    }else{
        pn = this->normal_intersection(p2);
    }

    // printf("\t\t%d -> %d\n", this->get_pi_count(), pn->get_pi_count());
    // pn->pool_simplify();    // 池简化, 交运算结束应该不需要简化
    return pn;
}

// 用变量替换数组成员. 如果 pi 为 nullptr， 返回 origin_ic
quaternion_with_info eliminate_array_member_by_var(quaternion_with_info origin_ic, pool_item* pi, int idx){
    if( pi != nullptr ){
        ic_op op = origin_ic.intermediate_code.op;
        pair< ic_operand, void* > arg1 = origin_ic.intermediate_code.arg1;
        pair< ic_operand, void* > arg2 = origin_ic.intermediate_code.arg2;
        pair< ic_operand, void* > result = origin_ic.intermediate_code.result;
        if( idx == 1){
            return quaternion_with_info(quaternion(op, ic_operand::DATA, (void* )pi->get_arg1(), arg2.first, arg2.second, result.first, result.second));
        }else if( idx == 2){
            return quaternion_with_info(quaternion(op, arg1.first, arg1.second, ic_operand::DATA, (void* )pi->get_arg1(), result.first, result.second));
        }
    }
    return origin_ic;
}

// 用变量替换表达式. 如果 pi 为 nullptr， 返回 origin_ic
quaternion_with_info eliminate_expression_by_var(quaternion_with_info origin_ic, pool_item* pi){
    if( pi != nullptr ){
        return quaternion_with_info(quaternion(ic_op::ASSIGN,ic_operand::DATA, (void* )pi->get_arg1(),ic_operand::NONE, (void* )nullptr,origin_ic.intermediate_code.result.first,origin_ic.intermediate_code.result.second));
    }
    return origin_ic;
}


int eliminate_common_exp_in_basic_block(ic_basic_block* blk, pool* p_in){

    for( auto & ic: blk->ic_sequence ){
        ic_op op = ic.intermediate_code.op;
        ic_data* arg1 = (ic_data*)ic.intermediate_code.arg1.second;
        ic_data* arg2 = (ic_data*)ic.intermediate_code.arg2.second;
        ic_data* result = (ic_data*)ic.intermediate_code.result.second;

        pair< ic_operand, void* > pair_arg1 = ic.intermediate_code.arg1;
        pair< ic_operand, void* > pair_arg2 = ic.intermediate_code.arg2;
        pair< ic_operand, void* > pair_result = ic.intermediate_code.result;

        pool_item* arg1_pi = p_in->get_pi_of_non_exp(arg1);
        pool_item* arg2_pi = p_in->get_pi_of_non_exp(arg2);
        eq_class* exp_eqc = p_in->get_eqc_of_exp_by_ic_data(op, arg1, arg2);

        // printf("before %d\t\tblk: %lx", ic.intermediate_code.op, blk);
        // printf("\t\t\t%s\n", pool_ic_outputs[op](ic.intermediate_code).c_str());

        // printf("after %d\t\tblk: %lx", ic.intermediate_code.op, blk);
        // printf("\t\t\t%s\n\n", pool_ic_outputs[ic.intermediate_code.op](ic.intermediate_code).c_str());

        switch (op)
        {
            case ic_op::ASSIGN:
                // 可以替换一些操作数之类的, 1 种情形
                if( arg1 != nullptr && arg1_pi != nullptr && arg1_pi->is_am() ){   // 换不了表达式, 尝试换一下操作数
                    pool_item* pi = arg1_pi->get_class()->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        // ic = eliminate_array_member_by_var(ic, pi, 1);                      
                    }
                }
                break;
            case ic_op::EQ:
            case ic_op::UEQ:
            case ic_op::GT:
            case ic_op::LT:
            case ic_op::GE:
            case ic_op::LE:
                // 可以替换一些操作数之类的, 6 种情形
                if( arg1 != nullptr && arg1_pi != nullptr && arg1_pi->is_am() ){   // 换不了表达式, 尝试换一下操作数
                    pool_item* pi = arg1_pi->get_class()->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        // ic = eliminate_array_member_by_var(ic, pi, 1);
                    }
                }
                if( arg2 != nullptr && arg2_pi != nullptr && arg2_pi->is_am() ){   // 换不了表达式, 尝试换一下操作数
                    pool_item* pi = arg2_pi->get_class()->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        // ic = eliminate_array_member_by_var(ic, pi, 2);
                    }
                }
                break;
        
            case ic_op::ADD:
            case ic_op::SUB:
            case ic_op::MUL:
            case ic_op::DIV:
            case ic_op::MOD:
            case ic_op::NOT:
                // 6 种情形
                if( exp_eqc != nullptr){ // 表达式在池里
                    pool_item* pi = exp_eqc->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        ic = eliminate_expression_by_var(ic, pi);
                    }
                }
                else if( arg1 != nullptr && arg1_pi != nullptr && arg1_pi->is_am() ){   // 换不了表达式, 尝试换一下操作数
                    pool_item* pi = arg1_pi->get_class()->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        // ic = eliminate_array_member_by_var(ic, pi, 1);
                    }
                }
                else if( arg2 != nullptr && arg2_pi != nullptr && arg2_pi->is_am() ){   // 换不了表达式, 尝试换一下操作数
                    pool_item* pi = arg2_pi->get_class()->get_cheapest_eq_pi();
                    if( pi != nullptr ){
                        // ic = eliminate_array_member_by_var(ic, pi, 2);
                    }
                }
                break;

            default:
                break;
        }
        p_in->single_interation(ic);    // 更新迭代
    }
    return 0;
}

// 需要操作数的个数. 特别的, 跳转、定义、 call 和 ret 会得到返回值 0
int operands_need(ic_op op){
    int ret = 0;
    switch (op)
    {
        case ic_op::ASSIGN:
        case ic_op::NOT:
            ret = 1;
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
            ret = 2;
            break;

        default:
            ret = 0;
            break;
    }
    return ret;
}

bool commutative(ic_op op){     // 判断是不是可交换的运算
    return (op == ic_op::ADD) || (op==ic_op::MUL) || (op==ic_op::EQ) || (op==ic_op::UEQ);
}

// void print_non_exp(pool_item* pi){
//     string str = "\t\t\t\t\t";
//     str = str + pool_output_ic_operand(pair(ic_operand::DATA, pi->get_arg1()), false, true).c_str();
//     printf("%s", str.c_str());
// }

// void print_exp(pool_item* pi){

//     ic_op op = pi->get_op();
//     if( op == ic_op::ASSIGN || op == ic_op::NOT ){
//         string str = "\t\t\t\t\t";
//         str = str + pool_ic_op_output_map[op].c_str() + "\t\t";
//         str = str + pool_output_ic_operand(pair(ic_operand::DATA, pi->get_arg1()), false, true).c_str();
//         printf("%s", str.c_str());
//     }
//     if( (op >= ic_op::ADD && op < ic_op::NOT) || (op > ic_op::NOT && op <= ic_op::LE)){
//         string str = "\t\t\t\t\t";
//         str = str + pool_ic_op_output_map[op].c_str() + "\t\t";
//         str = str + pool_output_ic_operand(pair(ic_operand::DATA, pi->get_arg1()), false, true).c_str() + "\t\t";
//         str = str + pool_output_ic_operand(pair(ic_operand::DATA, pi->get_arg2()), false, true).c_str();
//         printf("%s", str.c_str());
//     }
// }

// void print_pi(pool_item* pi){
//     if( pi->is_var() || pi->is_am() ){
//         print_non_exp(pi);
//     }
//     if( pi->is_exp() ){
//         print_exp(pi);
//     }
//     printf("\n");
// }

// void print_eqc(eq_class* eqc){
//     if( eqc != nullptr ){
//         auto pi_copy = eqc->get_class_items();
//         for( auto pi: pi_copy ){
//             print_pi(pi);
//         }
//     }
// }

// void pool::print_pool(){

//     // printf("\n\t\t\t\t\t%lx\n", (unsigned long)this->related_blk);
//     // printf("EQ_CLASSES:\n");
//     for( auto eqc: this->eq_classes ){
//         printf("\t\t\t\tEQC: %llx, size = %lu\n", (long long)eqc, eqc->get_class_items().size());
//         print_eqc(eqc);
//     }

// }

// void pool::pool_item_nullptr_test(){
//     for( auto pi:this->pool_items ){
//         if( pi == nullptr ){
//             printf("Fail: pool_item_nullptr_test\n");
//             exit(-1);
//         }
//     }
// }

// void pool::eqc_nullptr_test(){
//     for( auto eqc: this->eq_classes ){
//         if( eqc == nullptr ){
//             printf("Fail: eqc_nullptr_test\n");
//             exit(-1);
//         }
//     }
// }

// void pool::pi_shouheng_test(){
//     size_t sum1 = this->pool_items.size();
//     size_t sum2 = 0;
//     for( auto eqc: this->eq_classes ){
//         sum2 += eqc->get_class_items().size();
//     }
//     if( sum1 != sum2 ){
//         //this->print_pool();
//         //printf("Fail: pi_shouheng_test\n");
//         exit(-1);
//     }
// }

// void pool::pool_test(){
//     this->pool_item_nullptr_test();
//     this->eqc_nullptr_test();
//     this->pi_shouheng_test();
// }