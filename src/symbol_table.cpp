/*
 *
 *  symbol_table.h
 *  符号表
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/

#include "symbol_table.h"
#include<iostream>
#include<sstream>

using namespace std;

//单例对象
Symbol_table * Symbol_table::instance_=nullptr;

/*
Symbol_table的私有构造函数
*/
Symbol_table::Symbol_table():tmp_vars_num_(0),labels_num_(0),current_scope_(nullptr),current_func_(nullptr)
{
    list<struct ic_data * > * dimensions_len;
    //把库中的函数进行定义
    //int getint()
    functions_.insert(make_pair("getint",new struct ic_func("getint",language_data_type::INT,(size_t)0)));
    //int getch()
    functions_.insert(make_pair("getch",new struct ic_func("getch",language_data_type::INT,(size_t)0)));
    //int getarray(int a[])
    dimensions_len=new list<struct ic_data * >;
    dimensions_len->push_back(const_entry(language_data_type::INT,OAA((int)0)));
    functions_.insert(make_pair("getarray",new struct ic_func("getarray",language_data_type::INT,(size_t)1,new struct ic_data("a",language_data_type::INT,dimensions_len,false))));
    //int getfarray(float a[])
    dimensions_len=new list<struct ic_data * >;
    dimensions_len->push_back(const_entry(language_data_type::INT,OAA((int)0)));
    functions_.insert(make_pair("getfarray",new struct ic_func("getfarray",language_data_type::INT,(size_t)1,new struct ic_data("a",language_data_type::FLOAT,dimensions_len,false))));
    //float getfloat()
    functions_.insert(make_pair("getfloat",new struct ic_func("getfloat",language_data_type::FLOAT,(size_t)0)));
    //void putint(int a)
    functions_.insert(make_pair("putint",new struct ic_func("putint",language_data_type::VOID,(size_t)1,new struct ic_data("a",language_data_type::INT,nullptr,false))));
    //void putch(int a)
    functions_.insert(make_pair("putch",new struct ic_func("putch",language_data_type::VOID,(size_t)1,new struct ic_data("a",language_data_type::INT,nullptr,false))));
    //void putarray(int n,int a[])
    dimensions_len=new list<struct ic_data * >;
    dimensions_len->push_back(const_entry(language_data_type::INT,OAA((int)0)));
    functions_.insert(make_pair("putarray",new struct ic_func("putarray",language_data_type::VOID,(size_t)2,new struct ic_data("n",language_data_type::INT,nullptr,false),new struct ic_data("a",language_data_type::INT,dimensions_len,false))));
    //void putfloat()
    functions_.insert(make_pair("putfloat",new struct ic_func("putfloat",language_data_type::VOID,(size_t)0)));
    //void putfarray(int n,float a[])
    dimensions_len=new list<struct ic_data * >;
    dimensions_len->push_back(const_entry(language_data_type::INT,OAA((int)0)));
    functions_.insert(make_pair("putfarray",new struct ic_func("putfarray",language_data_type::VOID,(size_t)2,new struct ic_data("n",language_data_type::INT,nullptr,false),new struct ic_data("a",language_data_type::FLOAT,dimensions_len,false))));
    //void _sysy_starttime(int lineno)
    functions_.insert(make_pair("_sysy_starttime",new struct ic_func("_sysy_starttime",language_data_type::VOID,(size_t)1,new struct ic_data("lineno",language_data_type::INT,nullptr,false))));
    //void _sysy_stoptime(int lineno)
    functions_.insert(make_pair("_sysy_stoptime",new struct ic_func("_sysy_stoptime",language_data_type::VOID,(size_t)1,new struct ic_data("lineno",language_data_type::INT,nullptr,false))));
    //开始一个全局作用域
    start_scope();
}

/*
Symbol_table的私有析构函数
*/
Symbol_table::~Symbol_table()
{
    
}

/*
获取Symbol_table单例类的方法
*/
Symbol_table * Symbol_table::get_instance()
{
    if(instance_==nullptr)
    {
        instance_=new Symbol_table();
    }
    return instance_;
}

/*
销毁Symbol_table单例类的方法
*/
void Symbol_table::delete_instance()
{
    if(instance_!=nullptr)
    {
        delete instance_;
        instance_=nullptr;
    }
}

/*
获取符号表中的某一变量表项的指针

Parameters
----------
symbol_name:要获取的符号名字

Return
------
要获取的表项，如果找不到就返回nullptr
*/
struct ic_data * Symbol_table::var_entry(string var_name) const
{
    struct ic_scope * scope=current_scope_;
    struct ic_data * res=nullptr;
    while(scope)
    {
        res=scope->get_var(var_name);
        if(res!=nullptr)
        {
            break;
        }
        scope=scope->get_father();
    }
    return res;
}

/*
获取符号表中的某一函数表项的指针

Parameters
----------
symbol_name:要获取的符号名字

Return
------
要获取的表项
*/
struct ic_func * Symbol_table::func_entry(string func_name) const
{
    if(functions_.find(func_name)!=functions_.end())
    {
        return functions_.at(func_name);
    }
    return nullptr;
}

