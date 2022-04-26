# 词法分析器

## 说明
* lrc根据自动机原理，实现了一个词法分析器，目前能够识别大部分C语言关键字,具体能识别的token见language.h和language.cpp

## 扩展
* 如果想要对lrc的词法分析器进行扩展，支持更多的token的识别的话，需要修改language.h，language.cpp和lexical_analyzer.cpp三个文件

## 目前实现的功能：
1. 能够对大部分C语言的关键字，运算符和各种常数进行识别，包括字符串常量和字符常量
2. 支持在字符常量和字符串常量中使用转义字符
3. 支持无限长的字符串
4. 限制标识符和除了字符串常量之外的所有常量的长度
5.  能够在词法分析阶段发现如下错误：  
    ```ILLEGAL_CHAR,                           //非法字符  
    ID_TOO_LONG,                            //标识符长度过长  
    CONST_INT_TOO_LONG,                     //整常数长度过长  
    CONST_DOUBLE_TOO_LONG,                  //实常数长度过长  
    CONST_CHAR_TOO_LONG,                    //字符长度过长  
    UNMATCHED_BRACKET,                      //不匹配的括号  
    UNCLOSED_LEFT_BRACKET,                  //不闭合的左括号  
    UNCLOSED_RIGHT_BRACKET,                 //不闭合的右括号  
    NONEXISTENT_ESCAPE_CHAR,                //不存在的转义字符  
    UNCLOSED_CONST_CHAR,                    //不闭合的常量字符  
    UNCLOSED_CONST_STRING,                  //不闭合的常量字符
    ```
6. 能够定位错误发生的行数
7. 能够进行简单的错误恢复

## 使用的机制：
1. 双缓冲区
2. 超前搜索