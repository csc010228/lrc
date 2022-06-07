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
    add_data(data);
}

DAG_node::DAG_node(enum ic_op op,struct ic_data * data,list<struct DAG_node * > children,void * special_data):related_op(op),children(children),special_data(special_data)
{
    for(auto child:children)
    {
        child->fathers.insert(this);
    }
    if(data)
    {
        add_data(data);
    }
}

void DAG_node::add_data(struct ic_data * data)
{
    for(auto related_data:related_datas)
    {
        if(data==related_data)
        {
            return;
        }
    }
    related_datas.push_back(data);
}

bool DAG_node::is_leaf()
{
    return (related_op==ic_op::NOP && related_datas.size()==1 && children.empty());
}

struct ic_data * DAG_node::get_leaf_node_s_only_data()
{
    if(is_leaf() && !related_datas.empty())
    {
        return related_datas.front();
    }
    return nullptr;
}

struct ic_data * DAG_node::get_first_data()
{
    if(related_datas.empty())
    {
        return nullptr;
    }
    return related_datas.front();
}

struct DAG_node * DAG_node::get_only_child()
{
    if(children.size()==1)
    {
        for(auto child:children)
        {
            return child;
        }
    }
    return nullptr;
}

struct DAG_node * DAG_node::get_left_child()
{
    if(children.size()==2)
    {
        return (*children.begin());
    }
    return nullptr;
}

struct DAG_node * DAG_node::get_right_child()
{
    if(children.size()==2)
    {
        return (*(++children.begin()));
    }
    return nullptr;
}

enum language_data_type DAG_node::get_related_data_type()
{
    struct ic_data * data=get_first_data();
    if(data)
    {
        return data->get_data_type();
    }
    return language_data_type::VOID;
}

//==========================================================================//



//===================================== class DAG =====================================//

DAG::DAG()
{

}

