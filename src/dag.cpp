/*
 *
 *  dag.cpp
 *  DAG生成器
 *  Creator:Sichao Chen
 *  Create time:2022/5/4
 *
*/
#include"dag.h"
#include<stdarg.h>
#include<algorithm>

//===================================== class DAG =====================================//

DAG_node::DAG_node(struct ic_data * data):related_op(ic_op::NOP)
{
    related_data=data;
}

DAG_node::DAG_node(enum ic_op op,struct ic_data * data,list<struct DAG_node * > children,void * special_data):related_op(op),children(children),special_data(special_data)
{
    for(auto child:children)
    {
        child->add_a_father(this);
    }
    related_data=data;
}

struct DAG_node * DAG_node::get_only_child()
{
    if(children.size()==1)
    {
        return children.front();
    }
    return nullptr;
}

struct DAG_node * DAG_node::get_left_child()
{
    if(children.size()==2)
    {
        return children.front();
    }
    return nullptr;
}

struct DAG_node * DAG_node::get_right_child()
{
    if(children.size()==2)
    {
        return children.back();
    }
    return nullptr;
}

void DAG_node::add_a_father(struct DAG_node * father)
{
    if(fathers.find(father)==fathers.end())
    {
        fathers.insert(make_pair(father,0));
    }
    fathers.at(father)++;
}

void DAG_node::add_a_child(struct DAG_node * child)
{
    children.push_back(child);
    child->add_a_father(this);
}

void DAG_node::delete_a_father(struct DAG_node * father)
{
    if(fathers.find(father)!=fathers.end())
    {
        if(fathers.at(father)==0 || fathers.at(father)==1)
        {
            fathers.erase(father);
        }
        else
        {
            fathers.at(father)--;
        }
    }
}

size_t DAG_node::get_fathers_num()
{
    size_t res=0;
    for(auto father:fathers)
    {
        res+=father.second;
    }
    return res;
}

void DAG_node::change_left_child(struct DAG_node * node)
{
    if(children.size()==2)
    {
        (*children.begin())->delete_a_father(this);
        (*children.begin())=node;
        if(node)
        {
            node->add_a_father(this);
        }
    }
}

void DAG_node::change_right_child(struct DAG_node * node)
{
    if(children.size()==2)
    {
        (*(++children.begin()))->delete_a_father(this);
        (*(++children.begin()))=node;
        if(node)
        {
            node->add_a_father(this);
        }
    }
}

void DAG_node::remove_all_children()
{
    for(auto child:children)
    {
        child->delete_a_father(this);
    }
    children.clear();
}

bool DAG_node::is_related_to_a_const(enum language_data_type data_type)
{
    return (related_data && related_data->is_const() && (data_type==language_data_type::VOID || related_data->get_data_type()==data_type));
}

bool DAG_node::is_related_to_a_temp_var()
{
    return (related_data && related_data->is_tmp_var());
}

//==========================================================================//



//===================================== class DAG =====================================//

DAG::DAG():basic_block_(nullptr)
{

}

