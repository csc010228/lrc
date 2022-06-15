/*
 *
 *  syntax_analyzer.cpp
 *  语法分析器
 *  Creator:Sichao Chen
 *  Create time:2021/10/09
 *
*/
#include<fstream>
#include<sstream>
#include<iostream>
#include<list>
#include<stdlib.h>
#include<algorithm>

#include "grammer.h"
#include "lexical_analyzer.h"

//拓广文法的起始产生式的字符串
#define AUGMENTED_GRAMMER_START_PRODUCTION_STRING "SysY"

//判断某一个语法符号是不是常规终结符(即不包括空符号和栈底符号)
#define IS_NORMAL_TERMINATOR(syntax_symbol_index) (syntax_symbol_index>=0 && syntax_symbol_index<terminator_num_-2)

//判断某一个语法符号是不是终结符(包括空符号和栈底符号)
#define IS_TERMINATOR(syntax_symbol_index) (syntax_symbol_index>=0 && syntax_symbol_index<terminator_num_)

//判断某一个语法符号是不是语法变量
#define IS_SYNTAX_VARIABLE(syntax_symbol_index) (syntax_symbol_index>=terminator_num_ && syntax_symbol_index<syntax_symbol_num_)

//语义动作函数表
extern map<string,semantic_rule> semantic_rules;

//语义动作注册函数表
extern map<string,semantic_rule_register> semantic_rule_registers;

/*
生成一个随机的，以双下划线__开头的新生成的语法变量的名字
*/
string Grammer::generate_a_new_syntax_variable_name_()
{
    static int new_syntax_variable_num=0;

    return "__"+to_string(new_syntax_variable_num++);
}

