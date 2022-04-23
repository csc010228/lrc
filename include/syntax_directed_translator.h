/*
 *
 *  syntax_directed_translator.h
 *  LR(1)语法分析对应的语法制导翻译
 *  Creator:Sichao Chen
 *  Create time:2021/11/04
 *
*/
#ifndef __SYNTAX_DIRECTED_TRANSLATOR_H
#define __SYNTAX_DIRECTED_TRANSLATOR_H

#include "LR1_parser.h"
#include "semantic_rules.h"

using namespace std;

//定义LR(1)型的自底向上的语法分析对应的语法制导翻译
class Syntax_directed_translator:public LR1_parser
{
private:
    //语义栈(每一个map的键都是属性的字符串,值是属性值)
    vector<map<string,union semantic_stack_node_member> > * semantic_stack_;

    //语义栈的栈顶指针
    stack_index stack_top_;

    //查看语义栈是否需要扩展
    void check_semantic_stack();

    //往语义栈中插入
    void push_semantic_stack(map<string,union semantic_stack_node_member> semantic_stack_node_member);

    //往语义栈中插入
    void push_semantic_stack(string key,union semantic_stack_node_member value);

    //往语义栈中插入
    void push_semantic_stack();

    //从语义栈中弹出
    void pop_semantic_stack();

public:
    //构造函数
    Syntax_directed_translator();

    //初始化
    bool init(const char * productions_filename,const char * parser_table_filename,list<struct token * > * tokens,bool print_warning);

    //析构函数
    ~Syntax_directed_translator();

    //进行语义翻译
    list<struct quaternion> * translate();

    //进行语义翻译并将中间代码进行输出
    list<struct quaternion> * translate_then_output(const char * filename);
};

#endif //__SYNTAX_DIRECTED_TRANSLATOR_H