DAG::DAG(struct ic_basic_block * basic_block):basic_block_(basic_block)
{
    struct DAG_node * current_node;
    enum ic_op op;
    struct ic_data * arg1,* arg2,* result;
    list<struct ic_data * > * r_params;
    struct ic_label * label;
    struct ic_func * func;
    bool find_ret=false;
    for(auto ic_with_info:basic_block->ic_sequence)
    {
        op=ic_with_info.intermediate_code.op;
        if(find_ret)
        {
            if(op==ic_op::END_FUNC_DEFINE)
            {
                func_end_=ic_with_info.intermediate_code;
            }
            else
            {
                continue;
            }
        }
        switch(op)
        {
            case ic_op::ASSIGN:
                arg1=(struct ic_data *)ic_with_info.intermediate_code.arg1.second;
                result=(struct ic_data *)ic_with_info.intermediate_code.result.second;
                generate_ASSIGN_in_DAG(result,arg1);
                break;
            case ic_op::ADD:
            case ic_op::SUB:
            case ic_op::MUL:
            case ic_op::DIV:
            case ic_op::MOD:
            case ic_op::EQ:
            case ic_op::UEQ:
            case ic_op::GE:
            case ic_op::GT:
            case ic_op::LE:
            case ic_op::LT:
                arg1=(struct ic_data *)ic_with_info.intermediate_code.arg1.second;
                arg2=(struct ic_data *)ic_with_info.intermediate_code.arg2.second;
                result=(struct ic_data *)ic_with_info.intermediate_code.result.second;
                //先尝试常量合并
                //再尝试代数恒等式的化简
                //最后尝试公共子表达式的删除
                //如果都失败了，那么就建立对应的DAG节点，并且在DAG节点之间建立联系
                constant_merge(op,result,arg1,arg2) || algebraic_simplify(op,result,arg1,arg2) || common_expression_delete(op,result,arg1,arg2) || build_DAG_node_and_relation(op,result,arg1,arg2);
                break;
            case ic_op::NOT:
                arg1=(struct ic_data *)ic_with_info.intermediate_code.arg1.second;
                result=(struct ic_data *)ic_with_info.intermediate_code.result.second;
                constant_merge(op,result,arg1) || algebraic_simplify(op,result,arg1) || common_expression_delete(op,result,arg1) || build_DAG_node_and_relation(op,result,arg1);
                break;
            case ic_op::CALL:
                result=(struct ic_data *)ic_with_info.intermediate_code.result.second;
                r_params=(list<struct ic_data * > *)ic_with_info.intermediate_code.arg2.second;
                func=(struct ic_func *)ic_with_info.intermediate_code.arg1.second;
                //尝试进行函数调用的公共子表达式删除，如果失败了，就建立对应的DAG节点，并且在DAG节点之间建立联系
                if(function_call_common_expression_delete(result,func,r_params)==false)
                {
                    generate_CALL_in_DAG(func,result,r_params);
                }
                delete r_params;
                break;
            case ic_op::JMP:
                jmp_ic_=ic_with_info.intermediate_code;
                break;
            case ic_op::IF_JMP:
            case ic_op::IF_NOT_JMP:
                arg1=(struct ic_data *)ic_with_info.intermediate_code.arg1.second;
                label=(struct ic_label *)ic_with_info.intermediate_code.result.second;
                generate_IF_JMP_or_IF_NOT_JMP_in_DAG(op,arg1,label);
                break;
            case ic_op::RET:
                generate_RET_in_DAG((struct ic_data *)ic_with_info.intermediate_code.result.second);
                find_ret=true;
                break;
            case ic_op::VAR_DEFINE:
                //默认把所有变量定义都放在基本块的开头
                var_defs_.push_back(ic_with_info.intermediate_code);
                break;
            case ic_op::LABEL_DEFINE:
                label_def_=ic_with_info.intermediate_code;
                break;
            case ic_op::FUNC_DEFINE:
                func_def_=ic_with_info.intermediate_code;
                break;
            case ic_op::END_FUNC_DEFINE:
                func_end_=ic_with_info.intermediate_code;
                break;
            default:
                break;
        }
    }
}

DAG::~DAG()
{
    for(auto node:all_nodes_)
    {
        delete node;
    }
}

void DAG::attach_data_to_node(struct ic_data * data,struct DAG_node * node)
{
    struct ic_data * offset,* array;
    bool offset_is_const;
    //如果该节点的关联的操作不是NOP，那么说明此时要对该数据进行赋值，那么就需要查看和这个数据相关的数组取元素，查看这些数组取元素是否需要变成无效
    if(node->related_op!=ic_op::NOP)
    {
        if(data->is_array_member() && !data->is_array_var())
        {
            offset=data->get_offset();
            offset_is_const=offset->is_const();
            array=data->get_belong_array();
            if(basic_block_->array_to_array_member_map.find(array)!=basic_block_->array_to_array_member_map.end())
            {
                for(auto array_member:basic_block_->array_to_array_member_map.at(array))
                {
                    if(!(offset_is_const && array_member->get_offset()->is_const() && offset->get_value().int_data!=array_member->get_offset()->get_value().int_data) && !array_member->is_array_var())
                    {
                        unattach_data_s_current_node(array_member);
                    }
                }
            }
        }
        if(basic_block_->offset_to_array_member_map.find(data)!=basic_block_->offset_to_array_member_map.end())
        {
            for(auto array_member:basic_block_->offset_to_array_member_map.at(data))
            {
                if(!array_member->is_array_var())
                {
                    unattach_data_s_current_node(array_member);   
                }
            }
        }
    }
    //在映射表data_to_node_建立变量和节点之间的关联
    if(data_to_node_.find(data)==data_to_node_.end())
    {
        data_to_node_.insert(make_pair(data,node));
    }
    else
    {
        data_to_node_.at(data)=node;
    }
}

void DAG::unattach_data_s_current_node(struct ic_data * data)
{
    //在映射表data_to_node_解除变量和节点之间的关联
    if(data_to_node_.find(data)!=data_to_node_.end())
    {
        data_to_node_.erase(data);
    }
}