/*
文法的构造函数

Parameters
----------
filename:书写文法的文本文件名
terminators:文法的终结符字典
is_augmented:是否将该文法自动转换成拓广文法
*/
Grammer::Grammer(const char * filename,map<string,syntax_symbol_index> terminators,bool is_augmented):is_augmented_(is_augmented)
{
    string line,str,new_syntax_symbol_str;
    int line_length,head,tail;
    syntax_symbol_index syntax_symbol_index_found,old_start_symbol;
    struct production * pr,*new_pr;
    bool is_semantic_rule,semantic_rule_not_register=false,warning_info=false;
    string left_string,semantic_rule_str;
    vector<string> right_string;

    //语言的文法
    string grammer_str="COMP_UNIT                                   \nCOMP_UNIT -> DECL_or_FUNC_DEF  ___COMP_UNIT_1___               编译单元\nCOMP_UNIT -> DECL_or_FUNC_DEF COMP_UNIT  ___COMP_UNIT_2___                \nDECL_or_FUNC_DEF -> DECL  ___DECL_or_FUNC_DEF_1___                        声明或者函数定义\nDECL_or_FUNC_DEF -> FUNC_DEF  ___DECL_or_FUNC_DEF_2___                    \nDECL -> CONST_DECL  ___DECL_1___                              声明\nDECL -> VAR_DECL  ___DECL_2___                                \nB_TYPE -> int  ___B_TYPE_1___                                   基本数据类型\nB_TYPE -> float  ___B_TYPE_2___                                 \nCONST_DECL -> const B_TYPE one_or_more_CONST_DEF_divided_with_comma ;  ___CONST_DECL___          常量声明\none_or_more_CONST_DEF_divided_with_comma -> CONST_DEF , one_or_more_CONST_DEF_divided_with_comma  ___one_or_more_CONST_DEF_divided_with_comma_1___              一个或者多个用逗号隔开的CONST_DEF         \none_or_more_CONST_DEF_divided_with_comma -> CONST_DEF  ___one_or_more_CONST_DEF_divided_with_comma_2___               \nCONST_DEF -> id zero_or_more_CONST_EXP_surrounded_by_bracket = CONST_INIT_VAL  ___CONST_DEF___             常量定义\nzero_or_more_CONST_EXP_surrounded_by_bracket -> @  ___zero_or_more_CONST_EXP_surrounded_by_bracket_1___               零个或多个被中括号包围的CONST_EXP\nzero_or_more_CONST_EXP_surrounded_by_bracket -> [ CONST_EXP ] zero_or_more_CONST_EXP_surrounded_by_bracket  ___zero_or_more_CONST_EXP_surrounded_by_bracket_2___                  \nCONST_INIT_VAL -> CONST_EXP  ___CONST_INIT_VAL_1___             常量初值\nCONST_INIT_VAL -> { zero_or_more_CONST_INIT_VAL_divided_with_comma }  ___CONST_INIT_VAL_2___                   \nzero_or_more_CONST_INIT_VAL_divided_with_comma -> @  ___zero_or_more_CONST_INIT_VAL_divided_with_comma_1___              零个或多个用逗号隔开的CONST_INIT_VAL\nzero_or_more_CONST_INIT_VAL_divided_with_comma -> CONST_INIT_VAL  ___zero_or_more_CONST_INIT_VAL_divided_with_comma_2___             \nzero_or_more_CONST_INIT_VAL_divided_with_comma -> CONST_INIT_VAL , zero_or_more_CONST_INIT_VAL_divided_with_comma  ___zero_or_more_CONST_INIT_VAL_divided_with_comma_3___             \nVAR_DECL -> B_TYPE one_or_more_VAR_DEF_divided_with_comma ;  ___VAR_DECL___             变量声明\none_or_more_VAR_DEF_divided_with_comma -> VAR_DEF  ___one_or_more_VAR_DEF_divided_with_comma_1___                       一个或者多个用逗号隔开的VAR_DEF\none_or_more_VAR_DEF_divided_with_comma -> VAR_DEF , one_or_more_VAR_DEF_divided_with_comma  ___one_or_more_VAR_DEF_divided_with_comma_2___              \nVAR_DEF -> id zero_or_more_CONST_EXP_surrounded_by_bracket  ___VAR_DEF_1___              变量定义\nVAR_DEF -> id zero_or_more_CONST_EXP_surrounded_by_bracket = INIT_VAL  ___VAR_DEF_2___                   \nINIT_VAL -> EXP  ___INIT_VAL_1___                 变量初值\nINIT_VAL -> { zero_or_more_INIT_VAL_divided_with_comma }  ___INIT_VAL_2___                          \nzero_or_more_INIT_VAL_divided_with_comma -> @  ___zero_or_more_INIT_VAL_divided_with_comma_1___              零个或多个用逗号隔开的INIT_VAL\nzero_or_more_INIT_VAL_divided_with_comma -> INIT_VAL  ___zero_or_more_INIT_VAL_divided_with_comma_2___             \nzero_or_more_INIT_VAL_divided_with_comma -> INIT_VAL , zero_or_more_INIT_VAL_divided_with_comma  ___zero_or_more_INIT_VAL_divided_with_comma_3___             \nFUNC_DEF -> B_TYPE id ( )  ___FUNC_DEF_1___ BLOCK  ___FUNC_DEF_2___                函数定义\nFUNC_DEF -> B_TYPE id ( FUNC_F_PARAMS )  ___FUNC_DEF_3___ BLOCK  ___FUNC_DEF_4___                  \nFUNC_DEF -> void id ( )  ___FUNC_DEF_5___ BLOCK  ___FUNC_DEF_6___                \nFUNC_DEF -> void id ( FUNC_F_PARAMS )  ___FUNC_DEF_7___ BLOCK  ___FUNC_DEF_8___                   \nFUNC_F_PARAMS -> one_or_more_FUNC_F_PARAM_divided_with_comma  ___FUNC_F_PARAMS___            函数形参表\none_or_more_FUNC_F_PARAM_divided_with_comma -> FUNC_F_PARAM  ___one_or_more_FUNC_F_PARAM_divided_with_comma_1___             一个或者多个用逗号隔开的FUNC_F_PARAM\none_or_more_FUNC_F_PARAM_divided_with_comma -> FUNC_F_PARAM , one_or_more_FUNC_F_PARAM_divided_with_comma  ___one_or_more_FUNC_F_PARAM_divided_with_comma_2___           \nFUNC_F_PARAM -> B_TYPE id  ___FUNC_F_PARAM_1___              函数形参\nFUNC_F_PARAM -> B_TYPE id [ ] zero_or_more_EXP_surrounded_by_bracket  ___FUNC_F_PARAM_2___            \nzero_or_more_EXP_surrounded_by_bracket -> @  ___zero_or_more_EXP_surrounded_by_bracket_1___               零个或多个被中括号包围的EXP\nzero_or_more_EXP_surrounded_by_bracket -> [ EXP ] zero_or_more_EXP_surrounded_by_bracket  ___zero_or_more_EXP_surrounded_by_bracket_2___            \nBLOCK -> {  ___BLOCK_1___ zero_or_more_BLOCK_ITEM }  ___BLOCK_2___                语句块\nzero_or_more_BLOCK_ITEM -> @  ___zero_or_more_BLOCK_ITEM_1___                        零个或者多个BLOCK_ITEM\nzero_or_more_BLOCK_ITEM -> BLOCK_ITEM zero_or_more_BLOCK_ITEM  ___zero_or_more_BLOCK_ITEM_2___           \nBLOCK_ITEM -> DECL  ___BLOCK_ITEM_1___                  语句块项\nBLOCK_ITEM -> STMT  ___BLOCK_ITEM_2___                  \nSTMT -> BLOCK  ___STMT_1___                   语句\nSTMT -> L_VAL = EXP ;  ___STMT_2___               \nSTMT -> ;  ___STMT_3___                       \nSTMT -> EXP ;  ___STMT_4___               \nSTMT -> IF_STMT  ___STMT_5___               \nSTMT -> WHILE_STMT  ___STMT_6___             \nSTMT -> JMP_STMT  ___STMT_7___             \nJMP_STMT -> break ;  ___JMP_STMT_1___                         跳转语句\nJMP_STMT -> continue ;  ___JMP_STMT_2___                  \nJMP_STMT -> return ;  ___JMP_STMT_3___                        \nJMP_STMT -> return EXP ;  ___JMP_STMT_4___                        \nWHILE_STMT -> while  ___WHILE_STMT_1___ ( COND ) STMT  ___WHILE_STMT_2___            while语句\nIF_STMT -> if ( COND ) STMT  ___IF_STMT_1___ else STMT  ___IF_STMT_2___               \nIF_STMT -> if ( COND ) STMT  ___IF_STMT_3___                 if语句\nEXP -> ADD_EXP  ___EXP___              表达式（注：SysY表达式是int型表达式）\nCOND -> L_OR_EXP  ___COND___            条件表达式\nL_VAL -> id zero_or_more_EXP_surrounded_by_bracket  ___L_VAL___              \nPRIMARY_EXP -> ( EXP )  ___PRIMARY_EXP_1___          基本表达式\nPRIMARY_EXP -> L_VAL  ___PRIMARY_EXP_2___                \nPRIMARY_EXP -> NUMBER  ___PRIMARY_EXP_3___               \nNUMBER -> const_int  ___NUMBER_1___             数值\nNUMBER -> const_float  ___NUMBER_2___             \nUNARY_EXP -> PRIMARY_EXP  ___UNARY_EXP_1___                一元表达式\nUNARY_EXP -> id ( )  ___UNARY_EXP_2___                     \nUNARY_EXP -> id ( FUNC_R_PARAMS )  ___UNARY_EXP_3___           \nUNARY_EXP -> UNARY_OP UNARY_EXP  ___UNARY_EXP_4___                 \nUNARY_OP -> +  ___UNARY_OP_1___               单目运算符（注：!仅出现在条件表达式中）\nUNARY_OP -> -  ___UNARY_OP_2___               \nUNARY_OP -> !  ___UNARY_OP_3___               \nFUNC_R_PARAMS -> EXP  ___FUNC_R_PARAMS_1___            函数实参表\nFUNC_R_PARAMS -> EXP , FUNC_R_PARAMS  ___FUNC_R_PARAMS_2___            \nMUL_EXP -> UNARY_EXP  ___MUL_EXP_1___            乘除模表达式\nMUL_EXP -> MUL_EXP * UNARY_EXP  ___MUL_EXP_2___          \nMUL_EXP -> MUL_EXP / UNARY_EXP  ___MUL_EXP_3___          \nMUL_EXP -> MUL_EXP % UNARY_EXP  ___MUL_EXP_4___          \nADD_EXP -> MUL_EXP  ___ADD_EXP_1___              加减表达式\nADD_EXP -> ADD_EXP + MUL_EXP  ___ADD_EXP_2___            \nADD_EXP -> ADD_EXP - MUL_EXP  ___ADD_EXP_3___            \nREL_EXP -> ADD_EXP  ___REL_EXP_1___              关系表达式\nREL_EXP -> REL_EXP < ADD_EXP  ___REL_EXP_2___            \nREL_EXP -> REL_EXP > ADD_EXP  ___REL_EXP_3___            \nREL_EXP -> REL_EXP <= ADD_EXP  ___REL_EXP_4___            \nREL_EXP -> REL_EXP >= ADD_EXP  ___REL_EXP_5___            \nEQ_EXP -> REL_EXP  ___EQ_EXP_1___               相等性表达式\nEQ_EXP -> EQ_EXP == REL_EXP  ___EQ_EXP_2___             \nEQ_EXP -> EQ_EXP != REL_EXP  ___EQ_EXP_3___                \nL_AND_EXP -> EQ_EXP  ___L_AND_EXP_1___             逻辑与表达式\nL_AND_EXP -> L_AND_EXP &&  ___L_AND_EXP_2___ EQ_EXP  ___L_AND_EXP_3___            \nL_OR_EXP -> L_AND_EXP  ___L_OR_EXP_1___           逻辑或表达式\nL_OR_EXP -> L_OR_EXP ||  ___L_OR_EXP_2___ L_AND_EXP  ___L_OR_EXP_3___           \nCONST_EXP -> ADD_EXP  ___CONST_EXP___            常量表达式（注：使用的id必须是常量）";
    stringstream inFile(grammer_str);

    // //创建一个文件输入流对象
	// ifstream inFile;
	// //打开文件
	// inFile.open(filename);
	// if (!inFile)
    // {
    //     cout << "Grammer file do not exist!" << endl;
    //     return;
	// }


    terminators_=terminators;
    terminator_num_=terminators_.size();
    syntax_variable_num_=0;
    syntax_symbol_num_=0;
    firsts_=nullptr;
    follows_=nullptr;
    syntax_variables_derive_empty_=nullptr;
    productions_=new vector<struct production * >;
    //约定空符号的编号是终结符个数减1
    empty_symbol_=terminator_num_-1;
    //约定栈底符号的编号是终结符个数减2
    stack_bottom_symbol_=terminator_num_-2;

    //第一行读取拓广文法的起始符号，最后要有至少一个空格
    if(getline(inFile,line))
    {
        //第一行的第一个空格之前的字符串认为是拓广文法的起始符号
        if(line.find(" ")==line.npos)
        {
            return;
        }
        str=line.substr(0,line.find(" "));
        syntax_variables_[str]=(syntax_variable_num_++)+terminator_num_;
        start_symbol_=terminator_num_;
    }

	//按行读取文件内容,每一行都是一个产生式
    //产生式中的文法符号用一个空格隔开
    //产生式中的语义动作用两个空格隔开
    //这一行的最后一定要至少有三个空格，在这三个空格之后的内容都是注释，不会被读入
	while (getline(inFile,line))
	{
        if(line[0]=='#')
        {
            //如果第一个字符是#，说明这一行是注释，直接跳过
            continue;
        }
        //最后一定要至少有三个空格，在这三个空格之后的内容都是注释，不会被读入
        head=line.find("   ");
        if(head==line.npos)
        {
            return;
        }
        line=line.substr(0,head);

        pr=new struct production;
        pr->production_string=line.substr(0,head);

        line_length=line.length();
        right_string.clear();
        //第一个符号认为是产生式的左部的非终结符
        if((head=line.find(" "))==line.npos)
        {
            delete pr;
            return;
        }
        left_string=line.substr(0,head);
        if(syntax_variables_.find(left_string)==syntax_variables_.end())
        {
            syntax_variables_[left_string]=(syntax_variable_num_++)+terminator_num_;
        }
        pr->left=syntax_variables_[left_string];

        //第二个符号一定要是->，并且->后面一定要有一个空格
        if((tail=line.find(" ",++head))==line.npos || line.substr(head,tail-head)!="->")
        {
            delete pr;
            return;
        }

        //接下来就是产生式的右部的符号以及语义动作了
        do
        {
            head=tail+1;
            if(line[head]==' ')
            {
                //如果连续出现两个空格，说明此时这个位置是一个语义动作
                if(is_semantic_rule)
                {
                    //不能连续出现两个语义动作
                    delete pr;
                    return;
                }
                head++;
                is_semantic_rule=true;
            }
            else
            {
                //如果只有一个空格，说明此时这个位置是一个语法符号
                is_semantic_rule=false;
            }
            tail=line.find(" ",head);
            str=line.substr(head,tail-head);
            if(is_semantic_rule)
            {
                //如果此时找到了一个语义动作，先判断这个语义动作是不是位于该产生式的最后
                if(tail==line.npos)
                {
                    //如果这个语义动作位于该产生式的最后，那么就直接将其作为该产生式对应的语义动作即可
                    production_semantic_rule_.push_back(semantic_rules[str]);
                    if(semantic_rule_registers.find(str)!=semantic_rule_registers.end())
                    {
                        semantic_rule_registers[str](left_string,right_string,"");
                    }
                    else
                    {
                        if(!warning_info)
                        {
                            warning_info=true;
                            cout<<"Warning: There are some semantic rules not defined yet!"<<endl;
                        }
                    }
                }
                else
                {
                    //如果该语义动作不是位于产生式的最后，需要新建一个产生式和语法变量
                    //新建的语法变量以两个下划线__开头，因此要求在定义语法的时候语法变量不能以双下划线开头
                    new_syntax_symbol_str=generate_a_new_syntax_variable_name_();
                    syntax_symbol_index_found=(syntax_variable_num_++)+terminator_num_;
                    syntax_variables_[new_syntax_symbol_str]=syntax_symbol_index_found;
                    //新建一个产生式
                    new_pr=new struct production;
                    new_pr->production_string=new_syntax_symbol_str+" -> @";
                    new_pr->left=syntax_symbol_index_found;
                    new_pr->right.push_back(empty_symbol_);
                    //将原本的语义动作进行适当的更改之后，作为新生成的产生式的语义动作
                    production_semantic_rule_.push_back(semantic_rules[str]);
                    //semantic_rule_registers[str](left_string,right_string);
                    semantic_rule_str=str;
                    semantic_rule_not_register=true;
                    //将新生成的产生式加入产生式集合中
                    productions_->push_back(new_pr);
                    //将用户定义的产生式对应的语义动作的位置替换成新定义的语法变量
                    pr->right.push_back(syntax_symbol_index_found);
                }
            }
            else
            {
                //如果此时找到的是一个语法符号
                if(semantic_rule_not_register)
                {
                    //semantic_rule_registers[semantic_rule_str](left_string,right_string,str);
                    if(semantic_rule_registers.find(semantic_rule_str)!=semantic_rule_registers.end())
                    {
                        semantic_rule_registers[semantic_rule_str](left_string,right_string,str);
                    }
                    else
                    {
                        if(!warning_info)
                        {
                            warning_info=true;
                            cout<<"Warning: There are some semantic rules not defined yet!"<<endl;
                        }
                    }
                    right_string.push_back(new_syntax_symbol_str);
                    semantic_rule_not_register=false;
                }
                //判断是终结符还是语法变量
                if(terminators_.find(str)!=terminators_.end())
                {
                    syntax_symbol_index_found=terminators_[str];
                }
                else
                {
                    if(syntax_variables_.find(str)==syntax_variables_.end())
                    {
                        syntax_variables_[str]=(syntax_variable_num_++)+terminator_num_;
                    }
                    syntax_symbol_index_found=syntax_variables_[str];
                }
                if(syntax_symbol_index_found!=empty_symbol_)
                {
                    right_string.push_back(str);
                }
                pr->right.push_back(syntax_symbol_index_found);
            }
        }while(tail!=line.npos);

        if(!is_semantic_rule)
        {
            //production_semantic_rule_[productions_->size()]=nullptr;
            production_semantic_rule_.push_back(nullptr);
        }

        //将产生式加入文法
        productions_->push_back(pr);
	}

    syntax_symbol_num_=terminator_num_+syntax_variable_num_;

    if(is_augmented)
    {
        //给文法添加新的起始符号和产生式，将其转换成拓广文法
        //将拓广文法的起始符号加入文法中
        old_start_symbol=start_symbol_;
        start_symbol_=syntax_symbol_num_++;
        syntax_variable_num_++;

        //将拓广文法的起始表达式S'->S加入文法产生式的开头
        pr=new struct production;
        pr->left=start_symbol_;
        pr->right.push_back(old_start_symbol);
        pr->production_string=AUGMENTED_GRAMMER_START_PRODUCTION_STRING;
        productions_->insert(productions_->begin(),pr);
        //拓广文法的第一个产生式没有语义动作
        production_semantic_rule_.insert(production_semantic_rule_.begin(),nullptr);
    }
    
    //构造文法的FIRST集和FOLLOW
    //下面三个函数的顺序不能改变
    get_all_symbol_variables_derive_empty();
    get_all_firsts();
    get_all_follows();

    //关闭文件
    // inFile.close();
}

