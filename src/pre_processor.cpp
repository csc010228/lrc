/*
 *
 *  pre_processor.cpp
 *  预处理器
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/
#include "pre_processor.h"

#include<algorithm>
#include<sstream>
#include<stdarg.h>

//字符是否是字母
#define is_letter(ch) ((ch>='A' && ch<='Z') || (ch>='a' && ch<='z'))

//字符是否是数字
#define is_digit(ch) (ch>='0' && ch<='9')

//字符是否是下划线
#define is_underline(ch) (ch=='_')

//字符是否是左边的小括号
#define is_left_parentheses(ch) (ch=='(')

//字符是否是右边的小括号
#define is_right_parentheses(ch) (ch==')')

//字符是否是换行符
#define is_line_feed(ch) (ch=='\n')

//字符是否是空白符
#define is_whitespace(ch) (ch==' ')

//字符是否是井号
#define is_well_number(ch) (ch=='#')

//字符是否是逗号
#define is_comma(ch) (ch==',')

//字符是否是双引号
#define is_double_quotation(ch) (ch=='\"')

//字符是否是单引号
#define is_single_quotation(ch) (ch=='\'')

//字符是否是反斜杠
#define is_backslash(ch) (ch=='\\')

/*
Pre_processor的构造函数
*/
Pre_processor::Pre_processor():double_quotation_mark_(false),single_quotation_mark_(false)
{
    inner_macro_vars_["__FILE__"]="";
    inner_macro_vars_["__LINE__"]="";
    inner_macro_vars_["__DATE__"]="";
    inner_macro_vars_["__TIME__"]="";
    inner_macro_vars_["__STDC__"]="";
}

/*
Pre_processor的析构函数
*/
Pre_processor::~Pre_processor()
{
    
}

/*
增加一个宏变量

Parameters
----------
macro_source:待替换的宏
macro_destination:被替换的宏

Return
------
如果增加成功，返回true，否则返回false
*/
bool Pre_processor::add_macro_var(string macro_source,string macro_destination)
{
    if(macro_vars_.find(macro_source)!=macro_vars_.end() || macro_funcs_.find(macro_source)!=macro_funcs_.end() || inner_macro_vars_.find(macro_source)!=inner_macro_vars_.end())
    {
        return false;
    }
    macro_vars_[macro_source]=macro_destination;
    return true;
}

/*
增加一个宏函数

Parameters
----------
macro_source:待替换的宏
macro_func_f_params:宏函数的形参
macro_destination:被替换的宏

Return
------
如果增加成功，返回true，否则返回false
*/
bool Pre_processor::add_macro_func(string macro_source,list<string> macro_func_f_params,string macro_destination)
{
    size_t automata_state=0;
    string identifier;
    list<size_t> f_params_in_content;
    list<string> content;
    if(macro_vars_.find(macro_source)!=macro_vars_.end() || macro_funcs_.find(macro_source)!=macro_funcs_.end() || inner_macro_vars_.find(macro_source)!=inner_macro_vars_.end())
    {
        return false;
    }
    if(macro_func_f_params.size()>0)
    {
        cout<<"Warning: Unable to process macro functions "<<macro_source<<" with "<<macro_func_f_params.size()<<" parameters at this time!"<<endl;
        return false;
    }
    content.push_back(macro_destination);
    macro_funcs_.insert(make_pair(macro_source,macro_func(macro_source,0,f_params_in_content,content)));
    return true;
}

/*
进行没有参数的宏函数的宏替换
*/
void Pre_processor::no_f_params_macro_func_replace(string macro_source)
{
    string macro_destination;
    
    if(macro_funcs_.find(macro_source)!=macro_funcs_.end())
    {
        macro_destination=macro_funcs_.at(macro_source).content.front();
        macro_bufs_.push(make_pair(0,macro_destination));
    }
}

/*
进行宏变量的宏替换
*/
void Pre_processor::macro_var_replace(string macro_source)
{
    string macro_destination;

    if(macro_vars_.find(macro_source)!=macro_vars_.end())
    {
        macro_destination=macro_vars_.at(macro_source);
        macro_bufs_.push(make_pair(0,macro_destination));
    }
}