void DAG::move_node_to_trash(struct DAG_node * node)
{
    trash_.insert(node);
}

struct DAG_node * DAG::new_DAG_node(struct ic_data * data)
{
    struct DAG_node * new_node=new struct DAG_node(data);
    //将该变量和新建立的节点建立关联
    attach_data_to_node(data,new_node);
    all_nodes_.insert(new_node);
    return new_node;
}

struct DAG_node * DAG::new_DAG_node(enum ic_op op,struct ic_data * data,size_t operands_num,...)
{
    struct DAG_node * new_node;
    struct ic_data * operand;
    //创建这个变量和操作符对应的节点，并将其加入节点表中
    list<struct DAG_node * > children_nodes;
    va_list argptr;
    va_start(argptr,operands_num);
    for(size_t i=0;i<operands_num;i++)
    {
        operand=va_arg(argptr,struct ic_data *);
        children_nodes.push_back(get_DAG_node(operand));
    }
    va_end(argptr);
    //对于数组取元素，需要将其偏移量进行复制传播
    if(data && data->is_array_member() && !data->is_array_var())
    {
        data=Symbol_table::get_instance()->array_member_not_array_var_entry(data->get_belong_array(),copy_progagation(data->get_offset(),true));
    }
    new_node=new struct DAG_node(op,data,children_nodes);
    if(data)
    {
        //将该变量和新建立的节点建立关联
        attach_data_to_node(data,new_node);
    }
    nodes_order_.push_back(new_node);
    all_nodes_.insert(new_node);
    return new_node;
}

struct DAG_node * DAG::new_DAG_node(enum ic_op op,struct ic_data * data,list<struct ic_data * > * operands,void * special_data)
{
    struct DAG_node * new_node;
    //创建这个变量和操作符对应的节点，并将其加入节点表中
    list<struct DAG_node * > children_nodes;
    for(auto operand:(*operands))
    {
        children_nodes.push_back(get_DAG_node(operand));
    }
    //对于数组取元素，需要将其偏移量进行复制传播
    if(data && data->is_array_member() && !data->is_array_var())
    {
        data=Symbol_table::get_instance()->array_member_not_array_var_entry(data->get_belong_array(),copy_progagation(data->get_offset(),true));
    }
    new_node=new struct DAG_node(op,data,children_nodes,special_data);
    if(data)
    {
        //将该变量和新建立的节点建立关联
        attach_data_to_node(data,new_node);
    }
    nodes_order_.push_back(new_node);
    all_nodes_.insert(new_node);
    return new_node;
}

struct DAG_node * DAG::get_DAG_node(struct ic_data * data)
{
    if(data_to_node_.find(data)==data_to_node_.end())
    {
        new_DAG_node(data);
    }
    return data_to_node_.at(data);
}

bool DAG::check_data_s_node_available(struct ic_data * data,struct DAG_node * node)
{
    return (data_to_node_.find(data)!=data_to_node_.end() && data_to_node_.at(data)==node);
}

struct ic_data * DAG::copy_progagation(struct ic_data * data,bool stop_when_array_member)
{
    //这里我们规定：在DAG中，ASSIGN节点如果不涉及数据类型的转换，那么它有且仅有一个子节点，而且这个子节点只能是叶子节点，不能再是ASSIGN节点或者其他的节点
    //如果ASSIGN涉及了数据类型的转换，那么它的子节点可以是叶子节点或者ASSIGN节点
    struct DAG_node * node,* child;
    struct ic_data * offset;
    if(data->is_array_member())
    {
        if(data->is_array_var())
        {
            return data;
        }
        //如果要复制传播的变量是一个数组取元素的话，那么就先对其偏移量进行复制传播
        offset=copy_progagation(data->get_offset(),true);
        data=Symbol_table::get_instance()->array_member_not_array_var_entry(data->get_belong_array(),offset);
    }
    node=get_DAG_node(data);
    while(node->related_op==ic_op::ASSIGN)
    {
        child=node->get_only_child();
        if(child->related_data->get_data_type()!=data->get_data_type() || 
        !check_data_s_node_available(child->related_data,child) || 
        (stop_when_array_member && child->related_data->is_array_member()))
        {
            break;
        }
        node=child;
    }
    return node->related_data;
}

void DAG::generate_ASSIGN_in_DAG(struct ic_data * to,struct ic_data * from)
{
    new_DAG_node(ic_op::ASSIGN,to,1,copy_progagation(from));
}