/*
Grammer的析构函数
*/
Grammer::~Grammer()
{
    //释放所有产生式
    for(auto i : (*productions_))
    {
        delete i;
    }
    delete productions_;

    //释放所有语法符号的FIRST集和所有语法变量的FOLLOW集
    if(firsts_ && follows_)
    {
        for(int i=0;i<syntax_symbol_num_;i++)
        {
            delete firsts_[i];
            if(i>=terminator_num_)
            {
                delete follows_[i-terminator_num_];
            }
        }
        free(firsts_);
        free(follows_);
    }

    //释放syntax_variables_derive_empty_
    if(syntax_variables_derive_empty_)
    {
        free(syntax_variables_derive_empty_);
    }
}

/*
获取所有的能够派生出空符号的语法变量
*/
void Grammer::get_all_symbol_variables_derive_empty()
{
    vector<syntax_symbol_index> right;
    bool tag=true,tag2;

    syntax_variables_derive_empty_=(bool *)malloc(syntax_variable_num_*sizeof(bool));

    //先把所有的语法变量都认为是不能推导出空符号
    for(syntax_symbol_index syn_var=0;syn_var<syntax_symbol_num_-terminator_num_;syn_var++)
    {
        syntax_variables_derive_empty_[syn_var]=false;
    }

    //查看有没有能一步直接推导出空符号的语法变量
    for(auto i : (*productions_))
    {
        if(i->right.size()==1 && i->right[0]==empty_symbol_)
        {
            syntax_variables_derive_empty_[i->left-terminator_num_]=true;
        }
    }

    //接着反复检查所有语法变量能不能推导出空符号,直到有一次检查没有改变任何东西
    while(tag)
    {
        tag=false;
        for(auto i : (*productions_))
        {
            if(syntax_variables_derive_empty_[i->left-terminator_num_]==true)
            {
                continue;
            }
            right=i->right;
            tag2=true;
            for(auto j : right)
            {
                if((IS_TERMINATOR(j) && j!=empty_symbol_) || syntax_variables_derive_empty_[j-terminator_num_]==false)
                {
                    tag2=false;
                    break;
                }
            }
            if(tag2)
            {
                syntax_variables_derive_empty_[i->left-terminator_num_]=true;
                tag=true;
            }
        }
    }
}