/*
进行内置宏变量的宏替换
*/
void Pre_processor::inner_macro_var_replace(string macro_source)
{
    //string macro_destination;
    //stringstream ss;

    if(macro_source=="__LINE__")
    {
        //ss << current_line_;
        //ss >> macro_destination;
        macro_bufs_.push(make_pair(0,to_string(current_line_)));
    }
}

/*
添加内置的头文件

Parameters
----------
file_num:要添加的头文件的数量
filename:要添加的内置头文件的文件名

Return
------
如果添加成功返回true，否则返回false
*/
bool Pre_processor::add_inner_header_files(size_t file_num,...)
{
    //创建一个文件输入流对象
	ifstream inFile;
    string line;
    const char * filename;
    bool res=true;
    va_list argptr;
	va_start(argptr,file_num);
	for(size_t i=0;i<file_num;i++)
	{
		filename=va_arg(argptr,const char *);
        //打开文件
        inFile.open(filename);
        if(!inFile)
        {
            cout<<"Open file "<<filename<<" error!"<<endl;
            return false;
        }

        //预处理，这里主要是为了处理头文件中的#define语句
        if(!pre_process(filename,nullptr))
        {
            cout<<"Inner header file "<<filename<<" format error!"<<endl;
            res=false;
        }

        //关闭文件
        inFile.close();
	}
    va_end(argptr);
    return res;
}

/*
获取源程序的下一个字符
*/
char Pre_processor::read_char()
{
    static size_t read_buf_pos=0;

    while(!macro_bufs_.empty())
    {
        if(macro_bufs_.top().first>=macro_bufs_.top().second.size())
        {
            macro_bufs_.pop();
        }
        else
        {
            return macro_bufs_.top().second[macro_bufs_.top().first++];
        }
    }

    if(read_buf_pos>=read_buf_.size())
    {
        read_buf_pos=0;
        if(getline(source_program_,read_buf_))
        {
            read_buf_.push_back('\n');
        }
        else
        {
            read_buf_.clear();
            return EOF;
        }
    }
    return read_buf_[read_buf_pos++];
}