void DAG::generate_IF_JMP_or_IF_NOT_JMP_in_DAG(enum ic_op op,struct ic_data * arg,struct ic_label * label)
{
    list<struct ic_data * > operands;
    operands.push_back(arg);
    new_DAG_node(op,nullptr,&operands,(void *)label);
    operands.clear();
}

void DAG::generate_CALL_in_DAG(struct ic_func * func,struct ic_data * ret,list<struct ic_data * > * r_params)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    size_t f_param_pos=0;
    set<struct ic_data * > func_def_globals_and_f_params;
    struct ic_data * array;
    for(list<struct ic_data * >::iterator r_param=r_params->begin();r_param!=r_params->end();r_param++)
    {
        (*r_param)=copy_progagation(*r_param);
    }
    new_DAG_node(ic_op::CALL,ret,r_params,(void *)func);
    //还需要查看调用的这个函数可能会改变的
    //全局变量
    //数组形参对应的实参
    //和上述两类数据相关的数据
    //将这三类可能会被改变的数据和它们对应的DAG节点解除关联
    func_def_globals_and_f_params=symbol_table->get_func_def_globals_and_array_f_params(func);
    for(auto data:func_def_globals_and_f_params)
    {
        if(data->is_array_var())
        {
            if(data->is_f_param())
            {
                for(auto f_param:(*func->f_params))
                {
                    if(data==f_param)
                    {
                        break;
                    }
                    f_param_pos++;
                }
                for(auto r_param:(*r_params))
                {
                    if(f_param_pos==0)
                    {
                        data=r_param;
                        break;
                    }
                    f_param_pos--;
                }
            }
            if(data->is_array_member())
            {
                array=data->get_belong_array();
            }
            else
            {
                array=data;
            }
            if(basic_block_->array_to_array_member_map.find(array)!=basic_block_->array_to_array_member_map.end())
            {
                for(auto array_member:basic_block_->array_to_array_member_map.at(array))
                {
                    if(!array_member->is_array_var())
                    {
                        unattach_data_s_current_node(array_member);
                    }
                }
            }
        }
        else if(data->is_global())
        {
            if(basic_block_->offset_to_array_member_map.find(data)!=basic_block_->offset_to_array_member_map.end())
            {
                for(auto array_member:basic_block_->offset_to_array_member_map.at(data))
                {
                    if(!array_member->is_array_var())
                    {
                        unattach_data_s_current_node(array_member);
                    }
                }
            }
            unattach_data_s_current_node(data);
        }
    }
}

void DAG::generate_RET_in_DAG(struct ic_data * ret_value)
{
    if(ret_value)
    {
        ret_value=copy_progagation(ret_value);
        new_DAG_node(ic_op::RET,nullptr,1,ret_value);
    }
    else
    {
        new_DAG_node(ic_op::RET,nullptr,(size_t)0);
    }
}

bool DAG::constant_merge(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2)
{
    struct ic_data * new_arg;
    arg1=copy_progagation(arg1);
    //如果经过复制传播之后发现两个操作数（或者一个操作数）都变成了常数，那么就可以进行常量合并，把原本的运算变成一次简单的常量赋值即可
    if(arg2)
    {
        arg2=copy_progagation(arg2);
        new_arg=binary_compute(op,arg1,arg2);
    }
    else
    {
        new_arg=unary_compute(op,arg1);
    }
    if(new_arg)
    {
        generate_ASSIGN_in_DAG(result,new_arg);
        return true;
    }
    return false;
}

