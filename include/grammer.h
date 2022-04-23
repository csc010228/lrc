/*
 *
 *  grammer.h
 *  文法的类
 *  Creator:Sichao Chen
 *  Create time:2021/10/23
 *
*/
#ifndef __GRAMMER_H
#define __GRAMMER_H

#include<vector>
#include<string>
#include<map>
#include<set>

#include "semantic_rules.h"

using namespace std;

//语法符号（包括语法变量和终结符）编号
typedef int syntax_symbol_index;

//终结符编号
typedef int terminator_index;

//语法变量编号
typedef int syntax_variable_index;

//产生式编号
typedef int production_index;

//文法的产生式
struct production
{
    syntax_symbol_index left;                       //产生式左部的那个语法变量的编号
    vector<syntax_symbol_index> right;              //产生式右部的终结符和语法变量的编号数组
    string production_string;                       //产生式字符串
};

//文法
class Grammer
{

protected:
    //是否是拓广文法
    bool is_augmented_;

    //所有的语法符号的FIRST集
    set<syntax_symbol_index> ** firsts_;

    //所有的语法变量的FOLLOW集
    set<syntax_symbol_index> ** follows_;

    //能派生出空符号的语法变量的标志数组(1表示可以,0表示不行,-1表示还没有填入内容)
    bool *syntax_variables_derive_empty_;

    //获取所有的能够派生出空符号的语法变量
    void get_all_symbol_variables_derive_empty();

    //判断一个语法符号能不能派生出空符号
    bool derive_empty(syntax_symbol_index syn_sym);

    //判断一个符号串能不能派生出空符号
    bool string_derive_empty(vector<syntax_symbol_index> string);

    //将一个语法符号集合去除空符号
    set<syntax_symbol_index> set_difference_empty(set<syntax_symbol_index> syntax_symbol_set);

    //获取语法符号的FIRST集
    set<syntax_symbol_index> * first(syntax_symbol_index syn_sym);

    //获取语法符号的FOLLOW集
    set<syntax_symbol_index> * follow(syntax_symbol_index syn_var);

    //获取整个文法的所有语法符号的FIRST集
    void get_all_firsts();

    //获取整个文法的所有语法变量的FOLLOW集
    void get_all_follows();

    //生成一个新产生的语法变量的名字
    string generate_a_new_syntax_variable_name_();

public:
    //终结符个数
    int terminator_num_;

    //语法变量个数
    int syntax_variable_num_;

    //语法符号个数
    int syntax_symbol_num_;
    
    //语法变量
    map<string,syntax_symbol_index> syntax_variables_;

    //终结符
    map<string,syntax_symbol_index> terminators_;

    //产生式
    vector<struct production * > * productions_;

    //文法的起始符号的编号
    syntax_symbol_index start_symbol_;

    //空符号的编号
    syntax_symbol_index empty_symbol_;

    //栈底符号的编号
    syntax_symbol_index stack_bottom_symbol_;

    //每一个产生式和语义动作函数的对应表
    vector<semantic_rule> production_semantic_rule_;

    //构造函数
    Grammer(const char * filename,map<string,syntax_symbol_index> terminators,bool is_augmented);

    //析构函数
    ~Grammer();

    //查看该文法是否建立成功
    bool is_init_success();

    //获取一个符号串的FIRST集
    set<syntax_symbol_index> string_first(vector<syntax_symbol_index> string);
    
};

#endif  //__GRAMMER_H