/*
尝试进行#define语句的识别的自动机
能识别正常的宏变量定义
只能识别没有参数的宏函数定义

Return
------
是否成功进行了#define的识别
*/
bool Pre_processor::macro_define_distinguish_automata(char ch)
{
    bool res=false;
    static size_t automata_state=1,macro_define_string_pointer=0;
    static string macro_define_string="define",macro_source,macro_destination,macro_func_f_param;
    static list<string> macro_func_f_params;
    switch(automata_state)
    {
        case 0:
            if(is_line_feed(ch))
            {
                automata_state=1;
            }
            break;
        case 1:
            if(is_whitespace(ch) || is_line_feed(ch))
            {
                ;
            }
            else if(ch=='#')
            {
                automata_state=2;
            }
            else
            {
                automata_state=0;
            }
            break;
        case 2:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(macro_define_string[macro_define_string_pointer++]==ch)
            {
                if(macro_define_string_pointer>=macro_define_string.size())
                {
                    macro_define_string_pointer=0;
                    automata_state=3;
                }
            }
            else
            {
                macro_define_string_pointer=0;
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 3:
            if(is_whitespace(ch))
            {
                automata_state=4;
            }
            else
            {
                automata_state=is_line_feed(ch)?1:0;
            }
        case 4:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_letter(ch) || is_underline(ch))
            {
                macro_source.push_back(ch);
                automata_state=5;
            }
            else
            {
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 5:
            if(is_letter(ch) || is_underline(ch) || is_digit(ch))
            {
                macro_source.push_back(ch);
            }
            else if(is_whitespace(ch))
            {
                automata_state=6;
            }
            else if(is_left_parentheses(ch))
            {
                automata_state=9;
            }
            else
            {
                if(is_line_feed(ch))
                {
                    add_macro_var(macro_source,macro_destination);
                    automata_state=1;
                }
                else
                {
                    automata_state=0;
                }
                macro_source.clear();
            }
            break;
        case 6:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_line_feed(ch))
            {
                add_macro_var(macro_source,macro_destination);
                automata_state=1;
                macro_source.clear();
                macro_destination.clear();
            }
            else
            {
                macro_destination.push_back(ch);
                automata_state=7;
                res=true;
            }
            break;
        case 7:
            if(is_line_feed(ch))
            {
                add_macro_var(macro_source,macro_destination);
                automata_state=1;
                macro_source.clear();
                macro_destination.clear();
            }
            else
            {
                macro_destination.push_back(ch);
                res=true;
            }
            break;
        case 9:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_right_parentheses(ch))
            {
                automata_state=13;
            }
            else if(is_letter(ch) || is_underline(ch))
            {
                macro_func_f_param.push_back(ch);
                automata_state=10;
            }
            else
            {
                macro_source.clear();
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 10:
            if(is_letter(ch) || is_underline(ch) || is_digit(ch))
            {
                macro_func_f_param.push_back(ch);
            }
            else if(is_whitespace(ch))
            {
                macro_func_f_params.push_back(macro_func_f_param);
                macro_func_f_param.clear();
                automata_state=11;
            }
            else if(is_comma(ch))
            {
                macro_func_f_params.push_back(macro_func_f_param);
                macro_func_f_param.clear();
                automata_state=12;
            }
            else if(is_right_parentheses(ch))
            {
                macro_func_f_params.push_back(macro_func_f_param);
                macro_func_f_param.clear();
                automata_state=13;
            }
            else
            {
                macro_source.clear();
                macro_func_f_params.clear();
                macro_func_f_param.clear();
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 11:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_comma(ch))
            {
                automata_state=12;
            }
            else if(is_right_parentheses(ch))
            {
                automata_state=13;
            }
            else
            {
                macro_source.clear();
                macro_func_f_params.clear();
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 12:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_letter(ch) || is_underline(ch))
            {
                macro_func_f_param.push_back(ch);
                automata_state=10;
            }
            else
            {
                macro_source.clear();
                macro_func_f_params.clear();
                automata_state=is_line_feed(ch)?1:0;
            }
            break;
        case 13:
            if(is_whitespace(ch))
            {
                automata_state=14;
            }
            else if(is_line_feed(ch))
            {
                add_macro_func(macro_source,macro_func_f_params,macro_destination);
                automata_state=1;
                macro_source.clear();
                macro_func_f_params.clear();
            }
            else
            {
                macro_source.clear();
                macro_func_f_params.clear();
                automata_state=0;
            }
            break;
        case 14:
            if(is_whitespace(ch))
            {
                ;
            }
            else if(is_line_feed(ch))
            {
                add_macro_func(macro_source,macro_func_f_params,macro_destination);
                automata_state=1;
                macro_source.clear();
                macro_func_f_params.clear();
            }
            else
            {
                macro_destination.push_back(ch);
                automata_state=15;
                res=true;
            }
            break;
        case 15:
            if(is_line_feed(ch))
            {
                add_macro_func(macro_source,macro_func_f_params,macro_destination);
                automata_state=1;
                macro_source.clear();
                macro_func_f_params.clear();
                macro_destination.clear();
            }
            else
            {
                macro_destination.push_back(ch);
                res=true;
            }
            break;
        default:
            macro_define_string_pointer=0;
            macro_source.clear();
            macro_func_f_params.clear();
            macro_func_f_param.clear();
            macro_destination.clear();
            automata_state=0;
            break;
    }

    return res;
}

