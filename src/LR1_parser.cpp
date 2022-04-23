/*
 *
 *  LR1_parser.cpp
 *  LR(1)语法分析器
 *  Creator:Sichao Chen
 *  Create time:2021/10/09
 *
*/
#include<fstream>
#include<iostream>
#include<algorithm>

#include "LR1_parser.h"

//文法的终结符的个数(包括空符号和栈底符号)
extern int terminator_num;

/*
LR1_parser构造函数
*/
LR1_parser::LR1_parser()
{
    tokens_=nullptr;
    grammer_=nullptr;
    action_=nullptr;
    goto_=nullptr;
}

/*
LR1_parser构造函数

Parameters
----------
productions_filename:保存产生式的文件
parser_table_filename:保存语法分析表的文件，如果为nullptr表示不保存此次建立的语法分析表
tokens:词法分析输出的所有tokens
*/
bool LR1_parser::init(const char * productions_filename,const char * parser_table_filename,list<struct token * > * tokens,bool print_warning)
{
    //先构造终结符字典
    extern map<string,enum code_of_kind> key_word;
    map<string,syntax_symbol_index> terminators=TERMINATOR_NOT_IN_KEY_WORD;
    for(auto i : key_word)
    {
        terminators.insert(make_pair(i.first,(syntax_symbol_index)i.second));
    }
    //terminators.insert(key_word.begin(),key_word.end());

    //构造tokens
    tokens_=tokens;

    //构造拓广文法
    grammer_=new Grammer(productions_filename,terminators,true);

    if(!grammer_->is_init_success())
    {
        cout<<"Build augmented grammer error!"<<endl;
        delete grammer_;
        grammer_=nullptr;
        action_=nullptr;
        goto_=nullptr;
        return false;
    }

    //再根据拓广文法把action和goto表建出来
    if(!build_LR1_table(print_warning))
    {
        cout<<"Build action and goto table error!"<<endl;
        delete grammer_;
        free(action_);
        free(goto_);
        grammer_=nullptr;
        action_=nullptr;
        goto_=nullptr;
        return false;
    }

    //保存action和goto表
    if(parser_table_filename!=nullptr)
    {
        
    }

    return true;
}

/*
LR1_parser析构函数
*/
LR1_parser::~LR1_parser()
{
    //删除词法分析得到的tokens
    if(tokens_)
    {
        for(auto i : *tokens_)
        {
            delete i;
        }
        delete tokens_;
    }

    //删除文法
    if(grammer_)
    {
        delete grammer_;
    }

    //删除LR(1)文法的项目集规范族
    for(vector<set<struct LR1_item> * >::iterator i=canonical_LR1_collection_.begin();i!=canonical_LR1_collection_.end();i++)
    {
        delete (*i);
    }

    //删除action表
    if(action_)
    {
        free(action_);
    }

    //删除goto表
    if(goto_)
    {
        free(goto_);
    }

}

/*
判断语法分析器是否建立成功
*/
bool LR1_parser::is_init_success()
{
    return (action_!=nullptr && goto_!=nullptr);
}

/*
计算一个LR(1)项目集的闭包
并将其加入LR(1)项目集规范族中

Parameters
----------
I:要计算闭包的项目集

Return
------
计算出来的闭包的编号(其实就是这个项目集在LR(1)规范项目集中的位置)
*/
canonical_LR1_index LR1_parser::closure(set<struct LR1_item> I)
{
    int old_size=0,right_size;
    set<struct LR1_item> * result;
    syntax_symbol_index B;
    vector<syntax_symbol_index> right,string;
    set<syntax_symbol_index> first;
    vector<struct production * > * productions=grammer_->productions_;

    if(I.empty())
    {
        return -1;
    }

    //先进行构造
    result=new set<struct LR1_item>(I.begin(),I.end());
    while(old_size!=result->size())
    {
        old_size=result->size();

        for(set<struct LR1_item>::iterator i=result->begin();i!=result->end();i++)
        {
            right=productions->at((*i).production)->right;

            right_size=sizeof_production_right(right);

            if((*i).dot_position>=right_size)
            {
                continue;
            }
            B=right[(*i).dot_position];
            if(B>=grammer_->terminator_num_)
            {
                if(right.begin()+(*i).dot_position!=right.end())
                {
                    string.assign(right.begin()+((*i).dot_position+1),right.end());
                }
                string.push_back((*i).lookahead_symbol);
                first=grammer_->string_first(string);
                for(production_index j=0;j<productions->size();j++)
                {
                    if(productions->at(j)->left==B)
                    {
                        for(set<syntax_symbol_index>::iterator k=first.begin();k!=first.end();k++)
                        {
                            result->insert({j,0,(*k)});
                        }
                    }
                }
            }
        }
    }

    //然后查看构造之后的闭包是否已经存在于LR(1)的项目集规范族内，如果已经存在的话就返回原本的位置索引
    for(canonical_LR1_index i=0;i<canonical_LR1_collection_.size();i++)
    {
        if((*(canonical_LR1_collection_[i]))==*result)
        {
            delete result;
            return i;
        }
    }

    //如果不存在的话将新构造的闭包加入LR(1)的项目集规范族
    canonical_LR1_collection_.push_back(result);

    return canonical_LR1_collection_.size()-1;
}

