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

//===================================== struct ic_data =====================================//

//用户定义的局部变量的构造函数
ic_data::ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const):type(is_const?ic_data_type::LOCAL_CONST_VAR:ic_data_type::LOCAL_VAR),data_type(data_type),dimensions_len(dimensions_len),const_or_init_value(const_or_init_value),scope(nullptr)
{
    size_t name_size=name.size();
    var_name=new char[name_size+1];
    strcpy((char *)var_name,name.c_str());
}

//函数形参的构造函数
ic_data::ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,bool is_const):type(is_const?ic_data_type::CONST_FUNC_F_PARAM:ic_data_type::FUNC_F_PARAM),data_type(data_type),dimensions_len(dimensions_len),scope(nullptr)
{
    size_t name_size=name.size();
    var_name=new char[name_size+1];
    strcpy((char *)var_name,name.c_str());
}

//临时变量的构造函数
ic_data::ic_data(temporary_variable_index tmp_index,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const):type(is_const?ic_data_type::CONST_TMP_VAR:ic_data_type::TMP_VAR),tmp_index(tmp_index),data_type(data_type),dimensions_len(dimensions_len),const_or_init_value(const_or_init_value),scope(nullptr)
{
    
}

//纯常量的构造函数
ic_data::ic_data(enum language_data_type data_type,OAA const_or_init_value):var_name(nullptr),type(ic_data_type::PURE_CONST),data_type(data_type),dimensions_len(nullptr),const_or_init_value(const_or_init_value),scope(nullptr)
{

}

