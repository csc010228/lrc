/*
 *
 *  LR1_parser.h
 *  LR(1)语法分析器
 *  Creator:Sichao Chen
 *  Create time:2021/10/09
 *
*/
#ifndef __LR1_PARSER_H
#define __LR1_PARSER_H

#include<stack>
#include<queue>
#include<vector>
#include<map>
#include<string>

#include "lexical_analyzer.h"
#include "grammer.h"

using namespace std;

//LR(1)项目集的编号
typedef int canonical_LR1_index;

//LR(1)项目
struct LR1_item
{
    bool operator==(const LR1_item b) const
	{
		return (this->dot_position==b.dot_position && this->lookahead_symbol==b.lookahead_symbol && this->production==b.production);
	}
    bool operator<(const LR1_item b) const
	{
        if(this->dot_position!=b.dot_position)
        {
            return this->dot_position<b.dot_position;
        }
        else if(this->lookahead_symbol!=b.lookahead_symbol)
        {
            return this->lookahead_symbol<b.lookahead_symbol;
        }
        else
        {
            return (this->production)<(b.production);
        }
	}
    production_index production;            //产生式在文法中的索引
    int dot_position;                       //圆点的位置
    syntax_symbol_index lookahead_symbol;   //搜索符
};

//语法分析时的状态
typedef canonical_LR1_index state;

//action表项中能做的动作
enum class action_movement
{
    ERROR,              //出错
    REDUCE,             //归约
    SHIFT,              //移进
    ACCEPT,             //接受
};

//action表的表项
struct action_entry
{
    enum action_movement movement;      //要做的动作
    int index;                          //产生式或者状态的索引
};

//goto表的表项
typedef state goto_entry;

//定义LR(1)型的自底向上的语法分析器
class LR1_parser
{

protected:
    //从词法分析器中获得的token序列
    list<struct token * > * tokens_;

    //符号栈
    stack<syntax_symbol_index> token_stack_;

    //状态栈
    stack<state> state_stack_;

    //action
    struct action_entry * action_;

    //goto表
    goto_entry * goto_;

    //要分析的文法
    Grammer * grammer_;

    //拓广文法的起始符号
    syntax_symbol_index augmented_grammer_start_symbol_;

    //LR(1)文法的项目集规范族
    vector<set<struct LR1_item> * > canonical_LR1_collection_;

    //LR(1)分析表的状态个数
    int state_num_;

    //存储LR(1)文法的各个项目集之间的转移关系的表
    map<canonical_LR1_index,map<syntax_symbol_index,canonical_LR1_index> > canonical_LR1_go_table_;

    //私有化构造函数
    LR1_parser(const char * productions_filename,const char * parser_table_filename);

    //计算一个LR(1)项目集的闭包
    canonical_LR1_index closure(set<struct LR1_item> I);

    //计算从一个LR(1)项目闭包经过某一个输入字符之后可以到达的项目闭包
    canonical_LR1_index go(canonical_LR1_index I_index,syntax_symbol_index X);

    //构造LR(1)项目集规范族
    void build_canonical_LR1_collection();

    //构造LR(1)分析表
    bool build_LR1_table(bool print_warning);

    //打印LR(1)项目集
    void print_LR1_collection(state LR1_index);

    //获取下一个token
    struct token * next_token();

    //错误处理
    void error_handle();

    //赋值action表中的表项
    bool set_action_entry(state LR1_index,terminator_index ter_index,struct action_entry entry,bool overwrite);

    //赋值goto表中的表项
    bool set_goto_entry(state LR1_index,syntax_variable_index syn_var_index,goto_entry entry);

    //从action表中获取表项
    struct action_entry get_action_entry(state LR1_index,terminator_index ter_index);

    //从goto表中获取表项
    goto_entry get_goto_entry(state LR1_index,syntax_variable_index syn_var_index);

    //处理action表中产生的冲突
    bool handle_action_conflict(state LR1_index,terminator_index ter_index,struct action_entry entry,bool print_warning);

    //打印action表的冲突信息
    void print_action_conflict(state LR1_index,terminator_index ter_index,struct action_entry entry);

    //打印goto表的冲突信息
    void print_goto_conflict(state LR1_index,syntax_variable_index syn_var_index,goto_entry entry);

    //获取产生式右部的长度
    unsigned int sizeof_production_right(vector<syntax_symbol_index> right);

public:
    //构造函数
    LR1_parser();

    //初始化
    bool init(const char * productions_filename,const char * parser_table_filename,list<struct token * > * tokens,bool print_warning);

    //析构函数
    virtual ~LR1_parser();

    //判断语法分析器是否建立成功
    bool is_init_success();

    //进行语法分析
    queue<struct production * > * parse();

    //将语法分析得到的所有产生式输出到文件中
    void output_productions(const char * filename);
    
};

#endif //__LR1_PARSER_H