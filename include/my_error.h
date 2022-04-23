/*
 *
 *  my_error.h
 *  错误码头文件
 *  Creator:Sichao Chen
 *  Create time:2021/10/10
 *
*/
#ifndef __MY_ERROR_H
#define __MY_ERROR_H

//定义错误码
enum class my_error_code
{
    NO_ERROR,                               //没有错误
    /*下面这些错误出现在词法分析过程中*/
    ILLEGAL_CHAR,                           //非法字符
    ID_TOO_LONG,                            //标识符长度过长
    CONST_INT_TOO_LONG,                     //整常数长度过长
    CONST_DOUBLE_TOO_LONG,                  //实常数长度过长
    CONST_CHAR_TOO_LONG,                    //字符长度过长
    UNMATCHED_BRACKET,                      //不匹配的括号
    UNCLOSED_LEFT_BRACKET,                  //不闭合的左括号
    UNCLOSED_RIGHT_BRACKET,                 //不闭合的右括号
    NONEXISTENT_ESCAPE_CHAR,                //不存在的转义字符
    UNCLOSED_CONST_CHAR,                    //不闭合的常量字符
    UNCLOSED_CONST_STRING,                  //不闭合的常量字符串
};

#endif //__MY_ERROR_H