/*
计算从一个LR(1)项目闭包经过某一个输入字符之后可以到达的项目闭包
并把这个项目比闭包加入项目集规范族中

Parameters
----------
I_index:要计算的项目闭包的编号(其实就是这个项目集在LR(1)规范项目集中的位置)
X:输入的语法符号

Return
------
可以到达的项目闭包的编号(其实就是这个项目集在LR(1)规范项目集中的位置),如果到达不了任何的项目集就返回-1
*/
canonical_LR1_index LR1_parser::go(canonical_LR1_index I_index,syntax_symbol_index X)
{
    canonical_LR1_index result;
    set<struct LR1_item> J;
    vector<struct production * > * productions=grammer_->productions_;
    set<struct LR1_item> * I=canonical_LR1_collection_[I_index];
    vector<syntax_symbol_index> right;
    unsigned int right_size;
    for(set<struct LR1_item>::iterator i=I->begin();i!=I->end();i++)
    {
        right=productions->at((*i).production)->right;
        right_size=sizeof_production_right(right);
        if(right_size>(*i).dot_position && right[(*i).dot_position]==X)
        {
            J.insert({(*i).production,(*i).dot_position+1,(*i).lookahead_symbol});
        }
    }

    //求闭包
    result=closure(J);

    if(result!=-1)
    {
        //将这个闭包的转移情况加入闭包转移表中
        canonical_LR1_go_table_[I_index][X]=result;
    }

    return result;
}

/*
构造LR(1)项目集规范族

Return
------
LR(1)项目集规范族
*/
void LR1_parser::build_canonical_LR1_collection()
{
    set<struct LR1_item> I;
    int old_size=0;

    //构造一个只有第一个项目的[S'->.S,#]的闭包,并将其加入LR(1)项目集规范族闭包
    I.insert({0,0,grammer_->stack_bottom_symbol_});
    closure(I);

    //不断扩大LR1项目集规范族，直到其大小不再发生改变为止
    while(old_size!=canonical_LR1_collection_.size())
    {
        old_size=canonical_LR1_collection_.size();
        for(canonical_LR1_index i=0;i<canonical_LR1_collection_.size();i++)
        {
            for(syntax_symbol_index j=0;j<grammer_->syntax_symbol_num_;j++)
            {
                go(i,j);
            }
        }
    }
}

