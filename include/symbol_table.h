/*
 *
 *  symbol_table.h
 *  符号表
 *  Creator:Sichao Chen
 *  Create time:2021/10/01
 *
*/
#ifndef __SYMBOL_TABLE_H
#define __SYMBOL_TABLE_H

#include<map>
#include<string>
#include<vector>
#include<list>
#include<stack>
#include<set>
#include<utility>
#include<cstring>
#include<stdarg.h>
#include"language.h"
#include"util.h"

using namespace std;

//临时变量编号
typedef size_t temporary_variable_index;

//各个数据类型所占字节
extern map<enum language_data_type,size_t> bits_of_data_types;

struct ic_scope;

//struct var的类型
enum class ic_data_type
{
    LOCAL_VAR,                  //用户定义的局部变量
    LOCAL_CONST_VAR,            //用户定义的局部const变量
    GLOBAL_VAR,                 //用户定义的全局变量
    GLOBAL_CONST_VAR,           //用户定义的全局const变量
    TMP_VAR,                    //临时变量
    CONST_TMP_VAR,              //const临时变量
    PURE_CONST,                 //纯常量
    FUNC_F_PARAM,               //函数形参
    CONST_FUNC_F_PARAM,         //const函数形参
    ARRAY_MEMBER,               //数组成员
};

//符号表中的变量（广义的）
//包括定义的变量和临时变量以及纯常数
//包括基础变量和数组
struct ic_data
{
    //用户定义的局部变量的构造函数
    ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const);

    //函数形参的构造函数
    ic_data(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,bool is_const);

    //临时变量的构造函数
    ic_data(temporary_variable_index tmp_index,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const);

    //纯常量的构造函数
    ic_data(enum language_data_type data_type,OAA const_or_init_value);

    //数组元素的构造函数
    ic_data(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset);

    //判断该ic_data是不是临时变量
    bool is_tmp_var() const;

    //判断该ic_data是不是数组
    bool is_array_var() const;

    //判断该ic_data是不是const的，包括const变量和纯const
    bool is_const() const;

    //判断该ic_data是不是const变量
    bool is_const_var() const;

    //判断该ic_data是不是纯const
    bool is_pure_const() const;

    //判断该ic_data是不是数组元素
    bool is_array_member() const;

    //判断该ic_data是不是函数形参
    bool is_f_param() const;

    //检查该ic_data和另一个ic_data是不是有关联的
    //只有数组和数组取元素会和其他的变量有关联
    bool check_ic_data_related(struct ic_data * var);

    //获取变量的值
    OAA get_value() const;

    //获取变量的值
    void set_value(OAA new_value);

    //获取该变量的数据类型
    enum language_data_type get_data_type() const;

    //为一个数组取元素变量获取它所属的数组
    struct ic_data * get_belong_array() const;

    //为一个数组取元素变量获取它的偏移量
    struct ic_data * get_offset() const;

    //获取变量的名字
    string get_var_name() const;

    //获取变量的作用域
    struct ic_scope * get_scope() const;

    //判断该变量是不是全局变量
    bool is_global() const;

    //获取变量所占的空间(以byte为单位)
    size_t get_byte_size(bool is_r_param=false) const;

    //获取变量所占据的内存空间（以4bytes为单位）
    size_t get_4_bytes_size(bool is_r_param=false) const;

    //数据类型
    enum ic_data_type type;
    union
    {
        const char * var_name;                      //标识符名称字符串
        temporary_variable_index tmp_index;         //临时变量的编号
    };
    //标识符类型（或者是数组元素类型）
    enum language_data_type data_type;
    //这个成员只有当变量是数组的时候才有效，表示数组的各个维度的长度(如果某一个维度的长度为0，说明这个维度是不限长的),如果变量不是数组，那么这个成员就是nullptr
    //把地址（也就是指针）看成是一维数组
    list<struct ic_data * > * dimensions_len;
    //下面讨论这个const_or_init_value成员的具体含义：
    //if(is_const())
        //if(is_array_var())
            //那么如果该数组被初始化过，那么该成员就是一个vector<OAA> *，该数组存储着数组初始化的值；否则的话就是nullptr
        //else
            //该成员就是该变量的值
    //else if(is_global())
        //if(is_array_var())
            //那么如果该数组被初始化过，那么该成员就是一个vector<OAA> *，该数组存储着数组初始化的值；否则的话就是nullptr
        //else
            //该成员就是一个struct ic_data * ，如果它是nullptr，那么表示该变量没有初始化，否则的话这个指针指向的就是一个pure_const，就是该变量的初始值
    //else if(is_array_member())
        //该成员是一个pair<struct ic_data *,struct ic_data * > *，这个pair的first是该数组元素所属的数组，second是取数组元素的时候的偏移量(在中间代码的时候，把数组取元素全都变成一维的)
    //else
        //if(is_array_var())
            //如果该数组初始化过，那么该成员就是true；否则就是false
        //else
            //该成员是nullptr
    OAA const_or_init_value;
    //作用域
    struct ic_scope * scope;
};

