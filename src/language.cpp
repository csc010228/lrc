/*
 *
 *  language.cpp
 *  编程语言
 *  Creator:Sichao Chen
 *  Create time:2021/11/04
 *
*/

#include "language.h"
#include<string>
#include<map>

using namespace std;

//文法的终结符的个数(包括空符号和栈底符号)
int terminator_num=80;

//定义关键字
map<string,enum code_of_kind> key_word={
    {"short",code_of_kind::SHORT},
    {"int",code_of_kind::INT},
    {"long",code_of_kind::LONG},
    {"float",code_of_kind::FLOAT},
    {"double",code_of_kind::DOUBLE},
    {"char",code_of_kind::CHAR},
    {"void",code_of_kind::VOID},
    {"struct",code_of_kind::STRUCT},
    {"union",code_of_kind::UNION},
    {"enum",code_of_kind::ENUM},
    {"if",code_of_kind::IF},
    {"else",code_of_kind::ELSE},
    {"do",code_of_kind::DO},
    {"typedef",code_of_kind::TYPEDEF},
    {"const",code_of_kind::CONST},
    {"unsigned",code_of_kind::UNSIGNED},
    {"extern",code_of_kind::EXTERN},
    {"static",code_of_kind::STATIC},
    {"auto",code_of_kind::AUTO},
    {"register",code_of_kind::REGISTER},
    {"while",code_of_kind::WHILE},
    {"for",code_of_kind::FOR},
    {"switch",code_of_kind::SWITCH},
    {"case",code_of_kind::CASE},
    {"default",code_of_kind::DEFAULT},
    {"break",code_of_kind::BREAK},
    {"continue",code_of_kind::CONTINUE},
    {"return",code_of_kind::RETURN},
    {"goto",code_of_kind::GOTO},
    {"sizeof",code_of_kind::SIZEOF},
    {"(",code_of_kind::L_PS},
    {")",code_of_kind::R_PS},
    {";",code_of_kind::SEM},
    {",",code_of_kind::COM},
    {"=",code_of_kind::ASSIGN},
    {"+",code_of_kind::ADD},
    {"-",code_of_kind::SUB},
    {"*",code_of_kind::MUL},
    {"/",code_of_kind::DIV},
    {"%",code_of_kind::MOD},
    {".",code_of_kind::SPOT},
    {"->",code_of_kind::L_ARROW},
    {"++",code_of_kind::SELF_ADD},
    {"--",code_of_kind::SELF_SUB},
    {"&&",code_of_kind::LOGIC_AND},
    {"||",code_of_kind::LOGIC_OR},
    {"==",code_of_kind::EQ},
    {"!=",code_of_kind::UEQ},
    {">",code_of_kind::GT},
    {"<",code_of_kind::LT},
    {">=",code_of_kind::GE},
    {"<=",code_of_kind::LE},
    {"[",code_of_kind::L_BT},
    {"]",code_of_kind::R_BT},
    {"{",code_of_kind::L_BS},
    {"}",code_of_kind::R_BS},
    {"+=",code_of_kind::ADD_ASSIGN},
    {"-=",code_of_kind::SUB_ASSIGN},
    {"*=",code_of_kind::MUL_ASSIGN},
    {"/=",code_of_kind::DIV_ASSIGN},
    {"%=",code_of_kind::MOD_ASSIGN},
    {"&=",code_of_kind::AND_ASSIGN},
    {"|=",code_of_kind::OR_ASSIGN},
    {"^=",code_of_kind::XOR_ASSIGN},
    {"<<=",code_of_kind::L_BITMOV_ASSIGN},
    {">>=",code_of_kind::R_BITMOV_ASSIGN},
    {"!",code_of_kind::NOT},
    {"&",code_of_kind::AND},
    {"|",code_of_kind::OR},
    {"^",code_of_kind::XOR},
    {"<<",code_of_kind::L_BITMOV},
    {">>",code_of_kind::R_BITMOV},
    {":",code_of_kind::COL}
};

//定义转义字符
map<string,char> escape_char={
    {"\\a",'\a'},
    {"\\b",'\b'},
    {"\\f",'\f'},
    {"\\n",'\n'},
    {"\\r",'\r'},
    {"\\t",'\t'},
    {"\\v",'\v'},
    {"\\\\",'\\'},
    {"\\\'",'\''},
    {"\\\"",'\"'},
    {"\\0",'\0'}
};

//各个数据类型所占bits
map<enum language_data_type,size_t> bits_of_data_types={
    {language_data_type::VOID,0},
    {language_data_type::CHAR,8},
    {language_data_type::INT,32},
    {language_data_type::FLOAT,32},
    {language_data_type::LONG,64},
    {language_data_type::DOUBLE,64},
    {language_data_type::POINTER,32}
};

//进行二元运算的时候的类型转换
enum language_data_type binary_operation_type_conversion(enum language_data_type arg1_type,enum language_data_type arg2_type)
{
    //语言只支持int类型
    return language_data_type::INT;
}

//进行一元运算的时候的类型转换
enum language_data_type unary_operation_type_conversion(enum language_data_type arg_type)
{
    //语言只支持int类型
    return language_data_type::INT;
}