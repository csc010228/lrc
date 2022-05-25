/*
 *
 *  lexical_analyzer.cpp
 *  词法分析器
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/

#include "lexical_analyzer.h"
#include "symbol_table.h"

//判断缓冲区指针是不是位于第一个缓冲区
#define in_first_buf(index) ((index>=0) && (index<BUF_SIZE))                    
//判断缓冲区指针是不是位于第二个缓冲区
#define in_second_buf(index) ((index>=BUF_SIZE) && (index<(BUF_SIZE<<1)))       
//判断缓冲区指针是不是位于第一个缓冲区末尾
#define in_first_buf_end(index) (index==(BUF_SIZE-1))                           
//判断缓冲区指针是不是位于第二个缓冲区末尾
#define in_second_buf_end(index) (index==((BUF_SIZE<<1)-1))                     
//判断缓冲区指针是否在某一个缓冲区末尾
#define in_buf_end(index) ((index%BUF_SIZE)==(BUF_SIZE-1))                      
//判断缓冲区指针是否在某一个缓冲区开头
#define in_buf_begin(index) ((index%BUF_SIZE)==0)                               
//将缓冲区指针移到双缓冲区中的下一个位置
#define next_pos(index) (index=((index+1)%(BUF_SIZE<<1)))                       
//将缓冲区指针移到双缓冲区中的上一个位置
#define pre_pos(index) (index=((index-1)%(BUF_SIZE<<1)))                        
//判断字符是不是空格字符
#define is_blank(ch) ((ch>=0 && ch<=32) || ch==127)                             
//判断字符是不是字母
#define is_alpha(ch) ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
//判断某一个字符串能不能表示转义字符
#define is_escape_char(string) (escape_char.find(string)==escape_char.end())
//判断某一个字符是不是转义字符中的斜杠后面的字符
#define is_last_char_in_escape(ch) (ch=='a' || ch=='b' || ch=='f' || ch=='n' || ch=='r' || ch=='t' || ch=='v' || ch=='\\' || ch=='\'' || ch=='\"' || ch=='0')
//判断字符是不是数字
#define is_digit(ch,type) ((type==digit_type::DIGIT)?\
(ch>='0' && ch<='9'):\
(((type==digit_type::NOZERO)?\
(ch>='1' && ch<='9'):\
(((type==digit_type::OCTAL)?\
(ch>='0' && ch<='7'):\
(((type==digit_type::HEXADECIMAL)?\
((ch>='0' && ch<='9') || (ch>='a' && ch<='f') || (ch>='A' && ch<='F')):\
false)))))))

/*
获取缓冲区中的下一个字符
*/
#define next_char(cok,ch) ch=double_buf_[lexeme_ending_];\
next_pos(lexeme_ending_);\
if(ch==EOF)\
{\
    if(in_buf_begin(lexeme_ending_))\
    {\
        ch=double_buf_[lexeme_ending_];\
        next_pos(lexeme_ending_);\
    }\
    else\
    {\
        if(cok==code_of_kind::CONST_CHAR || cok==code_of_kind::CONST_STRING)\
        {\
            append_to_string_buf();\
            error_handle((cok==code_of_kind::CONST_CHAR)?my_error_code::UNCLOSED_CONST_CHAR:my_error_code::UNCLOSED_CONST_STRING);\
            if(!br_stack_.empty())\
            {\
                error_handle(my_error_code::UNCLOSED_LEFT_BRACKET);\
            }\
            return nullptr;\
        }\
        else\
        {\
            return get_token(cok);\
        }\
    }\
}

//定义关键字
extern map<string,enum code_of_kind> key_word;

//定义转义字符
extern map<string,char> escape_char;

/*
词法分析器的构造函数
*/
Lexical_analyzer::Lexical_analyzer()
{
    
}