bool DAG::algebraic_simplify(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct ic_data * new_arg;
    struct DAG_node * new_arg_s_DAG_node;
    arg1=copy_progagation(arg1);
    if(arg2)
    {
        arg2=copy_progagation(arg2);
    }
    if(op==ic_op::ADD && arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.f)))
    {
        //x=0+x
        generate_ASSIGN_in_DAG(result,arg2);
        return true;
    }
    else if((op==ic_op::ADD || op==ic_op::SUB) && arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.f)))
    {
        //x=x+0
        //x=x-0
        generate_ASSIGN_in_DAG(result,arg1);
        return true;
    }
    else if(op==ic_op::MUL && arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==1) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==1.f)))
    {
        //x=1*x
        generate_ASSIGN_in_DAG(result,arg2);
        return true;
    }
    else if((op==ic_op::MUL || op==ic_op::DIV) && arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.f)))
    {
        //x=x*1
        //x=x/1
        generate_ASSIGN_in_DAG(result,arg1);
        return true;
    }
    else if(op==ic_op::MOD && arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==1) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==1.f)))
    {
        //x=x%1
        generate_ASSIGN_in_DAG(result,arg1);
        return true;
    }
    else if(op==ic_op::MUL && ((arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.f))) && (arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.f))))) 
    {
        //0=0*x
        //0=x*0
        if(result->get_data_type()==language_data_type::INT)
        {
            generate_ASSIGN_in_DAG(result,symbol_table->const_entry(result->get_data_type(),OAA((int)0)));
        }
        else if(result->get_data_type()==language_data_type::FLOAT)
        {
            generate_ASSIGN_in_DAG(result,symbol_table->const_entry(result->get_data_type(),OAA((float)0.0f)));
        }
        return true;
    }
    else if(op==ic_op::EQ && arg1==arg2)
    {
        //(x==x)=1
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)1)));
        return true;
    }
    else if(op==ic_op::UEQ && arg1==arg2)
    {
        //(x!=x)=0
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)0)));
        return true;
    }
    else if(op==ic_op::GE && arg1==arg2)
    {
        //(x>=x)=1
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)1)));
    }
    else if(op==ic_op::GT && arg1==arg2)
    {
        //(x>x)=0
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)0)));
        return true;
    }
    else if(op==ic_op::LE && arg1==arg2)
    {
        //(x>=x)=1
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)1)));
        return true;
    }
    else if(op==ic_op::LT && arg1==arg2)
    {
        //(x>x)=0
        generate_ASSIGN_in_DAG(result,symbol_table->const_entry(language_data_type::INT,OAA((int)0)));
        return true;
    }
    return false;
}

bool DAG::common_expression_delete(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2)
{
    set<struct DAG_node * > common_fathers;
    struct DAG_node * arg1_node,* arg2_node,* result_node;
    arg1=copy_progagation(arg1);
    arg1_node=get_DAG_node(arg1);
    if(arg2)
    {
        arg2=copy_progagation(arg2);
        arg2_node=get_DAG_node(arg2);
        //查看两个操作数经过复制传播之后是否具有公共子表达式
        common_fathers=map_key_intersection(arg1_node->fathers,arg2_node->fathers);
        for(auto father:common_fathers)
        {
            if(op==father->related_op && 
            ((father->get_left_child()==arg1_node && father->get_right_child()==arg2_node) || 
            (father->get_left_child()==arg2_node && father->get_right_child()==arg1_node)) && 
            check_data_s_node_available(father->related_data,father))
            {
                generate_ASSIGN_in_DAG(result,father->related_data);
                return true;
            }
        }
    }
    else
    {
        for(auto father:arg1_node->fathers)
        {
            if(op==father.first->related_op && 
            check_data_s_node_available(father.first->related_data,father.first))
            {
                generate_ASSIGN_in_DAG(result,father.first->related_data);
                return true;
            }
        }
    }
    return false;
}

bool DAG::function_call_common_expression_delete(struct ic_data * result,struct ic_func * func,list<struct ic_data * > * r_params)
{
    Symbol_table * symbol_table=Symbol_table::get_instance();
    set<struct DAG_node * > common_fathers;
    list<struct DAG_node * > r_params_DAG_nodes;
    //对于这几类函数调用是不能进行公共子表达式的删除的：
    //（1）外部函数
    //（2）调用的函数对全局变量或者函数的数组形参进行了更改或者使用
    //（3）直接或者间接调用了外部函数的函数
    if(func->type==func_type::LIBRARY || 
    !symbol_table->get_func_def_globals_and_array_f_params(func).empty() || 
    !symbol_table->get_func_use_globals_and_array_f_params(func).empty() || 
    symbol_table->is_func_call_external_func(func))
    {
        return false;
    }
    if(!result)
    {
        return true;
    }
    if(r_params)
    {
        for(auto r_param:(*r_params))
        {
            r_params_DAG_nodes.push_back(get_DAG_node(copy_progagation(r_param)));
        }
    }
    for(auto node:all_nodes_)
    {
        if(node->related_op==ic_op::CALL && node->special_data==func && node->related_data)
        {
            common_fathers.insert(node);
        }
    }
    for(auto r_param_DAG_node:r_params_DAG_nodes)
    {
        common_fathers=map_key_and_set_intersection(r_param_DAG_node->fathers,common_fathers);
    }
    for(auto father:common_fathers)
    {
        if(father->related_op==ic_op::CALL && 
        father->special_data==func && 
        check_data_s_node_available(father->related_data,father))
        {
            generate_ASSIGN_in_DAG(result,father->related_data);
            return true;
        }
    }
    return false;
}