/*
尝试进行宏替换的自动机
能进行正常的宏变量替换
只能进行没有参数的宏函数替换

Return
------
返回写缓冲区需要弹出的字符
*/
size_t Pre_processor::macro_replace_automata(char ch,bool do_macro_replace)
{
    size_t res=0;
    static size_t automata_state=1,macro_source_size;
    static string identifier;

    if(!do_macro_replace)
    {
        return res;
    }

    switch(automata_state)
    {
        case 1:
            if(is_double_quotation(ch))
            {
                automata_state=2;
            }
            else if(is_single_quotation(ch))
            {
                automata_state=4;
            }
            else if(is_letter(ch) || is_underline(ch))
            {
                identifier.push_back(ch);
                automata_state=6;
            }
            break;
        case 2:
            if(is_double_quotation(ch))
            {
                automata_state=1;
            }
            else if(is_backslash(ch))
            {
                automata_state=3;
            }
            break;
        case 3:
            automata_state=2;
            break;
        case 4:
            if(is_single_quotation(ch))
            {
                automata_state=1;
            }
            else if(is_backslash(ch))
            {
                automata_state=5;
            }
            break;
        case 5:
            automata_state=4;
            break;
        case 6:
            if(is_letter(ch) || is_underline(ch) || is_digit(ch))
            {
                identifier.push_back(ch);
            }
            else if(macro_funcs_.find(identifier)!=macro_funcs_.end() && (is_whitespace(ch) || is_line_feed(ch)))
            {
                macro_source_size=identifier.size()+1;
                automata_state=7;
            }
            else if(macro_funcs_.find(identifier)!=macro_funcs_.end() && is_left_parentheses(ch))
            {
                macro_source_size=identifier.size()+1;
                automata_state=8;
            }
            else
            {
                if(macro_vars_.find(identifier)!=macro_vars_.end())
                {
                    macro_var_replace(identifier);
                    res=identifier.size();
                }
                else if(inner_macro_vars_.find(identifier)!=inner_macro_vars_.end())
                {
                    inner_macro_var_replace(identifier);
                    res=identifier.size();
                }
                automata_state=1;
                identifier.clear();
            }
            break;
        case 7:
            if(is_whitespace(ch) || is_line_feed(ch))
            {
                macro_source_size++;
            }
            else if(is_left_parentheses(ch))
            {
                macro_source_size++;
                automata_state=8;
            }
            else
            {
                automata_state=1;
                identifier.clear();
            }
            break;
        case 8:
            if(is_whitespace(ch) || is_line_feed(ch))
            {
                macro_source_size++;
            }
            else if(is_right_parentheses(ch))
            {
                macro_source_size++;
                automata_state=9;
            }
            else
            {
                automata_state=1;
                identifier.clear();
            }
            break;
        case 9:
            res=macro_source_size;
            no_f_params_macro_func_replace(identifier);
            automata_state=1;
            identifier.clear();
            break;
        default:
            automata_state=1;
            identifier.clear();
            break;
    }

    return res;
}

/*
向写缓冲区写入一个字符
同时检查是否能进行宏替换
并在必要的时候将缓冲区的内容写入预处理之后的文件

Parameters
----------
ch:要向写缓冲区中写入的字符
*/
void Pre_processor::write_char(char ch)
{
    bool do_macro_replace;
    size_t write_buf_pop_times;
    static bool is_identifier=false;
    static size_t func_macro_step=0,i;
    static string macro_func_source;
    static list<string> macro_func_r_params;

    //尝试进行#define语句的识别
    do_macro_replace=!macro_define_distinguish_automata(ch);

    //尝试进行宏替换
    write_buf_pop_times=macro_replace_automata(ch,do_macro_replace);

    if(write_buf_pop_times)
    {
        while(write_buf_pop_times)
        {
            write_buf_.pop_back();
            write_buf_pop_times--;
        }
        macro_bufs_.top().second.push_back(ch);
    }
    else
    {
        write_buf_.push_back(ch);
    }

    if(is_line_feed(ch))
    {
        current_line_++;
    }
}

/*
将写缓冲区的内容写入文件
*/
void Pre_processor::write_buf_to_file()
{
    if(source_program_after_pre_proces_.is_open())
    {
        source_program_after_pre_proces_<<write_buf_;
    }
    write_buf_.clear();
}