/*
判断一个语法符号能不能派生出空符号
调用这个函数之前必须先执行get_all_symbol_variables_derive_empty()

Parameters
----------
syn_sym:要判断的语法符号编号
*/
bool Grammer::derive_empty(syntax_symbol_index syn_sym)
{
    return IS_TERMINATOR(syn_sym)?(syn_sym==empty_symbol_):syntax_variables_derive_empty_[syn_sym];
}

/*
判断一个符号串能不能派生出空符号
Parameters
----------
string:要判断的符号串
*/
bool Grammer::string_derive_empty(vector<syntax_symbol_index> string)
{
    for(auto i : string)
    {
        if(derive_empty(i)==false)
        {
            return false;
        }
    }
    return true;
}

/*
将一个语法符号集合去除空符号

Parameters
----------
syntax_symbol_set:要去除空符号的集合
*/
set<syntax_symbol_index> Grammer::set_difference_empty(set<syntax_symbol_index> syntax_symbol_set)
{
    for(set<syntax_symbol_index>::iterator i=syntax_symbol_set.begin();i!=syntax_symbol_set.end();i++)
    {
        if((*i)==empty_symbol_)
        {
            syntax_symbol_set.erase(i);
            break;
        }
    }
    return syntax_symbol_set;
}

/*
获取语法符号的FIRST集
调用该函数之前必须调用get_all_firsts()

Parameters
----------
syn_sym:要求FIRST集的语法符号的编号

Return
------
返回一个set集合表示该变量的FIRST集的指针
*/
set<syntax_symbol_index> * Grammer::first(syntax_symbol_index syn_sym)
{
    return firsts_[syn_sym];
}