bool DAG::build_DAG_node_and_relation(enum ic_op op,struct ic_data * result,struct ic_data * arg1,struct ic_data * arg2)
{
    arg1=copy_progagation(arg1);
    if(arg2)
    {
        arg2=copy_progagation(arg2);
        new_DAG_node(op,result,2,arg1,arg2);
    }
    else
    {
        new_DAG_node(op,result,1,arg1);
    }
    return true;
}

void DAG::a_lot_of_adds_to_multi_in_a_DAG_tree(struct DAG_node * father_node)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct DAG_node * left_child,* right_child,* left_child_s_left_child,* left_child_s_right_child;
    if(father_node->related_op==ic_op::ADD)
    {
        left_child=father_node->get_left_child();
        right_child=father_node->get_right_child();
        a_lot_of_adds_to_multi_in_a_DAG_tree(left_child);
        a_lot_of_adds_to_multi_in_a_DAG_tree(right_child);
        if(left_child==right_child)
        {
            father_node->change_right_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)2))));
            father_node->related_op=ic_op::MUL;
            return;
        }
        switch(left_child->related_op)
        {
            case ic_op::MUL:
                left_child_s_left_child=left_child->get_left_child();
                left_child_s_right_child=left_child->get_right_child();
                if(left_child_s_left_child->is_related_to_a_const(language_data_type::INT))
                {
                    data_exchange(left_child_s_left_child,left_child_s_right_child);
                }
                if(left_child_s_right_child->is_related_to_a_const(language_data_type::INT) && 
                left_child_s_left_child==right_child && 
                left_child->is_related_to_a_temp_var() && 
                left_child->get_fathers_num()==1)
                {
                    father_node->change_left_child(left_child_s_left_child);
                    father_node->change_right_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)(left_child_s_right_child->related_data)->get_value().int_data+1))));
                    father_node->related_op=ic_op::MUL;
                }
                break;
            default:
                return;
                break;
        }
    }
}

void DAG::a_lot_of_adds_to_multi()
{
    for(auto father_node:nodes_order_)
    {
        a_lot_of_adds_to_multi_in_a_DAG_tree(father_node);
    }
}

void DAG::n_selfadd_one_to_one_selfadd_n_in_a_DAG_tree(struct DAG_node * father_node)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct DAG_node * left_child,* right_child,* left_child_s_left_child,* left_child_s_right_child;
    if(father_node->related_op==ic_op::ADD)
    {
        left_child=father_node->get_left_child();
        right_child=father_node->get_right_child();
        a_lot_of_adds_to_multi_in_a_DAG_tree(left_child);
        a_lot_of_adds_to_multi_in_a_DAG_tree(right_child);
        if(left_child->is_related_to_a_const(language_data_type::INT))
        {
            data_exchange(left_child,right_child);
        }
        if(!right_child->is_related_to_a_const(language_data_type::INT) || 
        left_child->fathers.size()!=1 || 
        left_child->related_data!=father_node->related_data || 
        left_child->related_op!=ic_op::ADD)
        {
            return;
        }
        left_child_s_left_child=left_child->get_left_child();
        left_child_s_right_child=left_child->get_right_child();
        if(left_child_s_left_child->is_related_to_a_const(language_data_type::INT))
        {
            data_exchange(left_child_s_left_child,left_child_s_right_child);
        }
        if(!left_child_s_right_child->is_related_to_a_const(language_data_type::INT))
        {
            return;
        }
        father_node->change_left_child(left_child_s_left_child);
        father_node->change_right_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)(left_child_s_right_child->related_data)->get_value().int_data+right_child->related_data->get_value().int_data))));
        left_child->change_left_child(nullptr);
        left_child->change_right_child(nullptr);
        move_node_to_trash(left_child);
    }
}

void DAG::n_selfadd_one_to_one_selfadd_n()
{
    for(auto father_node:nodes_order_)
    {
        n_selfadd_one_to_one_selfadd_n_in_a_DAG_tree(father_node);
    }
}