/*
词法分析器的初始化

Paramters
---------
source_program_filename:预处理之后的源程序文件名

Return
------
如果初始化成功，就返回true，否则返回false
*/
bool Lexical_analyzer::init(const char * source_program_filename)
{
    //打开文件
    source_program_=fopen(source_program_filename,"r");
    if(source_program_==nullptr)
    {
        cout<<"File "<<source_program_filename<<" not exist!"<<endl;
        return false;
    }

    line_=1;
    string_buf_=nullptr;
    last_error_=my_error_code::NO_ERROR;

    //构建双缓冲区
    double_buf_[BUF_SIZE-1]=EOF;
    double_buf_[(BUF_SIZE<<1)-1]=EOF;
    vaild_first_buf_=false;
    vaild_second_buf_=false;

    //往第两个缓冲区中读入数据
    write_buf();
    write_buf();
    lexeme_begining_=0;
    lexeme_ending_=0;

    return true;
}

/*
词法分析器的析构函数
*/
Lexical_analyzer::~Lexical_analyzer()
{
    if(source_program_)
    {
        fclose(source_program_);
    }
}

/*
判断词法分析器是否建立成功
*/
bool Lexical_analyzer::is_init_success()
{
    return (source_program_!=nullptr);
}

/*
将源程序文件内容读入下一个缓冲区中

Return
------
写入成功就返回true，否则返回false
*/
bool Lexical_analyzer::write_buf()
{
    bool result=true;
    int read_length;
    if(!vaild_first_buf_)
    {
        read_length=fread(double_buf_,1,(BUF_SIZE-1),source_program_);
        if(read_length!=(BUF_SIZE-1))
        {
            double_buf_[read_length]=EOF;
        }
        vaild_first_buf_=true;
    }
    else if(!vaild_second_buf_)
    {
        read_length=fread(double_buf_+BUF_SIZE,1,(BUF_SIZE-1),source_program_);
        if(read_length!=(BUF_SIZE-1))
        {
            double_buf_[BUF_SIZE+read_length]=EOF;
        }
        vaild_second_buf_=true;
    }
    else
    {
        result=false;
    }
    return result;
}

/*
将已读入的字符回退

Parameters
----------
num:要回退的字符个数

Return
------
实际回退的字符个数
*/
int Lexical_analyzer::retract(int num)
{
    int i;
    for(i=0;i<num && i<MAX_RETRACT_NUM;i++)
    {
        pre_pos(lexeme_ending_);
        if(in_buf_end(lexeme_ending_))
        {
            lexeme_ending_--;
        }
    }
    return i;
}

/*
获取lexeme_begining_和lexeme_ending_之间的字符串
*/
string Lexical_analyzer::get_token_string()
{
    string result,tmp;

    if(in_first_buf(lexeme_begining_) && in_second_buf(lexeme_ending_))                 //开始指针在第一个缓冲区而结束指针在第二个缓冲区
    {
        result=string(double_buf_+lexeme_begining_,(BUF_SIZE-1-lexeme_begining_));
        tmp=string(double_buf_+BUF_SIZE,(lexeme_ending_-BUF_SIZE));
        result.append(tmp);
        vaild_first_buf_=false;
        write_buf();
    }
    else if(in_second_buf(lexeme_begining_) && in_first_buf(lexeme_ending_))            //开始指针在第二个缓冲区而结束指针在第一个缓冲区
    {
        result=string(double_buf_+lexeme_begining_,((BUF_SIZE<<1)-1-lexeme_begining_));
        tmp=string(double_buf_,lexeme_ending_);
        result.append(tmp);
        vaild_second_buf_=false;
        write_buf();
    }
    else                //开始指针和结束指针在同一个缓冲区
    {
        result=string(double_buf_+lexeme_begining_,(lexeme_ending_-lexeme_begining_));

        if(in_first_buf_end(lexeme_ending_))                    //有可能此时结束指针指向的是缓冲区结尾
        {
            vaild_first_buf_=false;
            write_buf();
        }
        else if(in_second_buf_end(lexeme_ending_))
        {
            vaild_second_buf_=false;
            write_buf();
        }
        else
        {

        }
    }
    if(in_buf_end(lexeme_ending_))                  //开始指针不能指向缓冲区末尾
    {
        next_pos(lexeme_ending_);
    }
    lexeme_begining_=lexeme_ending_;

    return result;
}