//数组元素的构造函数
ic_data::ic_data(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset):type(ic_data_type::ARRAY_MEMBER),var_name(array_var->var_name),data_type(array_var->data_type),scope(nullptr)
{
    size_t i=0;
    pair<struct ic_data *,struct ic_data * > * tmp=new pair<struct ic_data *,struct ic_data * >;
    list<struct ic_data * > * array_var_dimensions_len=array_var->dimensions_len;
    if(array_var_dimensions_len->size()>offset_dimension_lens)
    {
        dimensions_len=new list<struct ic_data * >;
        i=array_var_dimensions_len->size()-offset_dimension_lens;
        for(list<struct ic_data * >::reverse_iterator it=array_var_dimensions_len->rbegin();it!=array_var_dimensions_len->rend();it++)
        {
            if(i--!=0)
            {
                dimensions_len->push_front(*it);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        dimensions_len=nullptr;
    }

    tmp->first=array_var;
    tmp->second=offset;
    const_or_init_value=OAA((void *)tmp);
}

//判断该ic_data是不是临时变量
bool ic_data::is_tmp_var() const
{
    return (type==ic_data_type::TMP_VAR || type==ic_data_type::CONST_TMP_VAR);
}

//判断该ic_data是不是数组
bool ic_data::is_array_var() const
{
    return dimensions_len!=nullptr;
}

//判断该ic_data是不是const的，包括const变量和纯const
bool ic_data::is_const() const
{
    return (is_const_var() || is_pure_const());
}

//判断该ic_data是不是const变量
bool ic_data::is_const_var() const
{
    return (type==ic_data_type::LOCAL_CONST_VAR || type==ic_data_type::GLOBAL_CONST_VAR || type==ic_data_type::CONST_TMP_VAR);
}

//判断该ic_data是不是纯const
bool ic_data::is_pure_const() const
{
    return type==ic_data_type::PURE_CONST;
}

//判断该ic_data是不是数组元素
bool ic_data::is_array_member() const
{
    return type==ic_data_type::ARRAY_MEMBER;
}

//判断该ic_data是不是函数形参
bool ic_data::is_f_param() const
{
    return (type==ic_data_type::FUNC_F_PARAM || type==ic_data_type::CONST_FUNC_F_PARAM);
}

//检查该ic_data和另一个ic_data是不是有关联的
//只有数组和数组取元素会和其他的变量有关联
bool ic_data::check_ic_data_related(struct ic_data * var)
{
    if(is_array_member())
    {
        //对于数组取元素来说
        //先检查其所属的数组在定义的时候的偏移量中有没有var
        for(auto i:(*get_belong_array()->dimensions_len))
        {
            if(i==var || (i->is_array_member() && i->check_ic_data_related(var)))
            {
                return true;
            }
        }
        //再检查该数组取元素在取偏移地址的时候有没有var
        if(get_offset()==var || (get_offset()->is_array_member() && get_offset()->check_ic_data_related(var)))
        {
            return true;
        }
        //最后检查其所属数组是不是var
        if(get_belong_array()==var)
        {
            return true;
        }
    }
    if(is_array_var())
    {
        //对于数组来说，检测var是不是定义数组的时候的某一个维度的偏移量
        for(auto i:*dimensions_len)
        {
            if(i==var)
            {
                return true;
            }
        }
    }
    return false;
}

//获取变量的值
OAA ic_data::get_value() const
{
    return const_or_init_value;
}

//获取变量的值
void ic_data::set_value(OAA new_value)
{
    const_or_init_value=new_value;
}

//获取该变量的数据类型
enum language_data_type ic_data::get_data_type() const
{
    return data_type;
}

//为一个数组取元素变量获取它所属的数组
struct ic_data * ic_data::get_belong_array() const
{
    if(!is_array_member())
    {
        return nullptr;
    }
    return ((pair<struct ic_data *,struct ic_data * > *)const_or_init_value.pointer_data)->first;
}

//为一个数组取元素变量获取它的偏移量
struct ic_data * ic_data::get_offset() const
{
    if(!is_array_member())
    {
        return nullptr;
    }
    return ((pair<struct ic_data *,struct ic_data * > *)const_or_init_value.pointer_data)->second;
}

//获取变量的名字
string ic_data::get_var_name() const
{
    if(var_name)
    {
        return string(var_name);
    }
    return "";
};

//获取变量的作用域
struct ic_scope * ic_data::get_scope() const
{
    return this->scope;
}

//判断该变量是不是全局变量
bool ic_data::is_global() const
{
    bool res=false;
    if(scope!=nullptr)
    {
        res=scope->is_global();
    }
    return res;
}

//获取变量所占的空间(以byte为单位)
size_t ic_data::get_byte_size(bool is_r_param) const
{
    size_t res=0,array_member_num=1;
    pair<struct ic_data *,list<struct ic_data * > * > * tmp;
    if(is_array_var())
    {
        if(is_f_param() || is_r_param)
        {
            //如果这个数组变量是一个函数形参的话
            //那么该数组所占空间的大小就是一个指针的大小
            res=bits_of_data_types.at(language_data_type::POINTER)/8;
        }
        else
        {
            //如果某一个数组是局部变量或者全局变量（总之不是函数形参）的话
            //那么该数组所占的空间就是这个数组中所有的元素加起来的空间大小
            for(auto i:(*dimensions_len))
            {
                array_member_num*=i->get_value().int_data;
            }
            res=array_member_num*(bits_of_data_types.at(data_type)/8);
        }
    }
    else if(is_array_member())
    {
        //对于数组取元素这个变量来说，需要判断取元素的时候的情况
        tmp=(pair<struct ic_data *,list<struct ic_data * > * > *)const_or_init_value.pointer_data;
        if(tmp->first->dimensions_len->size()==tmp->second->size())
        {
            res=bits_of_data_types.at(data_type)/8;
        }
        else
        {
            res=bits_of_data_types.at(language_data_type::POINTER)/8;
        }
    }
    else
    {
        res=bits_of_data_types.at(data_type)/8;
    }
    return res;
}

//获取变量所占据的内存空间（以4bytes为单位）
size_t ic_data::get_4_bytes_size(bool is_r_param) const
{
    size_t res=0,byte_size=get_byte_size(is_r_param);
    while(res*4<byte_size)
    {
        res++;
    }
    return res;
}

//==========================================================================//



//===================================== struct ic_label =====================================//

ic_label::ic_label(string label_name):name(label_name)
{

}

//==========================================================================//



//===================================== struct ic_func =====================================//

ic_func::ic_func(string name,enum language_data_type return_type,list<struct ic_data * > * f_params):name(name),return_type(return_type),f_params(f_params),scope(nullptr)
{
    
}

ic_func::ic_func(string name,enum language_data_type return_type,size_t f_params_num,...):name(name),return_type(return_type),scope(nullptr)
{
    struct ic_data * f_param;
    f_params=new list<struct ic_data * >;
    va_list argptr;
    va_start(argptr,f_params_num);
    for(size_t i=0;i<f_params_num;i++)
    {
        f_param=va_arg(argptr,struct ic_data *);
        f_params->push_back(f_param);
    }
    va_end(argptr);
}

struct ic_data * ic_func::get_f_param(string f_param_name) const
{
    struct ic_data * res=nullptr;
    for(auto i:*f_params)
    {
        if(i->get_var_name()==f_param_name)
        {
            res=i;
            break;
        }
    }
    return res;
}

list<struct ic_data * > ic_func::get_f_params() const
{
    return (*f_params);
}

bool ic_func::add_f_param(struct ic_data * f_param)
{
    bool res=true;
    string f_param_name=f_param->get_var_name();
    if(get_f_param(f_param_name)==nullptr)
    {
        f_params->push_back(f_param);
    }
    else
    {
        res=false;
    }
    return res;
}

void ic_func::set_scope(struct ic_scope * scope)
{
    this->scope=scope;
}

//获取函数的所有局部变量（不包括临时变量和函数形参，但是会包括const变量）
list<struct ic_data * > ic_func::get_local_vars() const
{
    list<struct ic_data * > res;
    stack<struct ic_scope * > scopes;
    struct ic_scope * current_scope;
    scopes.push(scope);
    
    while(!scopes.empty())
    {
        current_scope=scopes.top();
        scopes.pop();
        for(auto i:current_scope->children)
        {
            if(i)
            {
                scopes.push(i);
            }
        }
        for(auto i:current_scope->vars)
        {
            if(!i.second->is_f_param())
            {
                res.push_back(i.second);
            }
        }
    }

    return res;
}

//==========================================================================//



//===================================== struct ic_scope =====================================//

ic_scope::ic_scope():type(ic_scope_type::GLOBAL),father(nullptr),func(nullptr)
{

}

ic_scope::ic_scope(struct ic_scope * father,struct ic_func * func):type(ic_scope_type::FUNC),father(father),func(func)
{
    func->set_scope(this);
    for(auto i : *(func->f_params))
    {
        add_var(i);
    }
    father->children.push_back(this);
}

ic_scope::ic_scope(struct ic_scope * father):type(ic_scope_type::ANONYMOUS),father(father),func(nullptr)
{
    father->children.push_back(this);
}

void ic_scope::set_scope_type(enum ic_scope_type type)
{
    if(this->type==ic_scope_type::ANONYMOUS || this->type==ic_scope_type::IF || this->type==ic_scope_type::ELSE || this->type==ic_scope_type::WHILE || this->type==ic_scope_type::INLINE_FUNC)
    {
        if(this->father->different_type_children_num.find(type)==this->father->different_type_children_num.end())
        {
            this->father->different_type_children_num.insert(make_pair(type,0));
        }
        this->index_for_output=this->father->different_type_children_num.at(type)++;
        this->type=type;
    }
}

struct ic_scope * ic_scope::get_father() const
{
    return this->father;
}

bool ic_scope::is_global() const
{
    return this->type==ic_scope_type::GLOBAL;
}

bool ic_scope::is_func() const
{
    return this->type==ic_scope_type::FUNC;
}

bool ic_scope::is_inline_func() const
{
    return this->type==ic_scope_type::INLINE_FUNC;
}

struct ic_data * ic_scope::get_var(string var_name)
{
    struct ic_data * res=nullptr;
    if(vars.find(var_name)!=vars.end())
    {
        res=vars.at(var_name);
    }
    return res;
}

bool ic_scope::add_var(struct ic_data * new_var)
{
    bool res=true;
    string new_var_name=new_var->get_var_name();
    if(get_var(new_var_name)==nullptr)
    {
        new_var->scope=this;
        vars.insert(make_pair(new_var_name,new_var));
    }
    else
    {
        res=false;
    }
    return res;
}

//==========================================================================//



//===================================== class Symbol_table =====================================//

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
    //void putfloat(float a)
    functions_.insert(make_pair("putfloat",new struct ic_func("putfloat",language_data_type::VOID,(size_t)1,new struct ic_data("a",language_data_type::FLOAT,nullptr,false))));
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
offset_dimension_lens:数组取元素的维度长度
offsets:取元素的维度偏移

Return
------
要获取的表项
*/
struct ic_data * Symbol_table::array_member_entry(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset)
{
    pair<struct ic_data *,pair<size_t,struct ic_data * > > index;
    //查看偏移量，如果偏移量是constg关键字指定的变常量，那么就将其转换成常量
    if(offset->is_const_var())
    {
        offset=const_entry(offset->get_data_type(),offset->get_value());
    }
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
获取符号表中的某一个数组取元素（不是数组）

Parameters
----------
array_var:数组变量
offsets:取元素的维度偏移

Return
------
要获取的表项
*/
struct ic_data * Symbol_table::array_member_not_array_var_entry(struct ic_data * array_var,struct ic_data * offset)
{
    return array_member_entry(array_var,array_var->dimensions_len->size(),offset);
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
belong_scope:新增的变量的作用域，默认是nulltr，就是使用当前作用域

Return
------
如果添加成功，就返回参数ic_data，否则就会把ic_data释放，并且返回nullptr
*/
struct ic_data * Symbol_table::new_var(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const,struct ic_scope * belong_scope)
{
    struct ic_data * res=nullptr;
    if(belong_scope)
    {
        res=new struct ic_data(name,data_type,dimensions_len,const_or_init_value,is_const);
        belong_scope->add_var(res);
    }
    else if(current_scope_)
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

/*
增加一个函数会更改的全局变量和数组形参

Parameters
----------
func:要增加的函数
data:要增加的数据
*/
void Symbol_table::add_func_def_globals_and_f_params(struct ic_func * func,struct ic_data * data)
{
    if(data->is_array_member())
    {
        data=data->get_belong_array();
    }
    if(data->is_f_param() || data->is_global())
    {
        if(funcs_def_globals_and_f_params_.find(func)==funcs_def_globals_and_f_params_.end())
        {
            funcs_def_globals_and_f_params_.insert(make_pair(func,set<struct ic_data * >()));
        }
        funcs_def_globals_and_f_params_.at(func).insert(data);
    }
}

/*
增加一个函数会使用的全局变量和数组形参

Parameters
----------
func:要增加的函数
data:要增加的数据
*/
void Symbol_table::add_func_use_globals_and_f_params(struct ic_func * func,struct ic_data * data)
{
    if(data->is_array_member())
    {
        data=data->get_belong_array();
    }
    if(data->is_f_param() || data->is_global())
    {
        if(funcs_use_globals_and_f_params_.find(func)==funcs_use_globals_and_f_params_.end())
        {
            funcs_use_globals_and_f_params_.insert(make_pair(func,set<struct ic_data * >()));
        }
        funcs_use_globals_and_f_params_.at(func).insert(data);
    }
}

/*
增加一个函数会直接调用的函数

Parameters
----------
func:要增加直接调用的函数
called_func:被调用的函数
*/
void Symbol_table::add_func_direct_calls(struct ic_func * func,struct ic_func * called_func)
{
    if(funcs_direct_calls_.find(func)==funcs_direct_calls_.end())
    {
        funcs_direct_calls_.insert(make_pair(func,set<struct ic_func * >()));
    }
    funcs_direct_calls_.at(func).insert(called_func);
}

/*
获取一个函数会更改的所有全局变量和数组形参

Parameters
----------
func:要获取的函数

Return
------
返回该函数会更改的所有全局变量和数组形参
*/
set<struct ic_data * > Symbol_table::get_func_def_globals_and_f_params(struct ic_func * func)
{
    set<struct ic_data * > res;
    if(funcs_def_globals_and_f_params_.find(func)!=funcs_def_globals_and_f_params_.end())
    {
        res=funcs_def_globals_and_f_params_.at(func);
    }
    return res;
}

/*
获取一个函数会使用的所有全局变量和数组形参

Parameters
----------
func:要获取的函数

Return
------
返回该函数会使用的所有全局变量和数组形参
*/
set<struct ic_data * > Symbol_table::get_func_use_globals_and_f_params(struct ic_func * func)
{
    set<struct ic_data * > res;
    if(funcs_use_globals_and_f_params_.find(func)!=funcs_use_globals_and_f_params_.end())
    {
        res=funcs_use_globals_and_f_params_.at(func);
    }
    return res;
}

/*
获取一个函数会直接调用的所有函数

Parameters
----------
func:要获取的函数

Return
------
返回该函数会直接调用的所有函数
*/
set<struct ic_func * > Symbol_table::get_func_direct_calls(struct ic_func * func)
{
    set<struct ic_func * > res;
    if(funcs_direct_calls_.find(func)!=funcs_direct_calls_.end())
    {
        res=funcs_direct_calls_.at(func);
    }
    return res;
}

//==========================================================================//