//标签
struct ic_label
{
    ic_label(string label_name);

    string name;                    //标签名
};

//函数类型
enum class func_type
{
    PROGRAMER_DEFINED,              //程序员定义函数
    LIBRARY,                        //库函数
    THREAD_PART,                    //多线程部分函数
};

//函数
struct ic_func
{
    ic_func(string name,enum func_type type,enum language_data_type return_type,list<struct ic_data * > * f_params);

    ic_func(string name,enum func_type type,enum language_data_type return_type,size_t f_params_num,...);

    struct ic_data * get_f_param(string f_param_name) const;

    bool add_f_param(struct ic_data * f_param);

    void set_scope(struct ic_scope * scope);

    list<struct ic_data * > get_local_vars() const;

    string name;                                        //标识符名称字符串
    enum language_data_type return_type;                //返回值类型
    list<struct ic_data * > * f_params;                 //函数形参
    struct ic_scope * scope;                            //函数作用域
    enum func_type type;                                //函数类型
    //bool is_external;                                   //是否是外部函数
};

//作用域类型
enum class ic_scope_type
{
    GLOBAL,
    FUNC,
    IF,
    ELSE,
    WHILE,
    ANONYMOUS,
    INLINE_FUNC,
};

//变量的作用域
struct ic_scope
{
    ic_scope();

    ic_scope(struct ic_scope * father,struct ic_func * func);

    ic_scope(struct ic_scope * father);

    void set_scope_type(enum ic_scope_type type);

    struct ic_scope * get_father() const;
    
    bool is_global() const;

    bool is_func() const;

    bool is_inline_func() const;

    struct ic_data * get_var(string var_name);

    bool add_var(struct ic_data * new_var);

    enum ic_scope_type type;
    map<string,struct ic_data * > vars;
    struct ic_func * func;
    size_t index_for_output;
    struct ic_scope * father;
    list<struct ic_scope * > children;
    map<enum ic_scope_type,size_t> different_type_children_num;
};

//定义符号表
class Symbol_table
{

protected:
    //单例类对象
    static Symbol_table * instance_;

    //构造函数
    Symbol_table();

    //析构函数
    ~Symbol_table();

    //优化选项
    bool optimize_setting_;

    //debug选项
    bool debug_setting_;

    //目标文件名
    string target_filename_;

    //临时变量的个数
    size_t tmp_vars_num_;

    //标签的个数
    size_t labels_num_;

    //局部转全局数组的个数
    size_t l2g_array_nums_;

    //所有已经声明的函数
    map<string,struct ic_func * > functions_;

    //每个函数会改变的全局变量和形参
    map<struct ic_func *,set<struct ic_data * > > func_s_def_globals_and_f_params_;

    //每个函数会使用的全局变量和形参
    map<struct ic_func *,set<struct ic_data * > > func_s_use_globals_and_f_params_;

    //每个函数会直接调用的函数
    map<struct ic_func *,set<struct ic_func * > > func_s_direct_calls_;

    //每个函数会涉及到的数据类型
    map<struct ic_func *,set<enum language_data_type> > func_s_related_data_type_;

    //常量
    map<enum language_data_type,map<OAA,struct ic_data * > > consts_;

    //数组取元素
    map<pair<struct ic_data *,pair<size_t,struct ic_data * > >,struct ic_data * > array_members_;

    //目前正在生成的函数中
    struct ic_func * current_func_;