DAG::DAG(struct ic_basic_block * basic_block)
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
                generate_CALL_in_DAG(func,result,r_params);
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
            if(array_to_array_member_map_.find(array)!=array_to_array_member_map_.end())
            {
                for(auto array_member:array_to_array_member_map_.at(array))
                {
                    if(!(offset_is_const && array_member->get_offset()->is_const() && offset->get_value().int_data!=array_member->get_offset()->get_value().int_data))
                    {
                        unattach_data_s_current_node(array_member);
                    }
                }
            }
        }
        if(offset_to_array_member_map_.find(data)!=offset_to_array_member_map_.end())
        {
            for(auto array_member:offset_to_array_member_map_.at(data))
            {
                unattach_data_s_current_node(array_member);
            }
        }
    }
    //如果DAG节点中没有这个变量，就将其添加进去
    // if(node->related_datas.find(data)==node->related_datas.end())
    // {
    //     node->related_datas.insert(data);
    // }
    node->add_data(data);
    //在映射表data_to_node_建立变量和节点之间的关联
    if(data_to_node_.find(data)==data_to_node_.end())
    {
        data_to_node_.insert(make_pair(data,node));
    }
    else
    {
        data_to_node_.at(data)=node;
    }
    //如果该变量是数组元素，那么就记录这个数组取元素的偏移量和所属数组信息
    if(data->is_array_member() && !data->is_array_var())
    {
        if(array_to_array_member_map_.find(data->get_belong_array())==array_to_array_member_map_.end())
        {
            array_to_array_member_map_.insert(make_pair(data->get_belong_array(),set<struct ic_data * >()));
        }
        if(array_to_array_member_map_.at(data->get_belong_array()).find(data)==array_to_array_member_map_.at(data->get_belong_array()).end())
        {
            array_to_array_member_map_.at(data->get_belong_array()).insert(data);
        }
        if(offset_to_array_member_map_.find(data->get_offset())==offset_to_array_member_map_.end())
        {
            offset_to_array_member_map_.insert(make_pair(data->get_offset(),set<struct ic_data * >()));
        }
        if(offset_to_array_member_map_.at(data->get_offset()).find(data)==offset_to_array_member_map_.at(data->get_offset()).end())
        {
            offset_to_array_member_map_.at(data->get_offset()).insert(data);
        }
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

bool DAG::check_leaf_node_available(struct DAG_node * node)
{
    return node->is_leaf() && check_data_s_node_available(node->get_leaf_node_s_only_data(),node);
}

struct ic_data * DAG::copy_progagation(struct ic_data * data,bool stop_when_array_member)
{
    //这里我们规定：在DAG中，ASSIGN节点如果不涉及数据类型的转换，那么它有且仅有一个子节点，而且这个子节点只能是叶子节点，不能再是ASSIGN节点或者其他的节点
    //如果ASSIGN涉及了数据类型的转换，那么它的子节点可以是叶子节点或者ASSIGN节点
    struct DAG_node * node,* child;
    struct ic_data * res,* offset;
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
    if(node->related_op==ic_op::ASSIGN)
    {
        child=node->get_only_child();
        if(child && child->get_first_data()->get_data_type()==data->get_data_type())
        {
            for(auto related_data:child->related_datas)
            {
                if(check_data_s_node_available(related_data,child) && (!(stop_when_array_member && related_data->is_array_member())))
                {
                    return related_data;
                }
            }
        }
        // if(child && check_leaf_node_available(child) && child->get_first_data()->get_data_type()==data->get_data_type())
        // {
        //     res=child->get_leaf_node_s_only_data();
        //     if(!(stop_when_array_member && res->is_array_member()))
        //     {
        //         return res;
        //     }
        // }
    }
    return data;
}

void DAG::generate_ASSIGN_in_DAG(struct ic_data * to,struct ic_data * from)
{
    // struct DAG_node * from_s_DAG_node;
    // from=copy_progagation(from);
    // from_s_DAG_node=get_DAG_node(from);
    // for(auto father:from_s_DAG_node->fathers)
    // {
    //     if(father->related_op==ic_op::ASSIGN && father->get_related_data_type()==to->get_data_type())
    //     {
    //         attach_data_to_node(to,father);
    //         return;
    //     }
    // }
    // new_DAG_node(ic_op::ASSIGN,to,1,from);
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
    Symbol_table * symbol_table=Symbol_table::get_instance();
    size_t f_param_pos=0;
    set<struct ic_data * > func_def_global_vars_and_array_f_params;
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
    func_def_global_vars_and_array_f_params=symbol_table->get_func_def_global_vars_and_array_f_params(func);
    for(auto data:func_def_global_vars_and_array_f_params)
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
            if(array_to_array_member_map_.find(array)!=array_to_array_member_map_.end())
            {
                for(auto array_member:array_to_array_member_map_.at(array))
                {
                    unattach_data_s_current_node(array_member);
                }
            }
        }
        else
        {
            if(offset_to_array_member_map_.find(data)!=offset_to_array_member_map_.end())
            {
                for(auto array_member:offset_to_array_member_map_.at(data))
                {
                    unattach_data_s_current_node(array_member);
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
    Symbol_table * symbol_table=Symbol_table::get_instance();
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
    else if(op==ic_op::MUL && arg1->is_const() && ((arg1->get_data_type()==language_data_type::INT && arg1->get_value().int_data==0) || (arg1->get_data_type()==language_data_type::FLOAT && arg1->get_value().float_data==0.f)))
    {
        //x=0*x
        generate_ASSIGN_in_DAG(result,arg2);
        return true;
    }
    else if(op==ic_op::MUL && arg2->is_const() && ((arg2->get_data_type()==language_data_type::INT && arg2->get_value().int_data==0) || (arg2->get_data_type()==language_data_type::FLOAT && arg2->get_value().float_data==0.f)))
    {
        //x=x*0
        generate_ASSIGN_in_DAG(result,arg1);
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
    struct DAG_node * arg1_node,* arg2_node;
    arg1=copy_progagation(arg1);
    arg1_node=get_DAG_node(arg1);
    if(arg2)
    {
        arg2=copy_progagation(arg2);
        arg2_node=get_DAG_node(arg2);
        //查看两个操作数经过复制传播之后是否具有公共子表达式
        set_intersection(arg1_node->fathers.begin(),arg1_node->fathers.end(),arg2_node->fathers.begin(),arg2_node->fathers.end(),inserter(common_fathers,common_fathers.begin()));
        for(auto father:common_fathers)
        {
            if(op==father->related_op && (father->get_left_child()==arg1_node && father->get_right_child()==arg2_node) || (father->get_left_child()==arg2_node && father->get_right_child()==arg1_node))
            {
                attach_data_to_node(result,father);
                return true;
            }
        }
    }
    else
    {
        for(auto father:arg1_node->fathers)
        {
            if(op==father->related_op)
            {
                attach_data_to_node(result,father);
                return true;
            }
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

void DAG::optimize()
{
    
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
        op=node->related_op;
        //按照最初的顺序将这些DAG节点还原成中间代码
        switch(op)
        {
            case ic_op::ASSIGN:
                result=nullptr;
                for(auto data:node->related_datas)
                {
                    if(!result)
                    {
                        arg1=node->get_only_child()->get_first_data();
                        if(arg1->get_data_type()==data->get_data_type())
                        {
                            result=arg1;
                        }
                        else
                        {
                            result=data;
                        }
                        res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(data)));
                    }
                    else
                    {
                        res.push_back(quaternion(ic_op::ASSIGN,ic_operand::DATA,(void *)(result),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(data)));
                    }
                }
                break;
            case ic_op::NOT:
                result=nullptr;
                for(auto data:node->related_datas)
                {
                    if(!result)
                    {
                        arg1=node->get_only_child()->get_first_data();
                        result=data;
                        res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(result)));
                    }
                    else
                    {
                        res.push_back(quaternion(ic_op::ASSIGN,ic_operand::DATA,(void *)(result),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(data)));
                    }
                }
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
                result=nullptr;
                for(auto data:node->related_datas)
                {
                    if(!result)
                    {
                        arg1=node->get_left_child()->get_first_data();
                        arg2=node->get_right_child()->get_first_data();
                        result=data;
                        res.push_back(quaternion(op,ic_operand::DATA,(void *)(arg1),ic_operand::DATA,(void *)(arg2),ic_operand::DATA,(void *)(result)));
                    }
                    else
                    {
                        res.push_back(quaternion(ic_op::ASSIGN,ic_operand::DATA,(void *)(result),ic_operand::NONE,nullptr,ic_operand::DATA,(void *)(data)));
                    }
                }
                break;
            case ic_op::CALL:
                result=node->get_first_data();
                r_params=new list<struct ic_data * >;
                for(auto child:node->children)
                {
                    r_params->push_back(child->get_first_data());
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
                arg1=node->get_only_child()->get_first_data();
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
                    result=node->get_only_child()->get_first_data();
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