void DAG::algebraic_simplification_in_DAG_tree(struct DAG_node * father_node)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct DAG_node * left_child,* right_child,* left_child_s_left_child,* left_child_s_right_child;
    bool tag;
    switch(father_node->related_op)
    {
        case ic_op::ADD:
            left_child=father_node->get_left_child();
            right_child=father_node->get_right_child();
            algebraic_simplification_in_DAG_tree(left_child);
            algebraic_simplification_in_DAG_tree(right_child);
            if(left_child->related_op==ic_op::SUB && left_child->get_right_child()==right_child/* && 
            left_child->related_data!=left_child->get_left_child()->related_data && 
            left_child->related_data!=left_child->get_right_child()->related_data*/)
            {
                //(a-b)+b=a
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==left_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==left_child->get_left_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                father_node->related_op=ic_op::ASSIGN;
                father_node->remove_all_children();
                father_node->add_a_child(left_child->get_left_child());
            }
            else if(right_child->related_op==ic_op::SUB && right_child->get_right_child()==left_child/* && 
            right_child->related_data!=right_child->get_left_child()->related_data && 
            right_child->related_data!=right_child->get_right_child()->related_data*/)
            {
                //b+(a-b)=a
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==right_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==right_child->get_left_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                father_node->related_op=ic_op::ASSIGN;
                father_node->remove_all_children();
                father_node->add_a_child(right_child->get_left_child());
            }
            break;
        case ic_op::SUB:
            left_child=father_node->get_left_child();
            right_child=father_node->get_right_child();
            algebraic_simplification_in_DAG_tree(left_child);
            algebraic_simplification_in_DAG_tree(right_child);
            if(left_child->related_op==ic_op::ADD && left_child->get_left_child()==right_child/* && 
            left_child->related_data!=left_child->get_left_child()->related_data && 
            left_child->related_data!=left_child->get_right_child()->related_data*/)
            {
                //(a+b)-a=b
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==left_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==left_child->get_right_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                father_node->related_op=ic_op::ASSIGN;
                father_node->remove_all_children();
                father_node->add_a_child(left_child->get_right_child());
            }
            else if(left_child->related_op==ic_op::ADD && left_child->get_right_child()==right_child/* && 
            left_child->related_data!=left_child->get_left_child()->related_data && 
            left_child->related_data!=left_child->get_right_child()->related_data*/)
            {
                //(a+b)-b=a
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==left_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==left_child->get_left_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                father_node->related_op=ic_op::ASSIGN;
                father_node->remove_all_children();
                father_node->add_a_child(left_child->get_left_child());
            }
            else if(left_child->related_op==ic_op::SUB && left_child->get_left_child()==right_child/* && 
            left_child->related_data!=left_child->get_left_child()->related_data && 
            left_child->related_data!=left_child->get_right_child()->related_data*/)
            {
                //(a-b)-a=-b
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==left_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==left_child->get_right_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                if(father_node->related_data->get_data_type()==language_data_type::INT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)0))));
                }
                else if(father_node->related_data->get_data_type()==language_data_type::FLOAT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((float)0.f))));
                }
                father_node->change_right_child(left_child->get_right_child());
            }
            else if(right_child->related_op==ic_op::ADD && right_child->get_left_child()==left_child/* && 
            right_child->related_data!=right_child->get_left_child()->related_data && 
            right_child->related_data!=right_child->get_right_child()->related_data*/)
            {
                //a-(a+b)=-b
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==right_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==right_child->get_right_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                if(father_node->related_data->get_data_type()==language_data_type::INT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)0))));
                }
                else if(father_node->related_data->get_data_type()==language_data_type::FLOAT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((float)0.f))));
                }
                father_node->change_right_child(right_child->get_right_child());
            }
            else if(right_child->related_op==ic_op::ADD && right_child->get_right_child()==left_child/* && 
            right_child->related_data!=right_child->get_left_child()->related_data && 
            right_child->related_data!=right_child->get_right_child()->related_data*/)
            {
                //b-(a+b)=-a
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==right_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==right_child->get_left_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                if(father_node->related_data->get_data_type()==language_data_type::INT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((int)0))));
                }
                else if(father_node->related_data->get_data_type()==language_data_type::FLOAT)
                {
                    father_node->change_left_child(get_DAG_node(symbol_table->const_entry(language_data_type::INT,OAA((float)0.f))));
                }
                father_node->change_right_child(right_child->get_left_child());
            }
            else if(right_child->related_op==ic_op::SUB && right_child->get_left_child()==left_child/* && 
            right_child->related_data!=right_child->get_left_child()->related_data && 
            right_child->related_data!=right_child->get_right_child()->related_data*/)
            {
                //a-(a-b)=b
                tag=false;
                for(auto node_in_order:nodes_order_)
                {
                    if(node_in_order==right_child)
                    {
                        tag=true;
                    }
                    if(tag)
                    {
                        if(node_in_order==father_node)
                        {
                            break;
                        }
                        else if(node_in_order->related_data==right_child->get_right_child()->related_data)
                        {
                            return;
                        }
                    }
                }
                father_node->related_op=ic_op::ASSIGN;
                father_node->remove_all_children();
                father_node->add_a_child(right_child->get_right_child());
            }
            break;
        default:
            break;
    }
}