/*
处生成action表的时候产生的冲突
如果产生了冲突，那么说明此时这个文法是一个二义性文法
目前支持处理移进-归约冲突和归约-归约冲突

Parameters
----------
LR1_index:发生冲突的action表项的项目集编号(也就是状态编号)
ter_index:发生冲突的action表项的终结符编号
entry:想要赋值的新的表项

Return
------
如果该冲突能够被处理，那么就返回true，否则返回false
*/
bool LR1_parser::handle_action_conflict(state LR1_index,terminator_index ter_index,struct action_entry entry,bool print_warning)
{
    bool res=false;
    int min_index;
    set<struct LR1_item> * LR1_collection=canonical_LR1_collection_[LR1_index];

    struct action_entry old_entry=get_action_entry(LR1_index,ter_index);

    switch (old_entry.movement)
    {
        case action_movement::SHIFT:
            if(entry.movement==action_movement::REDUCE)
            {
                if(print_warning)
                {
                    //移进-归约冲突
                    //发生移进-归约冲突的时候默认采用移进优先的规则进行（这样能够处理if-else语句的二义性问题）
                    //因此这里只需要打印警告信息即可
                    cout<<"Warning: Action ( state_index: "<<LR1_index<<" , terminator_index: "<<ter_index<<" ) Shift-Reduce conflict!"<<endl;
                    cout<<"\tSHIFT "<<old_entry.index<<endl;
                    cout<<"\tREDUCE "<<grammer_->productions_->at(entry.index)->production_string<<endl;
                    cout<<"And we choose SHIFT "<<old_entry.index<<"!"<<endl;
                    cout<<"Involved LR1 collection:"<<endl;
                    print_LR1_collection(LR1_index);
                    cout<<endl;
                }
                res=true;
            }
            else
            {
                if(print_warning)
                {
                    //无法处理移进-移进冲突
                    //打印错误信息
                    print_action_conflict(LR1_index,ter_index,entry);
                }
            }
            break;
        case action_movement::REDUCE:
            if(entry.movement==action_movement::SHIFT)
            {
                //移进-归约冲突
                //发生移进-归约冲突的时候默认采用移进优先的规则进行（这样能够处理if-else语句的二义性问题）
                //因此这里需要把action对应的表项设置为SHIFT
                set_action_entry(LR1_index,ter_index,entry,true);
                if(print_warning)
                {
                    //打印警告信息
                    cout<<"Warning: Action ( state_index: "<<LR1_index<<" , terminator_index: "<<ter_index<<" ) Shift-Reduce conflict!"<<endl;
                    cout<<"\tREDUCE "<<grammer_->productions_->at(old_entry.index)->production_string<<endl;
                    cout<<"\tSHIFT "<<entry.index<<endl;
                    cout<<"And we choose SHIFT "<<entry.index<<"!"<<endl;
                    cout<<"Involved LR1 collection:"<<endl;
                    print_LR1_collection(LR1_index);
                    cout<<endl;
                }
                res=true;
            }
            else if(entry.movement==action_movement::REDUCE)
            {
                //归约-归约冲突
                //发生归约-归约冲突的时候默认采用书写在前面的产生式进行归约（也就是产生式编号比较小的那个）
                if(entry.index<old_entry.index)
                {
                    min_index=entry.index;
                    set_action_entry(LR1_index,ter_index,entry,true);
                }
                else
                {
                    min_index=old_entry.index;
                }
                if(print_warning)
                {
                    //打印警告信息
                    cout<<"Warning: Action ( state_index: "<<LR1_index<<" , terminator_index: "<<ter_index<<" ) Reduce-Reduce conflict!"<<endl;
                    cout<<"\tREDUCE "<<grammer_->productions_->at(old_entry.index)->production_string<<endl;
                    cout<<"\tREDUCE "<<grammer_->productions_->at(entry.index)->production_string<<endl;
                    cout<<"And we choose REDUCE "<<grammer_->productions_->at(min_index)->production_string<<" !"<<endl;
                    cout<<"Involved LR1 collection:"<<endl;
                    print_LR1_collection(LR1_index);
                    cout<<endl;
                }
                res=true;
            }
            else
            {
                if(print_warning)
                {
                    //打印错误信息
                print_action_conflict(LR1_index,ter_index,entry);
                }
            }
            break;
        case action_movement::ACCEPT:
            if(print_warning)
            {
                //打印错误信息
                print_action_conflict(LR1_index,ter_index,entry);
            }
            break;
        case action_movement::ERROR:
            if(print_warning)
            {
                //打印错误信息
                print_action_conflict(LR1_index,ter_index,entry);
            }
            break;
        default:
            break;
    }
    return res;
}

