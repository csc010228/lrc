/*
 *
 *  lexical_analyzer.h
 *  词法分析器
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/
#ifndef __LEXICAL_ANALYZER_H
#define __LEXICAL_ANALYZER_H

#include<stdio.h>
#include<iostream>
#include<string>
#include<list>
#include<map>
#include<stack>
#include "my_error.h"
#include "language.h"

#define BUF_SIZE 1024                   //双缓冲区机制中单个缓冲区能容纳的字符个数(包括缓冲区末尾的EOF)(这个值一定要大于等于9，因为最长的关键字长度就是8)
#define MAX_LEN_OF_ID BUF_SIZE-1        //单个保留字的最大长度就是单个缓冲区的长度减1(这里需要减1是因为在词法分析的时候会超前搜索一个单词)

#define MAX_RETRACT_NUM 2       //允许的最大回退字符个数

using namespace std;

//定义数字的类型
enum class digit_type
{
    DIGIT,              //普通的0到9的数字
    NOZERO,             //不包含0的1到9的数字
    OCTAL,              //八进制数字0到7
    HEXADECIMAL,        //十六进制数字，包括数字0到9，字母a到f和A到F
};

//定义属性值
union attribute_value
{
    char char_value;
    string * p_string_value;
    int int_value;
    float float_value;
};

//定义词法分析器要输出的token单词
struct token
{
    ~token()
    {
        if(code_of_kind==code_of_kind::ID)
        {
            delete attribute_value.p_string_value;
        }
    };

    enum code_of_kind code_of_kind;         //种别码
    union attribute_value attribute_value;  //属性值
};

//括号匹配栈的结点
struct br_stack_node
{
    enum code_of_kind cok;          //该括号的种别码
    int line;                       //该括号出现的行数
};

//定义词法分析器
class Lexical_analyzer
{

private:
    //源程序文件结构体
    FILE * source_program_;

    //此时读取到的源程序文件的行数(用于错误分析的时候进行定位)
    int line_;

    //双缓冲区
    char double_buf_[BUF_SIZE<<1];

    //字符串缓冲区
    string * string_buf_;

    //两个缓冲区的有效位(两个缓冲区一定要一直保持两个同时有效)
    bool vaild_first_buf_;
    bool vaild_second_buf_;

    //缓冲区中扫描单词的起始指针和结束指针
    int lexeme_begining_;
    int lexeme_ending_;

    //括号的匹配栈
    stack<struct br_stack_node> br_stack_;

    //引号出现的行
    int quotation_line_;

    //常数的进制(目前只支持8,10,16)
    int base_;

    //上一次出现的错误
    enum my_error_code last_error_;

    //将源程序文件内容读入下一个缓冲区中
    bool write_buf();

    //将已读入的字符回退
    int retract(int num);

    //获取lexeme_ending_和lexeme_ending_之间的字符串
    string get_token_string();

    //跳过从lexmem_ending_开始的所有空白字符
    bool skip_all_blanks();

    //将此时识别出来的符号加入string缓冲区的末尾
    void append_to_string_buf();

    //将此时识别出来的字符串作为单词token输出
    struct token * get_token(enum code_of_kind cok);

    //把左括号压入栈中
    void left_br_push(enum code_of_kind l_br_cok);

    //根据右括号寻找栈中的匹配左括号然后出栈
    bool right_br_pop(enum code_of_kind r_br_cok);

    //错误处理
    void error_handle(enum my_error_code ec);

public:
    //构造函数
    Lexical_analyzer();

    //将词法分析器进行初始化
    bool init(const char * filename);

    //析构函数
    ~Lexical_analyzer();
    
    //判断词法分析器是否建立成功
    bool is_init_success();

    //词法分析一个单词
    struct token * token_scan();

    //词法分析源程序中的所有单词
    list<struct token * > * tokens_scan();

    //词法分析源程序中的所有单词并将词法分析得到的所有token输出到文件中
    list<struct token * > * tokens_scan_then_output(const char * filename);
    
};

#endif //__LEXICAL_ANALYZER_H