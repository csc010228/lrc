/*
 *
 *  syntax_directed_translator.cpp
 *  LR(1)语法分析对应的语法制导翻译
 *  Creator:Sichao Chen
 *  Create time:2021/11/04
 *
*/

#include "syntax_directed_translator.h"
#include<fstream>
#include<sstream>

//中间代码输出信息
extern map<ic_op,ic_output> ic_outputs;

/*
构造函数
(同LR1_parser的三个构造函数)
*/
Syntax_directed_translator::Syntax_directed_translator()
{

}

/*
初始化
*/
bool Syntax_directed_translator::init(const char * productions_filename,const char * parser_table_filename,list<struct token * > * tokens,bool print_warning)
{
    //先调用父类LR1_parser的init函数
    bool res=LR1_parser::init(productions_filename,parser_table_filename,tokens,print_warning);

    //再进行自身的init
    if(res)
    {
        stack_top_=0;
        semantic_stack_=new vector<map<string,union semantic_stack_node_member> >;
    }
    else
    {
        semantic_stack_=nullptr;
    }

    return res;
}

/*
析构函数
*/
Syntax_directed_translator::~Syntax_directed_translator()
{
    if(semantic_stack_)
    {
        delete semantic_stack_;
    }
}

/*
查看语义栈是否需要扩展
*/
void Syntax_directed_translator::check_semantic_stack()
{
    while(stack_top_ >= semantic_stack_->size())
    {
        semantic_stack_->push_back(map<string,union semantic_stack_node_member>());
    }
}

/*
往语义栈的栈顶插入

Parameters
----------
semantic_node:要插入的元素
*/
void Syntax_directed_translator::push_semantic_stack(map<string,union semantic_stack_node_member> semantic_stack_node_member)
{
    check_semantic_stack();
    semantic_stack_->at(stack_top_++)=semantic_stack_node_member;
}

/*
往语义栈中插入

Parameters
----------
key:要插入的键
value:要插入的值
*/
void Syntax_directed_translator::push_semantic_stack(string key,union semantic_stack_node_member value)
{
    check_semantic_stack();
    semantic_stack_->at(stack_top_++)[key]=value;
}

/*
往语义栈中插入
*/
void Syntax_directed_translator::push_semantic_stack()
{
    check_semantic_stack();
    stack_top_++;
}

/*
从语义栈中弹出
*/
void Syntax_directed_translator::pop_semantic_stack()
{
    semantic_stack_->at(--stack_top_).clear();
}

/*
进行语义分析

Return
------
返回生成的中间代码序列(用四元式表示)对应的流图
*/
list<struct quaternion> * Syntax_directed_translator::translate()
{
    int tag=-1;
    list<struct quaternion> * intermediate_codes;
    struct action_entry entry;
    struct token * input_token=next_token();
    terminator_index input_token_code_of_kind=input_token?(terminator_index)input_token->code_of_kind:((tag==-1)?tag=grammer_->stack_bottom_symbol_:-1);
    vector<struct production * > * productions=grammer_->productions_;
    struct production * production;
    state next_state;
    semantic_rule rule;
    map<string,union semantic_stack_node_member> new_semantic_stack_node;
    Symbol_table * symbol_table=Symbol_table::get_instance();

    //先给符号栈和状态栈赋初始值
    state_stack_.push(0);

    //然后开始按照action表和goto表进行语法分析
    if(input_token_code_of_kind==-1)
    {
        return nullptr;
    }
    intermediate_codes=new list<struct quaternion>;

    while(true)
    {
        entry=get_action_entry(state_stack_.top(),input_token_code_of_kind);
        switch(entry.movement)
        {
            case action_movement::SHIFT:
            //cout<<"SHIFT"<<endl;
            //cout<<input_token_code_of_kind<<endl;
                switch(code_of_kind(input_token_code_of_kind))
                {
                    case code_of_kind::ID:
                        push_semantic_stack("id_string",{.pointer_value=(input_token->attribute_value.p_string_value)});
                        break;
                    case code_of_kind::CONST_INT:
                        push_semantic_stack("int_value",{.int_value=(input_token->attribute_value.int_value)});
                        break;
                    case code_of_kind::CONST_FLOAT:
                        push_semantic_stack("float_value",{.float_value=(input_token->attribute_value.float_value)});
                        break;
                    case code_of_kind::CONST_CHAR:
                        push_semantic_stack("char_value",{.char_value=(input_token->attribute_value.char_value)});
                        break;
                    case code_of_kind::CONST_STRING:
                        push_semantic_stack("string_value",{.pointer_value=(input_token->attribute_value.p_string_value)});
                        break;
                    default:
                        push_semantic_stack();
                        break;
                }

                state_stack_.push(entry.index);
                input_token=next_token();
                input_token_code_of_kind=input_token?(terminator_index)input_token->code_of_kind:((tag==-1)?tag=grammer_->stack_bottom_symbol_:-1);
                if(input_token_code_of_kind==-1)
                {
                    delete intermediate_codes;
                    return nullptr;
                }
                break;
            case action_movement::REDUCE:
            //cout<<"REDUCE:"<<endl;
                production=productions->at(entry.index);
            //cout<<production->production_string<<endl;
                //执行语义动作
                rule=grammer_->production_semantic_rule_.at(entry.index);
                if(rule)
                {
                    check_semantic_stack();
                    new_semantic_stack_node=rule(intermediate_codes,semantic_stack_,stack_top_,symbol_table);
                }
                else
                {
                    new_semantic_stack_node.clear();
                }

                for(vector<syntax_symbol_index>::iterator i=production->right.begin();i!=production->right.end();i++)
                {
                    if((*i)!=grammer_->empty_symbol_)
                    {
                        pop_semantic_stack();
                        state_stack_.pop();
                    }
                }

                push_semantic_stack(new_semantic_stack_node);

                next_state=get_goto_entry(state_stack_.top(),production->left);
                
                if(next_state!=-1)
                {
                    state_stack_.push(next_state);
                }
                else
                {
                    delete intermediate_codes;
                    return nullptr;
                }
                break;
            case action_movement::ACCEPT:
            //cout<<"ACCEPT: ";
            //cout<<intermediate_codes->size()<<endl;
                return intermediate_codes;
            case action_movement::ERROR:
            //cout<<"ERROR"<<endl;
            //cout<<input_token_code_of_kind<<endl;
            //cout<<state_stack_.top()<<endl;
                delete intermediate_codes;
                return nullptr;
        }
    }

    return nullptr;
}

/*
进行语义翻译并将中间代码进行输出到文件中

Parameters
----------
filename:要进行中间代码输出的文件

Return
------
返回生成的中间代码序列(用四元式表示)对应的流图
*/
list<struct quaternion> * Syntax_directed_translator::translate_then_output(const char * filename)
{
	ofstream outFile;
    list<struct quaternion> * res;
    
    //进行语义翻译
    res=translate();
    
    if(res)
    {
	    outFile.open(filename,ios::out);
	    if (!outFile)
        {
            cout<<"Open file "<<filename<<" error!"<<endl;
	    }
        else
        {
            for(auto i:*res)
            {
                outFile<<(ic_outputs[i.op](i))<<endl;
            }
            outFile.close();
        }
    }

    return res;
}