/*
打印action表的冲突信息

Paramters
---------
LR1_index:发生冲突的action表项的项目集编号(也就是状态编号)
ter_index:发生冲突的action表项的终结符编号
entry:想要赋值的新的表项
*/
void LR1_parser::print_action_conflict(state LR1_index,terminator_index ter_index,struct action_entry entry)
{
    set<struct LR1_item> * LR1_collection=canonical_LR1_collection_[LR1_index];

    struct action_entry old_entry=get_action_entry(LR1_index,ter_index);
    cout<<endl<<"Action ( state_index: "<<LR1_index<<" , terminator_index: "<<ter_index<<" ) conflict!"<<endl;

    //打印冲突的旧的action entry的信息
    cout<<"\tOld action entry:";
    switch (old_entry.movement)
    {
        case action_movement::SHIFT:
            cout<<"SHIFT "<<old_entry.index<<endl;
            break;
        case action_movement::REDUCE:
            cout<<"REDUCE "<<grammer_->productions_->at(old_entry.index)->production_string<<endl;
            break;
        case action_movement::ACCEPT:
            cout<<"ACCEPT"<<endl;
            break;
        case action_movement::ERROR:
            cout<<"ERROR"<<endl;
            break;
        default:
            break;
    }

    //打印冲突的新的action entry的信息
    cout<<"\tConflict action entry:";
    switch (entry.movement)
    {
        case  action_movement::SHIFT:
            cout<<"SHIFT "<<entry.index<<endl;
            break;
        case action_movement::REDUCE:
            cout<<"REDUCE "<<grammer_->productions_->at(entry.index)->production_string<<endl;
            break;
        case action_movement::ACCEPT:
            cout<<"ACCEPT"<<endl;
            break;
        case action_movement::ERROR:
            cout<<"ERROR"<<endl;
            break;
        default:
            break;
    }

    //打印对应的LR1状态集
    print_LR1_collection(LR1_index);
    cout<<endl;
    /*cout<<"=========================================="<<LR1_index<<"=========================================="<<endl;
    for(set<struct LR1_item>::iterator i=LR1_collection->begin();i!=LR1_collection->end();i++)
    {
        cout<<grammer_->productions_->at((*i).production)->production_string<<"\t\t\tdot_position:"<<(*i).dot_position<<"\t\t\tlookahead_symbol_index:"<<(*i).lookahead_symbol<<endl;
    }
    cout<<endl;*/
}

/*
打印goto表的冲突信息

Paramters
---------
LR1_index:发生冲突的goto表项的项目集编号(也就是状态编号)
ter_index:发生冲突的goto表项的终结符编号
entry:想要赋值的新的表项
*/
void LR1_parser::print_goto_conflict(state LR1_index,syntax_variable_index syn_var_index,goto_entry entry)
{
    //set<struct LR1_item> * LR1_collection=canonical_LR1_collection_[LR1_index];

    goto_entry old_entry=get_goto_entry(LR1_index,syn_var_index);
    cout<<endl<<"Action ( state_index: "<<LR1_index<<" , syntax_variable_index: "<<syn_var_index<<" ) conflict!"<<endl;

    //打印冲突的旧的goto entry的信息
    cout<<"\tOld goto entry: "<<old_entry<<endl;

    //打印冲突的新的goto entry的信息
    cout<<"\tConflict goto entry: "<<entry<<endl;

    //打印对应的LR1状态集
    print_LR1_collection(LR1_index);
    cout<<endl;
    /*cout<<"=========================================="<<LR1_index<<"=========================================="<<endl;
    for(set<struct LR1_item>::iterator i=LR1_collection->begin();i!=LR1_collection->end();i++)
    {
        cout<<grammer_->productions_->at((*i).production)->production_string<<"\t\t\tdot_position:"<<(*i).dot_position<<"\t\t\tlookahead_symbol_index:"<<(*i).lookahead_symbol<<endl;
    }
    cout<<endl;*/
}

/*
打印LR(1)项目集

Prarmeters
----------
LR1_index:要打印的LR(1)项目集
*/
void LR1_parser::print_LR1_collection(state LR1_index)
{
    set<struct LR1_item> * LR1_collection=canonical_LR1_collection_[LR1_index];

    cout<<"=========================================="<<LR1_index<<"=========================================="<<endl;
    for(set<struct LR1_item>::iterator i=LR1_collection->begin();i!=LR1_collection->end();i++)
    {
        cout<<grammer_->productions_->at((*i).production)->production_string<<"\t\t\tdot_position:"<<(*i).dot_position<<"\t\t\tlookahead_symbol_index:"<<(*i).lookahead_symbol<<endl;
    }
}