    //目前的作用域
    struct ic_scope * current_scope_;

    //全局作用域
    struct ic_scope * global_scope_;

public:
    //单例模式不应该有克隆构造函数
    Symbol_table(Symbol_table & other) = delete;
    
    //单例模式不应该重载赋值运算符
    void operator=(const Symbol_table &) = delete;

    //获取单例对象的方法
    static Symbol_table * get_instance();

    //销毁单例类
    static void delete_instance();

    //设置优化选项
    void set_optimize_setting(bool optimize);

    //获取优化选项
    bool get_optimize_setting() const;

    //设置debug选项
    void set_debug_setting(bool debug);

    //获取debug选项
    bool get_debug_setting() const;

    //设置目标文件名
    void set_target_filename(string target_filename);

    //获取目标文件名
    string get_target_filename() const;

    //往符号表中添加一个用户定义的变量
    struct ic_data * new_var(string name,enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const,struct ic_scope * belong_scope=nullptr);

    //往符号表中添加一个临时变量
    struct ic_data * new_tmp_var(enum language_data_type data_type,list<struct ic_data * > * dimensions_len,OAA const_or_init_value,bool is_const);

    //定义一个标签
    struct ic_label * new_label();

    //定义一个函数
    struct ic_func * new_func(string name,enum func_type type,enum language_data_type return_type,list<struct ic_data * > * f_params);

    //获取正在生成的函数
    struct ic_func * get_current_func();

    //开始一个新的作用域
    void start_scope();

    //结束当前的作用域
    struct ic_scope * end_scope();

    //获取当前的作用域
    struct ic_scope * get_current_scope();

    //获取全局作用域
    struct ic_scope * get_global_scope();

    //获取符号表中的某一个变量
    struct ic_data * var_entry(string var_name) const;

    //获取符号表中的某一个函数
    struct ic_func * func_entry(string func_name) const;

    //获取符号表中的某一个常量
    struct ic_data * const_entry(enum language_data_type data_type,OAA data_value);

    //获取符号表中的某一个数组取元素
    struct ic_data * array_member_entry(struct ic_data * array_var,size_t offset_dimension_lens,struct ic_data * offset);

    //获取符号表中的某一个数组取元素（不是数组）
    struct ic_data * array_member_not_array_var_entry(struct ic_data * array_var,struct ic_data * offset);

    //增加一个函数会更改的全局变量和形参
    void add_func_def_globals_and_f_params(struct ic_func * func,struct ic_data * data);

    //增加一个函数会使用的全局变量和形参
    void add_func_use_globals_and_f_params(struct ic_func * func,struct ic_data * data);

    //增加一个函数会直接调用的函数
    void add_func_direct_calls(struct ic_func * func,struct ic_func * called_func,list<struct ic_data * > * r_params);

    //增加一个函数会涉及的的数据结构
    void add_func_related_data_type(struct ic_func * func,enum language_data_type data_type);

    //获取一个函数会更改的所有全局变量和形参
    set<struct ic_data * > get_func_def_globals_and_f_params(struct ic_func * func) const;

    //获取一个函数会使用的所有全局变量和形参
    set<struct ic_data * > get_func_use_globals_and_f_params(struct ic_func * func) const;

    //获取一个函数会更改的所有全局变量和数组形参
    set<struct ic_data * > get_func_def_globals_and_array_f_params(struct ic_func * func) const;

    //获取一个函数会使用的所有全局变量和数组形参
    set<struct ic_data * > get_func_use_globals_and_array_f_params(struct ic_func * func) const;

    //获取一个函数会直接调用的所有函数
    set<struct ic_func * > get_func_direct_calls(struct ic_func * func) const;

    //判断一个函数是否直接或者间接调用过外部函数
    bool is_func_call_external_func(struct ic_func * func) const;

    //查看一个函数是否涉及某一个数据类型
    bool is_func_related_to_a_data_type(struct ic_func * func,enum language_data_type data_type) const;

    //根据函数名判断一个函数是否是定义的函数获取库函数
    bool is_a_defined_or_library_func(string func_name) const;

    //获取一个新的局部转全局数组的名称
    string new_l2g_array_name();
};


#endif //__SYMBOL_TABLE_H