/*
跳过从lexmem_ending_开始的所有空白字符
要求进入函数的时候和退出函数的时候lexmem_begining_和lexmem_ending_都要相同

Return
------
如果此时已经到了文件末尾,那么就返回false,否则返回true
*/
bool Lexical_analyzer::skip_all_blanks()
{
    char ch;
    if(double_buf_[lexeme_begining_]==EOF)
    {
        return false;
    }
    while(true)
    {
        ch=double_buf_[lexeme_ending_];
        if(is_blank(ch))
        {
            if(ch=='\n')
            {
                line_++;
            }
        }
        else if(ch==EOF)
        {
            if(!in_buf_end(lexeme_ending_))
            {
                lexeme_begining_=lexeme_ending_;
                return false;
            }
            if(in_first_buf(lexeme_ending_))
            {
                vaild_first_buf_=false;
            }
            else if(in_second_buf(lexeme_ending_))
            {
                vaild_second_buf_=false;
            }
            else
            {

            }
            write_buf();
        }
        else
        {
            break;
        }
        next_pos(lexeme_ending_);
    }
    lexeme_begining_=lexeme_ending_;
    return true;
}

/*
将此时识别出来的串加入string缓冲区的末尾
（此时需要保证string缓冲区string_buf_有效）
*/
void Lexical_analyzer::append_to_string_buf()
{
    string string_tmp=get_token_string();
    if(string_buf_==nullptr)
    {
        string_buf_=new string(string_tmp);
    }
    else 
    {
        string_buf_->append(string_tmp);
    }
}

/*
将此时识别出来的字符串作为单词token输出

Parameters
----------
cok:单词的种别码

Return
------
识别出来的token,出错返回nullptr
*/
struct token * Lexical_analyzer::get_token(enum code_of_kind cok)
{
    struct token * result=new struct token;
    string token_string,*token_string_value;
    char ch;
    int token_len,i;
    Symbol_table * symbol_table=Symbol_table::get_instance();
    result->code_of_kind=cok;

    if(cok==code_of_kind::CONST_CHAR || cok==code_of_kind::CONST_STRING || cok==code_of_kind::ID)
    {
        token_string=*string_buf_;
        delete string_buf_;
        string_buf_=nullptr;
    }
    else
    {
        token_string=get_token_string();
    }

    switch(cok)
    {
        case code_of_kind::ID:
            if(key_word.find(token_string)==key_word.end())
            {
                result->attribute_value.p_string_value=new string(token_string);
            }
            else
            {
                result->code_of_kind=key_word[token_string];
                result->attribute_value.int_value=0;
            }
            break;
        case code_of_kind::CONST_INT:
            result->attribute_value.int_value=strtol(token_string.c_str(),nullptr,base_);
            break;
        case code_of_kind::CONST_FLOAT:
            result->attribute_value.float_value=strtof(token_string.c_str(),nullptr);
            break;
        case code_of_kind::CONST_CHAR:
            ch=token_string.at(1);
            result->attribute_value.char_value=(ch!='\\')?ch:escape_char[token_string.substr(1,2)];
            break;
        case code_of_kind::CONST_STRING:
            token_string_value=new string();
            token_len=token_string.length();
            for(i=1;i<token_len-1;i++)
            {
                ch=token_string.at(i);
                if(ch!='\\')
                {
                    (*token_string_value)+=ch;
                }
                else
                {
                    (*token_string_value)+=escape_char[token_string.substr(i++,2)];
                }
            }
            result->attribute_value.p_string_value=token_string_value;
            break;
        default:
            result->attribute_value.int_value=0;
            break;
    }

    return result;
}

/*
把左括号压入栈中

Parameters
----------
l_br_cok:要压入的左括号的种别码
*/
void Lexical_analyzer::left_br_push(enum code_of_kind l_br_cok)
{
    br_stack_.push({l_br_cok,line_});
}

