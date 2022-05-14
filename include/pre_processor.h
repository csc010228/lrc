/*
 *
 *  pre_processor.h
 *  预处理器
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/
#ifndef __PRE_PROCESSOR_H
#define __PRE_PROCESSOR_H

#include<iostream>
#include<fstream>
#include<map>
#include<string>
#include<list>
#include<utility>
#include<stack>

using namespace std;

//缓冲区大小
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024

//宏函数
struct macro_func
{
    macro_func(string macro_fun_name,size_t f_param_num,list<size_t> f_params_in_content,list<string> content):macro_fun_name(macro_fun_name),f_param_num(f_param_num),f_params_in_content(f_params_in_content),content(content)
    {

    };

    string macro_fun_name;                  //宏函数名
    size_t f_param_num;                     //形参个数
    list<size_t> f_params_in_content;       //形参在宏函数内容中的顺序
    list<string> content;                   //宏函数内容
};

//定义预处理器
class Pre_processor
{

private:
    //源程序文件
    ifstream source_program_;

    //预处理之后的源程序文件
    ofstream source_program_after_pre_proces_;

    //处理的程序文件目前处理到的行数
    size_t current_line_;

    //目前源程序是否在双引号中
    bool double_quotation_mark_;

    //目前源程序是否在单引号中
    bool single_quotation_mark_;

    //读缓冲区
    string read_buf_;

    //宏替换缓冲区
    stack<pair<size_t,string> > macro_bufs_;

    //写缓冲区
    string write_buf_;

    //内置宏变量
    map<string,string> inner_macro_vars_;

    //宏变量
    map<string,string> macro_vars_;

    //宏函数
    map<string,struct macro_func> macro_funcs_;

    //获取源程序的下一个字符
    char read_char();

    //尝试进行#define语句的识别
    bool macro_define_distinguish_automata(char ch);

    //尝试进行宏替换
    size_t macro_replace_automata(char ch,bool do_macro_replace);

    //向写缓冲区写入一个字符，如果写缓冲区满就将其写入预处理之后的文件
    void write_char(char ch);

    //将写缓冲区的内容写入文件
    void write_buf_to_file();

    //增加一个宏变量
    bool add_macro_var(string macro_source,string macro_destination);

    //增加一个宏函数
    bool add_macro_func(string macro_source,list<string> macro_func_f_params,string macro_destination);

    //进行没有参数的宏函数的宏替换
    void no_f_params_macro_func_replace(string macro_source);

    //进行宏变量的宏替换
    void macro_var_replace(string macro_source);

    //进行内置宏变量的宏替换
    void inner_macro_var_replace(string macro_source);

public:
    //Pre_processor的构造函数
    Pre_processor();

    //Pre_processor的析构函数
    ~Pre_processor();

    //添加内置的头文件
    bool add_inner_header_files(size_t file_num,...);

    //进行预处理
    bool pre_process(const char * source_program_filename,const char * source_program_after_pre_processor_filename);
};

#endif //__PRE_PROCESSOR_H