/*
构造LR(1)分析表

Return
------
如果构造出来的LR(1)分析表没有冲突的话就返回true，否则返回false
*/
bool LR1_parser::build_LR1_table(bool print_warning)
{
    set<struct LR1_item> * I_k;
    vector<struct production * > * productions=grammer_->productions_;
    struct production * production;
    syntax_symbol_index left,a;
    vector<syntax_symbol_index> right;
    unsigned int right_size;
    struct action_entry action_entry;

    //先把LR(1)项目集规范族建出来
    build_canonical_LR1_collection();

    //LR(1)分析表的状态个数
    state_num_=canonical_LR1_collection_.size();

    /*int num=0;
    cout<<"Num of LR1_item:"<<canonical_LR1_collection_.size()<<endl;
    for(vector<set<struct LR1_item> * >::iterator i=canonical_LR1_collection_.begin();i!=canonical_LR1_collection_.end();i++)
    {
        cout<<endl<<"===================="<<num<<"======================"<<endl;
        num++;
        
        //cout<<"num of LR1_item:"<<(*i)->size()<<endl;
        
        for(set<struct LR1_item>::iterator j=(*i)->begin();j!=(*i)->end();j++)
        {
            cout<<grammer_->productions_->at((*j).production)->production_string<<"\t\t\tdot_position:"<<(*j).dot_position<<"\t\t\tlookahead_symbol:"<<(*j).lookahead_symbol<<endl;
        }
    }*/

    //先对两个表进行初始化
    action_=(struct action_entry *)malloc(state_num_*grammer_->terminator_num_*sizeof(struct action_entry));
    goto_=(goto_entry *)malloc(state_num_*grammer_->syntax_variable_num_*sizeof(goto_entry));
    for(int i=0;i<state_num_;i++)
    {
        for(int j=0;j<grammer_->terminator_num_;j++)
        {
            action_[grammer_->terminator_num_*i+j]={action_movement::ERROR,-1};
        }
        for(int j=0;j<grammer_->syntax_variable_num_;j++)
        {
            goto_[grammer_->syntax_variable_num_*i+j]=-1;
        }
    }

    //然后再根据规则对其进行赋值
    for(state k=0;k<state_num_;k++)
    {
        I_k=canonical_LR1_collection_[k];

        //赋值action表
        for(set<struct LR1_item>::iterator i=I_k->begin();i!=I_k->end();i++)
        {
            production=productions->at((*i).production);
            left=production->left;
            right=production->right;
            right_size=sizeof_production_right(right);
            if((*i).dot_position<right_size)
            {
                a=right[(*i).dot_position];
                if(a<grammer_->terminator_num_ && canonical_LR1_go_table_.find(k)!=canonical_LR1_go_table_.end() && canonical_LR1_go_table_[k].find(a)!=canonical_LR1_go_table_[k].end())
                {
                    //cout<<k<<","<<a<<":"<<grammer_->productions_->at((*i).production)->production_string<<endl;
                    //cout<<canonical_LR1_go_table_[k][a]<<endl;
                    if(!set_action_entry(k,a,{action_movement::SHIFT,canonical_LR1_go_table_[k][a]},false))
                    {
                        //如果发生了冲突，那么就说明这个文法是具有歧义的，就尝试进行冲突处理
                        if(!handle_action_conflict(k,a,{action_movement::SHIFT,canonical_LR1_go_table_[k][a]},print_warning))
                        {
                            //cout<<"ACTION ERROR!!!! state:"<<k<<",transfer:"<<a<<endl;
                            //cout<<grammer_->productions_->at((*i).production)->production_string<<endl;
                            //cout<<canonical_LR1_go_table_[k][a]<<endl;
                            return false;
                        }
                    }
                }
            }
            else
            {
                //if(left==augmented_grammer_start_symbol_ && right_size==1 && right[0]==grammer_->start_symbol_)
                if(left==grammer_->start_symbol_ && right_size==1)
                {
                    //cout<<k<<",stack_bottom_symbol:"<<grammer_->productions_->at((*i).production)->production_string<<endl;
                    if(!set_action_entry(k,grammer_->stack_bottom_symbol_,{action_movement::ACCEPT,-1},false))
                    {
                        //cout<<"ACTION ERROR!!!! state:"<<k<<",transfer:stack_bottom_symbol"<<endl;
                        //如果发生了冲突，就打印冲突信息
                        print_action_conflict(k,grammer_->stack_bottom_symbol_,{action_movement::ACCEPT,-1});
                        return false;
                    }
                }
                else
                {
                    //cout<<k<<","<<(*i).lookahead_symbol<<":"<<grammer_->productions_->at((*i).production)->production_string<<endl;
                    if(!set_action_entry(k,(*i).lookahead_symbol,{action_movement::REDUCE,(*i).production},false))
                    {
                        //cout<<k<<","<<(*i).lookahead_symbol<<":"<<grammer_->productions_->at((*i).production)->production_string<<endl;
                        //cout<<"ACTION ERROR!!!! state:"<<k<<",transfer:"<<(*i).lookahead_symbol<<endl;
                        //如果发生了冲突，那么就说明这个文法是具有歧义的，就尝试进行冲突处理
                        if(!handle_action_conflict(k,(*i).lookahead_symbol,{action_movement::REDUCE,(*i).production},print_warning))
                        {
                            return false;
                        }
                    }
                }
            }
        }

        //赋值goto表
        if(canonical_LR1_go_table_.find(k)!=canonical_LR1_go_table_.end())
        {
            for(map<syntax_symbol_index,state>::iterator i=canonical_LR1_go_table_[k].begin();i!=canonical_LR1_go_table_[k].end();i++)
            {
                if((*i).first>=grammer_->terminator_num_)
                {
                    if(!set_goto_entry(k,(*i).first,(*i).second))
                    {
                        //如果goto表发生了冲突，就打印冲突信息
                        print_goto_conflict(k,(*i).first,(*i).second);
                        //cout<<"GOTO ERROR!!!! state:"<<k<<",transfer:"<<(*i).first<<endl;
                        return false;
                    }
                }
            }
        }
    }

    /*cout<<endl<<endl<<"action"<<endl<<"==================="<<endl;
    for(int i=0;i<state_num_*grammer_->terminator_num_;i++)
    {
        switch(action_[i].movement)
        {
            case action_movement::ERROR:
                cout<<"ERROR"<<endl;
                break;
            case action_movement::REDUCE:
                cout<<"REDUCE "<<action_[i].index<<endl;
                break;
            case action_movement::SHIFT:
                cout<<"SHIFT "<<action_[i].index<<endl;
                break;
            case action_movement::ACCEPT:
                cout<<"ACCEPT"<<endl;
                break;
            default:
                cout<<"??????????"<<endl;
                break;
        }
    }

    cout<<"goto"<<endl<<"==================="<<endl;
    for(int i=0;i<state_num_*grammer_->syntax_variable_num_;i++)
    {
        cout<<goto_[i]<<endl;
    }*/


    return true;

}