/*
根据右括号寻找栈中的匹配左括号然后出栈
(需要保证此时的栈不能是空的)

Parameters
----------
r_br_cok:右括号的种别码

Return
------
如果输入的右括号能和栈顶的左括号匹配，那么就返回true并且把栈顶的左括号出栈，否则返回false
*/
bool Lexical_analyzer::right_br_pop(enum code_of_kind r_br_cok)
{
    enum code_of_kind l_br_cok;
    l_br_cok=br_stack_.top().cok;
    switch(r_br_cok)
    {
        case code_of_kind::R_PS:
            if(l_br_cok==code_of_kind::L_PS)
            {
                br_stack_.pop();
                return true;
            }
            else
            {
                return false;
            }
            break;
        case code_of_kind::R_BT:
            if(l_br_cok==code_of_kind::L_BT)
            {
                br_stack_.pop();
                return true;
            }
            else
            {
                return false;
            }
            break;
        case code_of_kind::R_BS:
            if(l_br_cok==code_of_kind::L_BS)
            {
                br_stack_.pop();
                return true;
            }
            else
            {
                return false;
            }
            break;
        default:
            break;
    }
    return false;
}

/*
错误处理
*/
void Lexical_analyzer::error_handle(enum my_error_code ec)
{
    string token_string;
    struct br_stack_node bsn;
    switch(ec)
    {
        case my_error_code::ILLEGAL_CHAR:
            token_string=get_token_string();
            cout<<endl<<"Line "<<line_<<" Error: Illegal char "<<token_string<<endl;
            break;
        // case my_error_code::ID_TOO_LONG:
            // cout<<endl<<"Line "<<line_<<" Error: Identifier length is too long (Identifer max length is "<<MAX_LEN_OF_ID<<")"<<endl;
            // break;
        case my_error_code::CONST_CHAR_TOO_LONG:
            cout<<endl<<"Line "<<line_<<" Error: Const char length is too long (Char length must be 1 )"<<endl;
            break;
        case my_error_code::UNMATCHED_BRACKET:
            bsn=br_stack_.top();
            br_stack_.pop();
            cout<<endl<<"Line "<<bsn.line<<" Error: ";
            switch(bsn.cok)
            {
                case code_of_kind::R_PS:
                    cout<<")";
                    break;
                case code_of_kind::R_BT:
                    cout<<"]";
                    break;
                case code_of_kind::R_BS:
                    cout<<"}";
                    break;
                default:
                    cout<<"right bracket";
                    break;
            }
            cout<<" cannot match ";
            bsn=br_stack_.top();
            switch(bsn.cok)
            {
                case code_of_kind::L_PS:
                    cout<<"(";
                    break;
                case code_of_kind::L_BT:
                    cout<<"[";
                    break;
                case code_of_kind::L_BS:
                    cout<<"{";
                    break;
                default:
                    cout<<"left bracket";
                    break;
            }
            cout<<" in line "<<bsn.line<<endl;
            break;
        case my_error_code::UNCLOSED_LEFT_BRACKET:
            while(!br_stack_.empty())
            {
                bsn=br_stack_.top();
                br_stack_.pop();
                cout<<endl<<"Line "<<bsn.line<<" Error: Unclosed ";
                switch(bsn.cok)
                {
                    case code_of_kind::L_PS:
                        cout<<"("<<endl;
                        break;
                    case code_of_kind::L_BT:
                        cout<<"["<<endl;
                        break;
                    case code_of_kind::L_BS:
                        cout<<"{"<<endl;
                        break;
                    default:
                        cout<<"bracket"<<endl;
                        break;
                }
            }
            break;
        case my_error_code::UNCLOSED_RIGHT_BRACKET:
            bsn=br_stack_.top();
            br_stack_.pop();
            cout<<endl<<"Line "<<bsn.line<<" Error: Unclosed ";
            switch(bsn.cok)
            {
                case code_of_kind::R_PS:
                    cout<<")"<<endl;
                    break;
                case code_of_kind::R_BT:
                    cout<<"]"<<endl;
                    break;
                case code_of_kind::R_BS:
                    cout<<"}"<<endl;
                    break;
                default:
                    cout<<"bracket"<<endl;
                    break;
            }
            break;
        case my_error_code::UNCLOSED_CONST_CHAR:
            cout<<endl<<"Line "<<quotation_line_<<" Error: Missing another ' \' '"<<endl;
            delete string_buf_;
            string_buf_=nullptr;
            break;
        case my_error_code::UNCLOSED_CONST_STRING:
            cout<<endl<<"Line "<<quotation_line_<<" Error: Missing another ' \" '"<<endl;
            delete string_buf_;
            string_buf_=nullptr;
            break;
        case my_error_code::NONEXISTENT_ESCAPE_CHAR:
            cout<<endl<<"Line "<<line_<<" Error: Nonexistent escape char \'\\"<<double_buf_[lexeme_ending_]<<"\'"<<endl;
            break;
        case my_error_code::CONST_INT_TOO_LONG:
            cout<<endl<<"Line "<<line_<<" Error: Const int length is too long (Max const int length is "<<MAX_LEN_OF_ID<<")"<<endl;
            break;
        case my_error_code::CONST_DOUBLE_TOO_LONG:
            cout<<endl<<"Line "<<line_<<" Error: Const double length is too long (Max const int length is "<<MAX_LEN_OF_ID<<")"<<endl;
            break;
        case my_error_code::INVALID_CONST_FORMAT:
            cout<<endl<<"Line "<<line_<<" Error: Invalid const format"<<endl;
            break;
        default:
            cout<<"Line "<<line_<<" Error: Unknown error"<<endl;
            break;
    }
    last_error_=ec;
}

