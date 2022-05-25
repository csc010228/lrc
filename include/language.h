/*
 *
 *  language.h
 *  编程语言
 *  Creator:Sichao Chen
 *  Create time:2021/10/14
 *
*/
#ifndef __LANGUAGE_H
#define __LANGUAGE_H

#include<string>
#include<map>

using namespace std;

//约定语言的入口函数名
#define MAIN_FUNC "main"

//约定空符号的编号是终结符个数减1
#define EMPTY_SYMBOL terminator_num-1
//约定栈底符号的编号是终结符个数减2
#define STACK_BOTTOM_SYMBOL terminator_num-2

//属于文法的终结符但是不属于关键字的符号
#define TERMINATOR_NOT_IN_KEY_WORD {\
    {"id",(syntax_symbol_index)code_of_kind::ID},\
    {"const_int",(syntax_symbol_index)code_of_kind::CONST_INT},\
    {"const_float",(syntax_symbol_index)code_of_kind::CONST_FLOAT},\
    {"const_char",(syntax_symbol_index)code_of_kind::CONST_CHAR},\
    {"const_string",(syntax_symbol_index)code_of_kind::CONST_STRING},\
    {"$",STACK_BOTTOM_SYMBOL},\
    {"@",EMPTY_SYMBOL},\
}


//定义种别码
enum class code_of_kind
{
    ID,                 // 标识符
    CONST_INT,          // 无符号整常数
    CONST_FLOAT,        // 单精度实常数
    CONST_CHAR,         // 字符常量
    CONST_STRING,       // 字符串常量
    SHORT,              // short
    INT,                // int
    LONG,               // long
    FLOAT,              // float
    DOUBLE,             // double
    CHAR,               // char
    VOID,               // void
    STRUCT,             // struct
    UNION,              // union
    ENUM,               // enum
    IF,                 // if
    ELSE,               // else
    DO,                 // do
    TYPEDEF,            // typedef
    CONST,              // const
    UNSIGNED,           // unsigned
    EXTERN,             // extern
    STATIC,             // static
    AUTO,               // auto
    REGISTER,           // register
    WHILE,              // while
    FOR,                // for
    SWITCH,             // switch
    CASE,               // case
    DEFAULT,            // default
    BREAK,              // break
    CONTINUE,           // continue
    RETURN,             // return
    GOTO,               // goto
    SIZEOF,             // sizeof
    L_PS,               // (
    R_PS,               // )
    SEM,                // ;
    COM,                // ,
    ASSIGN,             // =
    ADD,                // +
    SUB,                // -
    MUL,                // *
    DIV,                // /
    MOD,                // %
    SPOT,               // .
    L_ARROW,            // ->
    SELF_ADD,           // ++
    SELF_SUB,           // --
    LOGIC_AND,          // &&
    LOGIC_OR,           // ||
    EQ,                 // ==
    UEQ,                // !=
    GT,                 // >
    LT,                 // <
    GE,                 // >=
    LE,                 // <=
    L_BT,               // [
    R_BT,               // ]
    L_BS,               // {
    R_BS,               // }
    ADD_ASSIGN,         // +=
    SUB_ASSIGN,         // -=
    MUL_ASSIGN,         // *=
    DIV_ASSIGN,         // /=
    MOD_ASSIGN,         // %=
    AND_ASSIGN,         // &=
    OR_ASSIGN,          // |=
    XOR_ASSIGN,         // ^=
    L_BITMOV_ASSIGN,    // <<=
    R_BITMOV_ASSIGN,    // >>=
    NOT,                // !
    AND,                // &
    OR,                 // |
    XOR,                // ^
    L_BITMOV,           // <<
    R_BITMOV,           // >>
    COL,                // :
};

//语言支持的标识符类型
enum class language_data_type
{
    VOID,
    CHAR,
    INT,
    FLOAT,
    LONG,
    DOUBLE,
    POINTER,
};

//One Above All
typedef union OAA
{
    char char_data;
    bool bool_data;
    short short_data;
    int int_data;
    float float_data;
    double double_data;
    long long_data;
    void * pointer_data;

    OAA():int_data(0)
    {
        ;
    };

    OAA(char char_data):char_data(char_data)
    {
        ;
    };

    OAA(bool bool_data):bool_data(bool_data)
    {
        ;
    };

    OAA(short short_data):short_data(short_data)
    {
        ;
    };

    OAA(int int_data):int_data(int_data)
    {
        ;
    };

    OAA(float float_data):float_data(float_data)
    {
        ;
    };

    OAA(double double_data):double_data(double_data)
    {
        ;
    };

    OAA(long long_data):long_data(long_data)
    {
        ;
    };

    OAA(void * pointer_data):pointer_data(pointer_data)
    {
        ;
    };

    bool operator <(const union OAA & a) const
    {
        return (this->int_data<a.int_data);
    };

    bool operator >(const union OAA & a) const
    {
        return (this->int_data>a.int_data);
    };

    bool operator <=(const union OAA & a) const
    {
        return (this->int_data<=a.int_data);
    };

    bool operator >=(const union OAA & a) const
    {
        return (this->int_data>=a.int_data);
    };

    bool operator ==(const union OAA & a) const
    {
        return (this->int_data==a.int_data);
    };

    bool operator !=(const union OAA & a) const
    {
        return (this->int_data!=a.int_data);
    };

    void type_conversion(enum language_data_type from_data_type,enum language_data_type to_data_type)
    {
        if(from_data_type==to_data_type)
        {
            return;
        }
        if(from_data_type==language_data_type::INT && to_data_type==language_data_type::FLOAT)
        {
            float_data=(float)int_data;
        }
        else if(from_data_type==language_data_type::FLOAT && to_data_type==language_data_type::INT)
        {
            int_data=(int)float_data;
        }
    };

} OAA;

//OAA的赋值
#define OAA_assign(to,from,data_type) \
switch(data_type)\
{\
    case language_data_type::INT:\
        to.int_data=from;\
        break;\
    case language_data_type::FLOAT:\
        to.float_data=from;\
        break;\
    default:\
        break;\
}

#endif  //__LANGUAGE_H