/*
从token的列表中获取下一个token

Return
------
返回指向下一个token的指针，如果没有下一个token就返回NULL
*/
struct token * LR1_parser::next_token()
{
    static list<struct token * >::iterator i=tokens_->begin();

    while(i!=tokens_->end())
    {
        return (*(i++));
    }

    return NULL;
}

/*
错误处理
*/
void LR1_parser::error_handle()
{

}

/*
赋值action表中的表项

Paramters
---------
LR1_index:项目集编号(也就是状态编号)
ter_index:终结符编号
entry:要赋值的表项
overwrite:如果发生冲突，是否进行覆盖

Return
------
如果此时对action表的赋值没有冲突就返回true，否则返回false
*/
bool LR1_parser::set_action_entry(state LR1_index,terminator_index ter_index,struct action_entry entry,bool overwrite)
{
    int length=grammer_->terminator_num_;
    if(action_[length*LR1_index+ter_index].movement==entry.movement && action_[length*LR1_index+ter_index].index==entry.index)
    {
        return true;
    }
    if(action_[length*LR1_index+ter_index].movement==action_movement::ERROR)
    {
        action_[length*LR1_index+ter_index]=entry;
        return true;
    }
    if(overwrite)
    {
        action_[length*LR1_index+ter_index]=entry;
    }
    return false;
}

/*
赋值goto表中的表项

Paramters
---------
LR1_index:项目集编号(也就是状态编号)
syn_var_index:语法变量编号
entry:要赋值的表项

Return
------
如果此时对goto表的赋值没有冲突就返回true，否则返回false
*/
bool LR1_parser::set_goto_entry(state LR1_index,syntax_variable_index syn_var_index,goto_entry entry)
{
    int y=syn_var_index-grammer_->terminator_num_;
    int length=grammer_->syntax_variable_num_;
    if(goto_[length*LR1_index+y]==-1)
    {
        goto_[length*LR1_index+y]=entry;
        return true;
    }
    //goto_[length*LR1_index+y]=entry;
    return false;
}

/*
从action表中获取表项

Parameters
----------
LR1_index:项目集编号(也就是状态编号)
ter_index:终结符编号
*/
struct action_entry LR1_parser::get_action_entry(state LR1_index,terminator_index ter_index)
{
    int length=grammer_->terminator_num_;
    return action_[length*LR1_index+ter_index];
}