/*
词法分析出一个单词
*/
struct token * Lexical_analyzer::token_scan()
{
    char ch;
    int length;
    int tag;
    enum digit_type dig_type;

    //跳过此前的所有空白字符
    if(!skip_all_blanks())
    {
        if(!br_stack_.empty())
        {
            error_handle(my_error_code::UNCLOSED_LEFT_BRACKET);
        }
        return nullptr;
    }

repeat:

    //获取本单词的第一个字符,如果此时这个字符是文件末尾的EOF的话就直接返回NULL
    ch=double_buf_[lexeme_ending_];
    next_pos(lexeme_ending_);
    if(ch==EOF)
    {
        if(in_buf_begin(lexeme_ending_))
        {
            ch=double_buf_[lexeme_ending_];
            next_pos(lexeme_ending_);
        }
        else
        {
            lexeme_ending_=lexeme_begining_;
            if(!br_stack_.empty())
            {
                error_handle(my_error_code::UNCLOSED_LEFT_BRACKET);
            }
            return nullptr;
        }
    }

    //识别标识符和保留字
    if(is_alpha(ch) || ch=='_')             //以字母和_开头的单词是合法的标识符
    {
        length=1;
        do
        {
            if(length>=MAX_LEN_OF_ID)
            {
                //error_handle(my_error_code::ID_TOO_LONG);
                //break;
                append_to_string_buf();
                length=0;
            }
            next_char(code_of_kind::ID,ch);
            length++;
        }while(is_alpha(ch) || is_digit(ch,digit_type::DIGIT) || ch=='_');        //合法的标识符只能包含字母,数字和_
        retract(1);
        append_to_string_buf();
        return get_token(code_of_kind::ID);
    }