/*
获取符号表中的某一个常量，如果没有的话就新建一个

Parameters
----------
data_type:常量的数据类型
data_value:常量的数据

Return
------
要获取的表项
*/
struct ic_data * Symbol_table::const_entry(enum language_data_type data_type,OAA data_value)
{
    map<OAA,struct ic_data * > & designated_data_type_consts=consts_[data_type];
    if(designated_data_type_consts.find(data_value)==designated_data_type_consts.end())
    {
        designated_data_type_consts.insert(make_pair(data_value,new struct ic_data(data_type,data_value)));
    }
    return designated_data_type_consts[data_value];
}

/*
获取符号表中的某一个数组取元素，如果没有的话就新建一个

Parameters
----------
array_var:数组变量
offsets:取元素的维度偏移

Return
------
要获取的表项
*/
struct ic_data * Symbol_table::array_member_entry(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset)
{
    pair<struct ic_data *,pair<size_t,struct ic_data * > > index;
    //如果该数组取元素时的数组和偏移都是常量，同时数组取元素本身不是一个数组的话，就使用返回常量
    if(array_var->is_const_var() && offset->is_const() && offset_dimension_lens==array_var->dimensions_len->size())
    {
        return const_entry(array_var->get_data_type(),((vector<OAA> * )array_var->get_value().pointer_data)->at(offset->get_value().int_data));
    }
    //否则的话就只能使用数组取元素变量
    index=make_pair(array_var,make_pair(offset_dimension_lens,offset));
    if(array_members_.find(index)==array_members_.end())
    {
        array_members_.insert(make_pair(index,new struct ic_data(array_var,offset_dimension_lens,offset)));
    }
    return array_members_.at(index);
}

/*
往符号表中添加一个用户定义的变量

Parameters
----------
name:变量名
data_type:变量类型
dimensions_len:存储数组各个维度的长度的数组,如果这个参数是nullptr，说明此时这个变量是基本变量，反之则说明是数组变量
const_or_init_value:常量的值
is_const:是否是const变量

Return
------
如果添加成功，就返回参数ic_data，否则就会把ic_data释放，并且返回nullptr
*/
struct ic_data * Symbol_table::new_var(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const)
{
    struct ic_data * res=nullptr;
    if(current_scope_)
    {
        res=new struct ic_data(name,data_type,dimensions_len,const_or_init_value,is_const);
        current_scope_->add_var(res);
    }
    return res;
}

/*
往符号表中添加一个临时变量

Return
------
返回添加成功的临时变量指针
*/
struct ic_data * Symbol_table::new_tmp_var(enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const)
{
    return new struct ic_data(tmp_vars_num_++,data_type,dimensions_len,const_or_init_value,is_const);
}

/*
定义一个标签

Return
------
返回该标签在符号表中的表项指针，如果定义失败就返回nullptr
*/
struct ic_label * Symbol_table::new_label()
{
    return new struct ic_label("."+to_string(labels_num_++));
}

/*
定义一个函数

Parameters
----------
name:函数名
return_type:函数返回值类型
f_params:函数形参列表(如果函数没有形参，那么这个参数就是nullptr)
func_scope:函数定义域

Return
------
如果定义成功，返回该函数在符号表中的表项的指针，否则返回nullptr
*/
struct ic_func * Symbol_table::new_func(string name,enum language_data_type return_type,list<struct ic_data * > * f_params)
{
    //需要查看是否有重名的函数被定义了(函数之间不能重名,但是函数和变量可以重名)
    if(func_entry(name)!=nullptr)
    {
        return nullptr;
    }
    current_func_=new struct ic_func(name,return_type,f_params);
    functions_[name]=current_func_;
    return current_func_;
}

/*
获取正在生成的函数

Return
------
返回正在生成的函数
*/
struct ic_func * Symbol_table::get_current_func()
{
    return current_func_;
}

/*
开始一个新的作用域

Parameters
----------
type:作用域类型
*/
void Symbol_table::start_scope()
{
    struct ic_scope * new_scope=nullptr;
    if(!current_scope_)
    {
        new_scope=new struct ic_scope();
    }
    else
    {
        if(current_scope_->is_global())
        {
            new_scope=new struct ic_scope(current_scope_,current_func_);
        }
        else
        {
            new_scope=new struct ic_scope(current_scope_);
        }
    }
    current_scope_=new_scope;
}

/*
结束当前的作用域

Return
------
返回结束了的作用域
*/
struct ic_scope * Symbol_table::end_scope()
{
    struct ic_scope * res=current_scope_;
    if(current_scope_)
    {
        current_scope_=current_scope_->get_father();
    }
    return res;
}

/*
获取当前的作用域

Return
------
返回当前的作用域
*/
struct ic_scope * Symbol_table::get_current_scope()
{
    return current_scope_;
}