/*
从goto表中获取表项

Parameters
----------
LR1_index:项目集编号(也就是状态编号)
syn_var_index:语法变量编号
*/
state LR1_parser::get_goto_entry(state LR1_index,syntax_variable_index syn_var_index)
{
    int length=grammer_->syntax_variable_num_;
    return goto_[length*LR1_index+syn_var_index-grammer_->terminator_num_];
}

/*
获取产生式右部的长度

Parameters
----------
right:产生式右部

Return
------
返回这个右部的长度，不包括空符号
*/
unsigned int LR1_parser::sizeof_production_right(vector<syntax_symbol_index> right)
{
    unsigned int result=0;
    for(vector<syntax_symbol_index>::iterator i=right.begin();i!=right.end();i++)
    {
        if((*i)!=EMPTY_SYMBOL)
        {
            result++;
        }
    }
    return result;
}

/*
语法分析

Return
------
返回产生式队列，如果出错就返回nullptr
*/
queue<struct production * > * LR1_parser::parse()
{
    int tag=-1;
    queue<struct production * > * result;
    struct action_entry entry;
    struct token * input_token=next_token();
    terminator_index input_token_code_of_kind=input_token?(terminator_index)input_token->code_of_kind:((tag==-1)?tag=grammer_->stack_bottom_symbol_:-1);
    syntax_symbol_index syntax_symbol;
    vector<struct production * > * productions=grammer_->productions_;
    struct production * production;
    state next_state;

    //先给符号栈和状态栈赋初始值
    token_stack_.push(grammer_->stack_bottom_symbol_);
    state_stack_.push(0);

    //然后开始按照action表和goto表进行语法分析
    if(input_token_code_of_kind==-1)
    {
        return nullptr;
    }
    result=new queue<struct production * >;
    while(true)
    {
        //cout<<"inptu_token:"<<input_token_code_of_kind<<endl;
        //cout<<"state:"<<state_stack_.top()<<endl;
        entry=get_action_entry(state_stack_.top(),input_token_code_of_kind);
        switch(entry.movement)
        {
            case action_movement::SHIFT:
            //cout<<"SHIFT"<<endl;
                token_stack_.push(input_token_code_of_kind);
                state_stack_.push(entry.index);
                input_token=next_token();
                input_token_code_of_kind=input_token?(terminator_index)input_token->code_of_kind:((tag==-1)?tag=grammer_->stack_bottom_symbol_:-1);
                if(input_token_code_of_kind==-1)
                {
                    delete result;
                    return nullptr;
                }
                break;
            case action_movement::REDUCE:
            //cout<<"REDUCE:";
                production=productions->at(entry.index);
                //cout<<production->production_string<<endl;
                for(vector<syntax_symbol_index>::iterator i=production->right.begin();i!=production->right.end();i++)
                {
                    if((*i)!=grammer_->empty_symbol_)
                    {
                        token_stack_.pop();
                        state_stack_.pop();
                    }
                }
                token_stack_.push(production->left);

                next_state=get_goto_entry(state_stack_.top(),production->left);
                
                if(next_state!=-1)
                {
                    state_stack_.push(next_state);
                }
                else
                {
                    delete result;
                    return nullptr;
                }

                result->push(production);
                break;
            case action_movement::ACCEPT:
            //cout<<"ACCEPT"<<endl;
                return result;
            case action_movement::ERROR:
            //cout<<"ERROR"<<endl;
                delete result;
                return nullptr;
        }
    }
}

/*
将语法分析得到的所有产生式输出到文件中

Parameters
----------
filename:语法分析得到的产生式序列要输出的文件名
*/
void LR1_parser::output_productions(const char * filename)
{
    //进行语法分析
    queue<struct production * > * productions=parse();
    if(!productions)
    {
        cout<<"Parse error!"<<endl;
        return;
    }

    //创建一个文件输出流对象
	ofstream outFile;
	//打开文件
	outFile.open(filename,ios::out);
	if (!outFile)
    {
        cout<<"Open file "<<filename<<" error!"<<endl;
        return;
	}

    //把产生式输出到文件中
    while(!productions->empty())
    {
        outFile<<productions->front()->production_string<<endl;
        productions->pop();
    }

    outFile.close();
}