void DAG::algebraic_simplification()
{
    for(auto father_node:nodes_order_)
    {
        algebraic_simplification_in_DAG_tree(father_node);
    }
}

void DAG::optimize()
{
    //将多个加法转换成乘法
    a_lot_of_adds_to_multi();
    //n个+1转换成1个+n
    n_selfadd_one_to_one_selfadd_n();
    //代数化简
    algebraic_simplification();
}

list<struct quaternion> DAG::to_basic_block()
{
    list<struct quaternion> res;
    enum ic_op op;
    struct ic_data * result,* arg1,* arg2;
    list<struct ic_data * > * r_params;
    if(func_def_.op!=ic_op::NOP)
    {
        res.push_back(func_def_);
    }
    if(label_def_.op!=ic_op::NOP)
    {
        res.push_back(label_def_);
    }
    for(auto var_def:var_defs_)
    {
        res.push_back(var_def);
    }
    for(auto node:nodes_order_)
    {
        if(trash_.find(node)!=trash_.end())
        {
            continue;
        }
        op=node->related_op;
        //按照最初的顺序将这些DAG节点还原成中间代码
        switch(op)
        {
            case ic_op::ASSIGN:
                arg1=node->get_only_child()->related_data;
                result=node->related_data;
                res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(result)));
                break;
            case ic_op::NOT:
                arg1=node->get_only_child()->related_data;
                result=node->related_data;
                res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(result)));
                break;
            case ic_op::ADD:
            case ic_op::SUB:
            case ic_op::MUL:
            case ic_op::DIV:
            case ic_op::MOD:
            case ic_op::EQ:
            case ic_op::UEQ:
            case ic_op::GE:
            case ic_op::GT:
            case ic_op::LE:
            case ic_op::LT:
                arg1=node->get_left_child()->related_data;
                arg2=node->get_right_child()->related_data;
                result=node->related_data;
                res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::DATA,(void *)(arg2),ic_operand::DATA,(void *)(result)));
                break;
            case ic_op::CALL:
                result=node->related_data;
                r_params=new list<struct ic_data * >;
                for(auto child:node->children)
                {
                    r_params->push_back(child->related_data);
                }
                if(result)
                {
                    res.push_back(quaternion(op,ic_operand::FUNC,(void *)(node->special_data),ic_operand::DATAS,(void *)(r_params),ic_operand::DATA,(void *)(result)));
                }
                else
                {
                    res.push_back(quaternion(op,ic_operand::FUNC,(void *)(node->special_data),ic_operand::DATAS,(void *)(r_params),ic_operand::NONE,nullptr));
                }
                break;
            case ic_op::IF_JMP:
            case ic_op::IF_NOT_JMP:
                arg1=node->get_only_child()->related_data;
                if(arg1->is_const())
                {
                    switch(arg1->get_data_type())
                    {
                        case language_data_type::INT:
                            if((arg1->get_value().int_data==0 && op==ic_op::IF_NOT_JMP) || (arg1->get_value().int_data!=0 && op==ic_op::IF_JMP))
                            {
                                res.push_back(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,node->special_data));
                            }
                            break;
                        case language_data_type::FLOAT:
                            if((arg1->get_value().float_data==0.f && op==ic_op::IF_NOT_JMP) || (arg1->get_value().float_data!=0.f && op==ic_op::IF_JMP))
                            {
                                res.push_back(quaternion(ic_op::JMP,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::LABEL,node->special_data));
                            }
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::NONE,nullptr,ic_operand::LABEL,node->special_data));
                }
                break;
            case ic_op::RET:
                if(node->children.size()==1)
                {
                    result=node->get_only_child()->related_data;
                    res.push_back(quaternion(op,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(result)));
                }
                else
                {
                    res.push_back(quaternion(op,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr));
                }
                break;
            default:
                break;
        }
    }
    if(jmp_ic_.op!=ic_op::NOP)
    {
        res.push_back(jmp_ic_);
    }
    if(func_end_.op!=ic_op::NOP)
    {
        res.push_back(func_end_);
    }
    return res;
}

//==========================================================================//