/*
进行预处理
主要是用于去除注释,以及进行简单的宏替换

Parameters
----------
source_program_filename:源程序文件名
source_program_after_pre_processor_filename:预处理之后的源程序名

Return
------
如果预处理成功返回true，否则返回false
*/
bool Pre_processor::pre_process(const char * source_program_filename,const char * source_program_after_pre_processor_filename)
{
    char ch1,ch2;
    bool singleline_comment=false,multiline_comment=false;
    current_line_=1;

    //打开读文件
    source_program_.open(source_program_filename);
	if (!source_program_)
    {
        cout<<"File "<<source_program_filename<<" open error!"<<endl;
        return false;
	}

    //打开写文件
	if(source_program_after_pre_processor_filename)
    {
        source_program_after_pre_proces_.open(source_program_after_pre_processor_filename,ios::out);
        if(!source_program_after_pre_proces_)
        {
            cout<<"File "<<source_program_after_pre_processor_filename<<" open error!"<<endl;
            source_program_.close();
            return false;
        }
    }

    //获取下一个字符，进行注释的删除
    while((ch1=read_char())!=EOF)
    {
        if(singleline_comment)
        {
            //如果此时已经在单行注释里了
            switch(ch1)
            {
                case '\n':
                    write_char(ch1);
                    singleline_comment=false;
                    break;
                case '\\':
                    ch2=read_char();
                    if(ch2=='\n')
                    {
                        write_char(ch2);
                    }
                    else if(ch2==EOF)
                    {
                        goto out;
                    }
                    else
                    {
                        singleline_comment=false;
                    }
                    break;
                case EOF:
                    goto out;
                default:
                    break;
            }
        }
        else if(multiline_comment)
        {
            //如果此时已经在多行注释里了
            switch(ch1)
            {
                case '\n':
                    write_char(ch1);
                    break;
                case '*':
                    ch2=read_char();
                    if(ch2=='/')
                    {
                        multiline_comment=false;
                    }
                    else
                    {
                        if(ch2=='\n')
                        {
                            write_char(ch2);
                        }
                        else if(ch2==EOF)
                        {
                            goto out;
                        }
                    }
                    break;
                case EOF:
                    goto out;
                default:
                    break;
            }
        }
        else
        {
            if(double_quotation_mark_ || single_quotation_mark_)
            {
                //如果此时在单引号或者双引号中
                switch(ch1)
                {
                    case '\\':
                        ch2=read_char();
                        if(ch2==EOF)
                        {
                            write_char(ch1);
                            goto out;
                        }
                        else if(ch2!='\n')
                        {
                            write_char(ch1);
                            write_char(ch2);
                        }
                        break;
                    case '\"':
                        write_char(ch1);
                        if(double_quotation_mark_)
                        {
                            double_quotation_mark_=false;
                        }
                        break;
                    case '\'':
                        write_char(ch1);
                        if(single_quotation_mark_)
                        {
                            single_quotation_mark_=false;
                        }
                        break;
                    case EOF:
                        goto out;
                    default:
                        write_char(ch1);
                        break;
                }
            }
            else
            {
                //如果此时不在注释中
                switch(ch1)
                {
                    case '/':
                        ch2=read_char();
                        if(ch2=='/')
                        {
                            singleline_comment=true;
                        }
                        else if(ch2=='*')
                        {
                            multiline_comment=true;
                        }
                        else if(ch2==EOF)
                        {
                            write_char(ch1);
                            goto out;
                        }
                        else
                        {
                            write_char(ch1);
                            write_char(ch2);
                        }
                        break;
                    case '\"':
                        double_quotation_mark_=true;
                        write_char(ch1);
                        break;
                    case '\'':
                        single_quotation_mark_=true;
                        write_char(ch1);
                        break;
                    case '\\':
                        ch2=read_char();
                        if(ch2==EOF)
                        {
                            write_char(ch1);
                            goto out;
                        }
                        else if(ch2!='\n')
                        {
                            write_char(ch1);
                            write_char(ch2);
                        }
                        break;
                    case EOF:
                        goto out;
                        break;
                    default:
                        write_char(ch1);
                        break;
                }
            }
        }
    }

out:
    //检查写缓冲区，将里面的内容写入文件
    write_buf_to_file();

    //关闭文件
    source_program_.close();
    if(source_program_after_pre_proces_.is_open())
    {
        source_program_after_pre_proces_.close();
    }
    return true;
}