/*
获取语法符号的FOLLOW集
调用该函数之前必须调用get_all_follows()

Parameters
----------
syn_sym:要求FOLLOW集的语法符号的编号

Return
------
返回一个set集合表示该变量的FOLLOW集
*/
set<syntax_symbol_index> * Grammer::follow(syntax_symbol_index syn_var)
{
    return follows_[syn_var-terminator_num_];
}

/*
获取整个文法的所有语法符号的FIRST集
*/
void Grammer::get_all_firsts()
{
    syntax_symbol_index syn_sym;
    set<syntax_symbol_index> * tmp_first,tmp;
    vector<syntax_symbol_index> right;
    int old_size;
    bool tag=true,tag2;

    firsts_=(set<syntax_symbol_index> **)malloc(syntax_symbol_num_*sizeof(set<syntax_symbol_index> *));

    //遍历所有的语法符号,构造它们最初的FIRST集
    for(syn_sym=0;syn_sym<syntax_symbol_num_;syn_sym++)
    {
        tmp_first=new set<syntax_symbol_index>;
        if(syn_sym<terminator_num_)
        {
            //如果是常规终结符
            tmp_first->insert(syn_sym);
        }
        else
        {
            //如果是非终结符
            for(auto i : (*productions_))
            {
                if(i->left==syn_sym)
                {
                    if(IS_NORMAL_TERMINATOR(i->right[0]))
                    {
                        tmp_first->insert(i->right[0]);
                    }
                    else if(i->right.size()==1 && i->right[0]==empty_symbol_)
                    {
                        tmp_first->insert(empty_symbol_);
                    }
                }
            }
        }
        firsts_[syn_sym]=tmp_first;
    }

    //接着遍历所有的语法变量,不断计算它们的FIRST集,直到所有的FIRST集都不再改变为止
    while(tag)
    {
        tag=false;
        for(auto i : (*productions_))
        {
            tmp_first=firsts_[i->left];
            old_size=tmp_first->size();
            right=i->right;

            if(IS_SYNTAX_VARIABLE(right[0]))
            {
                tmp=set_difference_empty(*(firsts_[right[0]]));
                set_union(tmp.begin(),tmp.end(),tmp_first->begin(),tmp_first->end(),inserter(*tmp_first,tmp_first->begin()));

                if(derive_empty(right[0]))
                {
                    tag2=true;
                    for(vector<syntax_symbol_index>::iterator j=right.begin();j!=right.end();j++)
                    {
                        if(!derive_empty(*j))
                        {
                            tag2=false;
                            for(vector<syntax_symbol_index>::iterator k=right.begin()+1;k!=j+1;k++)
                            {
                                tmp=set_difference_empty(*(firsts_[*k]));
                                set_union(tmp.begin(),tmp.end(),tmp_first->begin(),tmp_first->end(),inserter(*tmp_first,tmp_first->begin()));
                            }
                        }
                    }
                    if(tag2)
                    {
                        tmp_first->insert(empty_symbol_);
                    }
                }
            }

            tag|=(old_size!=tmp_first->size());
        }
    }
}