    //识别整常数和实常数
    if(is_digit(ch,digit_type::DIGIT) || ch=='.')
    {
        if(ch=='0')
        {   
            //如果常数的开头是0，那么就说明这个可能是8进制数（也有可能是16进制）
            base_=8;
            dig_type=digit_type::OCTAL;
        }
        else
        {
            //如果常数的开头不是0，那么就这个是10进制数
            base_=10;
            dig_type=digit_type::DIGIT;
        }
        length=1;
again:
        if(ch!='.')
        {
            do
            {
                if(length>MAX_LEN_OF_ID)
                {
                    error_handle(my_error_code::CONST_INT_TOO_LONG);
                    retract(1);
                    return get_token(code_of_kind::CONST_INT);
                }
                next_char(code_of_kind::CONST_INT,ch);
                length++;
            }while(is_digit(ch,dig_type));
        }
        if(ch=='.')
        {
            do
            {
                if(length>MAX_LEN_OF_ID)
                {
                    error_handle(my_error_code::CONST_DOUBLE_TOO_LONG);
                    break;
                }
                next_char(code_of_kind::CONST_FLOAT,ch);
                length++;
            }while(is_digit(ch,(dig_type==digit_type::HEXADECIMAL?digit_type::HEXADECIMAL:digit_type::DIGIT)));
            if((ch=='e' || ch=='E' && dig_type!=digit_type::HEXADECIMAL) || (ch=='p' || ch=='P' && dig_type==digit_type::HEXADECIMAL))
            {
                next_char(code_of_kind::CONST_FLOAT,ch);
                length++;
                if(ch=='-' || ch=='+')
                {
                    tag=-1;
                    do
                    {
                        if(length>MAX_LEN_OF_ID)
                        {
                            error_handle(my_error_code::CONST_DOUBLE_TOO_LONG);
                            break;
                        }
                        next_char(code_of_kind::CONST_FLOAT,ch);
                        length++;
                        tag++;
                    }while(is_digit(ch,digit_type::DIGIT));
                    if(tag==0)
                    {
                        error_handle(my_error_code::INVALID_CONST_FORMAT);
                        retract(1);
                        return get_token(code_of_kind::CONST_FLOAT);
                    }
                    retract(1);
                    return get_token(code_of_kind::CONST_FLOAT);
                }
                else if(is_digit(ch,digit_type::DIGIT))
                {
                    do
                    {
                        if(length>MAX_LEN_OF_ID)
                        {
                            error_handle(my_error_code::CONST_DOUBLE_TOO_LONG);
                            break;
                        }
                        next_char(code_of_kind::CONST_FLOAT,ch);
                        length++;
                    }while(is_digit(ch,digit_type::DIGIT));
                    retract(1);
                    return get_token(code_of_kind::CONST_FLOAT);
                }
                else
                {
                    error_handle(my_error_code::INVALID_CONST_FORMAT);
                    retract(1);
                    return get_token(code_of_kind::CONST_FLOAT);
                }
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::CONST_FLOAT);
            }
        }
        else if((ch=='e' || ch=='E' && dig_type!=digit_type::HEXADECIMAL) || (ch=='p' || ch=='P' && dig_type==digit_type::HEXADECIMAL))
        {
            next_char(code_of_kind::CONST_FLOAT,ch);
            length++;
            if(ch=='-' || ch=='+')
            {
                tag=-1;
                do
                {
                    if(length>MAX_LEN_OF_ID)
                    {
                        error_handle(my_error_code::CONST_DOUBLE_TOO_LONG);
                        break;
                    }
                    next_char(code_of_kind::CONST_FLOAT,ch);
                    length++;
                    tag++;
                }while(is_digit(ch,digit_type::DIGIT));
                if(tag==0)
                {
                    error_handle(my_error_code::INVALID_CONST_FORMAT);
                    retract(1);
                    return get_token(code_of_kind::CONST_FLOAT);
                }
                retract(1);
                return get_token(code_of_kind::CONST_FLOAT);
            }
            else if(is_digit(ch,digit_type::DIGIT))
            {
                do
                {
                    if(length>MAX_LEN_OF_ID)
                    {
                        error_handle(my_error_code::CONST_DOUBLE_TOO_LONG);
                        break;
                    }
                    next_char(code_of_kind::CONST_FLOAT,ch);
                    length++;
                }while(is_digit(ch,digit_type::DIGIT));
                retract(1);
                return get_token(code_of_kind::CONST_FLOAT);
            }
            else
            {
                error_handle(my_error_code::INVALID_CONST_FORMAT);
                retract(1);
                return get_token(code_of_kind::CONST_FLOAT);
            }
        }
        else if(length==2 && (ch=='x' || ch=='X'))
        {
            //如果常数以0x或者0X开头，那么说明是16进制数
            base_=16;
            dig_type=digit_type::HEXADECIMAL;
            goto again;
        }
        else
        {
            retract(1);
            return get_token(code_of_kind::CONST_INT);
        }
    }

    //识别字符常量
    if(ch=='\'')
    {
        length=1;
        quotation_line_=line_;
        tag=-2;
        do
        {
            tag++;
            if(length>=BUF_SIZE-1)
            {
                append_to_string_buf();
                length=0;
            }
            next_char(code_of_kind::CONST_CHAR,ch);
            length++;
            if(ch=='\\')
            {
                if(length>=BUF_SIZE-1)
                {
                    append_to_string_buf();
                    length=0;
                }
                next_char(code_of_kind::CONST_CHAR,ch);
                length++;
                if(is_last_char_in_escape(ch))
                {
                    if(length>=BUF_SIZE-1)
                    {
                        append_to_string_buf();
                        length=0;
                    }
                    next_char(code_of_kind::CONST_CHAR,ch);
                    length++;
                }
                else
                {
                    error_handle(my_error_code::NONEXISTENT_ESCAPE_CHAR);
                }
            }
        }while(ch!='\'');
        append_to_string_buf();
        if(tag)
        {
            error_handle(my_error_code::CONST_CHAR_TOO_LONG);
        }
        return get_token(code_of_kind::CONST_CHAR);
    }

    //识别字符串常量（
    if(ch=='\"')
    {
        length=1;
        quotation_line_=line_;
        do
        {
            if(length>=BUF_SIZE-1)
            {
                append_to_string_buf();
                length=0;
            }
            next_char(code_of_kind::CONST_STRING,ch);
            length++;
            if(ch=='\\')
            {
                if(length>=BUF_SIZE-1)
                {
                    append_to_string_buf();
                    length=0;
                }
                next_char(code_of_kind::CONST_STRING,ch);
                length++;
                if(is_last_char_in_escape(ch))
                {
                    if(length>=BUF_SIZE-1)
                    {
                        append_to_string_buf();
                        length=0;
                    }
                    next_char(code_of_kind::CONST_STRING,ch);
                    length++;
                }
                else
                {
                    error_handle(my_error_code::NONEXISTENT_ESCAPE_CHAR);
                }
            }
        }while(ch!='\"');
        append_to_string_buf();
        return get_token(code_of_kind::CONST_STRING);
    }

    //识别其他的运算符
    switch(ch)
    {
        case '(':
            left_br_push(code_of_kind::L_PS);
            return get_token(code_of_kind::L_PS);
            break;
        case ')':
            if(br_stack_.empty())
            {
                br_stack_.push({code_of_kind::R_PS,line_});
                error_handle(my_error_code::UNCLOSED_RIGHT_BRACKET);
            }
            else if(!right_br_pop(code_of_kind::R_PS))
            {
                br_stack_.push({code_of_kind::R_PS,line_});
                error_handle(my_error_code::UNMATCHED_BRACKET);
            }
            return get_token(code_of_kind::R_PS);
            break;
        case ';':
            return get_token(code_of_kind::SEM);
            break;
        case ',':
            return get_token(code_of_kind::COM);
            break;
        case '=':
            next_char(code_of_kind::EQ,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::EQ);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::ASSIGN);
            }
            break;
        case '+':
            next_char(code_of_kind::ADD,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::ADD_ASSIGN);
            }
            else if(ch=='+')
            {
                return get_token(code_of_kind::SELF_ADD);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::ADD);
            }
            break;
        case '-':
            next_char(code_of_kind::SUB,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::SUB_ASSIGN);
            }
            else if(ch=='-')
            {
                return get_token(code_of_kind::SELF_SUB);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::SUB);
            }
            break;
        case '*':
            next_char(code_of_kind::MUL,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::MUL_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::MUL);
            }
            break;
        case '/':
            next_char(code_of_kind::DIV,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::DIV_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::DIV);
            }
            break;
        case '%':
            next_char(code_of_kind::MOD,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::MOD_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::MOD);
            }
            break;
        case '.':
            return get_token(code_of_kind::SPOT);
            break;
        case '&':
            next_char(code_of_kind::AND,ch);
            if(ch=='&')
            {
                return get_token(code_of_kind::LOGIC_AND);
            }
            else if(ch=='=')
            {
                return get_token(code_of_kind::AND_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::AND);
            }
            break;
        case '|':
            next_char(code_of_kind::OR,ch);
            if(ch=='|')
            {
                return get_token(code_of_kind::LOGIC_OR);
            }
            else if(ch=='=')
            {
                return get_token(code_of_kind::OR_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::OR);
            }
            break;
        case '^':
            next_char(code_of_kind::XOR,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::XOR_ASSIGN);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::XOR);
            }
            break;
        case '!':
            next_char(code_of_kind::NOT,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::UEQ);
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::NOT);
            }
            break;
        case '>':
            next_char(code_of_kind::GT,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::GE);
            }
            else if(ch=='>')
            {
                next_char(code_of_kind::R_BITMOV,ch);
                if(ch=='=')
                {
                    return get_token(code_of_kind::R_BITMOV_ASSIGN);
                }
                else
                {
                    retract(1);
                    return get_token(code_of_kind::R_BITMOV);
                }
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::GT);
            }
            break;
        case '<':
            next_char(code_of_kind::LT,ch);
            if(ch=='=')
            {
                return get_token(code_of_kind::LE);
            }
            else if(ch=='<')
            {
                next_char(code_of_kind::L_BITMOV,ch);
                if(ch=='=')
                {
                    return get_token(code_of_kind::L_BITMOV_ASSIGN);
                }
                else
                {
                    retract(1);
                    return get_token(code_of_kind::L_BITMOV);
                }
            }
            else
            {
                retract(1);
                return get_token(code_of_kind::LT);
            }
            break;
        case '[':
            left_br_push(code_of_kind::L_BT);
            return get_token(code_of_kind::L_BT);
            break;
        case ']':
            if(br_stack_.empty())
            {
                br_stack_.push({code_of_kind::R_BT,line_});
                error_handle(my_error_code::UNCLOSED_RIGHT_BRACKET);
            }
            else if(!right_br_pop(code_of_kind::R_BT))
            {
                br_stack_.push({code_of_kind::R_BT,line_});
                error_handle(my_error_code::UNMATCHED_BRACKET);
            }
            return get_token(code_of_kind::R_BT);
            break;
        case '{':
            left_br_push(code_of_kind::L_BS);
            return get_token(code_of_kind::L_BS);
            break;
        case '}':
            if(br_stack_.empty())
            {
                br_stack_.push({code_of_kind::R_BS,line_});
                error_handle(my_error_code::UNCLOSED_RIGHT_BRACKET);
            }
            else if(!right_br_pop(code_of_kind::R_BS))
            {
                br_stack_.push({code_of_kind::R_BS,line_});
                error_handle(my_error_code::UNMATCHED_BRACKET);
            }
            return get_token(code_of_kind::R_BS);
            break;
        case ':':
            return get_token(code_of_kind::COL);
            break;
        default:
            error_handle(my_error_code::ILLEGAL_CHAR);
            break;
    }
    goto repeat;            //只有出现了非法字符了才会运行到这个goto
}

/*
词法分析出所有单词
*/
list<struct token * > * Lexical_analyzer::tokens_scan()
{
    list<struct token * > * result=new list<struct token * >;
    struct token * t=token_scan();

    while(t)
    {
        result->push_back(t);
        t=token_scan();
    }

    return result;
}

/*
词法分析出所有单词，并且将它们输出到文件中

Parameters
----------
filename:要输出的文件名
*/
list<struct token * > * Lexical_analyzer::tokens_scan_then_output(const char * filename)
{
    FILE * fp=fopen(filename,"w");
    list<struct token * > * result=nullptr;
    struct token * t;
    string tmp;

    if(fp!=nullptr)
    {
        result=tokens_scan();

        for(auto i : (*result))
        {
            tmp="("+to_string((int)(i->code_of_kind))+",";
            switch(i->code_of_kind)
            {
                case code_of_kind::ID:
                    tmp+=((*(i->attribute_value.p_string_value))+")"+"\n");
                    break;
                case code_of_kind::CONST_INT:
                    tmp+=(to_string(i->attribute_value.int_value)+")"+"\n");
                    break;
                case code_of_kind::CONST_FLOAT:
                    tmp+=(to_string(i->attribute_value.float_value)+")"+"\n");
                    break;
                case code_of_kind::CONST_CHAR:
                    tmp+=(string("\'")+(i->attribute_value.char_value)+"\')"+"\n");
                    break;
                case code_of_kind::CONST_STRING:
                    tmp+=("\""+(*(i->attribute_value.p_string_value))+"\")"+"\n");
                    break;
                default:
                    tmp+=(to_string((int)(i->attribute_value.int_value))+")"+"\n");
                    break;
            }
            fwrite(tmp.c_str(),tmp.length(),1,fp);
        }

        fclose(fp);
    }
    else
    {
        cout<<"Create file "<<filename<<" fail!"<<endl;
    }

    return result;
}