/*
获取一个符号串的FIRST集
调用该函数之前必须调用get_all_firsts()

Parameters
----------
string:要获取FIRST集的符号串
*/
set<syntax_symbol_index> Grammer::string_first(vector<syntax_symbol_index> string)
{
    int string_length=string.size(),i;
    set<syntax_symbol_index> result,tmp;
    if(string_length!=0)
    {
        result=set_difference_empty(*first(string[0]));

        i=0;
        while(first(string[i])->find(empty_symbol_)!=first(string[i])->end() && i<string_length-1)
        {
            tmp=set_difference_empty(*first(string[i+1]));
            set_union(result.begin(),result.end(),tmp.begin(),tmp.end(),inserter(result,result.begin()));
            i++;
        }

        if(first(string_length-1)->find(empty_symbol_)!=first(string_length-1)->end())
        {
            result.insert(empty_symbol_);
        }
    }
    return result;
}

/*
获取整个文法的所有语法变量的FOLLOW集
*/
void Grammer::get_all_follows()
{
    syntax_symbol_index syn_var,A,B;
    vector<syntax_symbol_index> right,string;
    set<syntax_symbol_index> *tmp_follow,tmp;
    int old_size;
    bool tag=true;

    follows_=(set<syntax_symbol_index> **)malloc(syntax_variable_num_*sizeof(set<syntax_symbol_index> *));

    //先把所有的语法变量的FOLLOW集设置为空集
    for(syn_var=0;syn_var<syntax_variable_num_;syn_var++)
    {
        follows_[syn_var]=new set<syntax_symbol_index>;
    }
    //把起始符号的FOLLOW集设置为只有一个栈底符号
    follows_[start_symbol_-terminator_num_]->insert(stack_bottom_symbol_);
    //遍历所有的产生式，计算所有语法变量的FOLLOW集，直到所有的FOLLOW集都不再变化为止
    while(tag)
    {
        tag=false;
        for(vector<struct production * >::iterator i=productions_->begin();i!=productions_->end();i++)
        {
            A=(*i)->left-terminator_num_;
            right=(*i)->right;
            for(vector<syntax_symbol_index>::iterator j=right.begin();j!=right.end();j++)
            {
                B=(*j);
                if(IS_SYNTAX_VARIABLE(B))
                {
                    B-=terminator_num_;
                    tmp_follow=follows_[B];
                    old_size=tmp_follow->size();
                    string=vector<syntax_symbol_index>(j+1,right.end());
                    tmp=set_difference_empty(string_first(string));
                    set_union(tmp_follow->begin(),tmp_follow->end(),tmp.begin(),tmp.end(),inserter(*tmp_follow,tmp_follow->begin()));

                    if(j==right.end()-1 || (string_derive_empty(string) && A!=B))
                    {
                        set_union(tmp_follow->begin(),tmp_follow->end(),follows_[A]->begin(),follows_[A]->end(),inserter(*tmp_follow,tmp_follow->begin()));
                    }
                    tag|=(old_size!=tmp_follow->size());
                }
            }
        }
    }
}

/*
查看该文法是否建立成功
*/
bool Grammer::is_init_success()
{
    return syntax_symbol_num_!=0;
}