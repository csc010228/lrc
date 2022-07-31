/*
 *
 *  register_manager.h
 *  寄存器管理器
 *  Creator:Sichao Chen
 *  Create time:2022/1/27
 *
*/
#ifndef __REGISTER_MANAGER_H
#define __REGISTER_MANAGER_H

#include<map>
#include<list>
#include<stdarg.h>
#include "symbol_table.h"
#include "asm_generator_component.h"
#include "ic_optimizer.h"

using namespace std;

//寄存器属性
enum class reg_attr
{
    ZERO,               //零寄存器
    STACK_POINTER,      //栈指针寄存器
    FRAME_POINTER,      //帧指针寄存器
    GLOBAL_POINTER,     //全局指针寄存器
    THREAD_POINTER,     //线程指针寄存器
    INTRA_PROCEDURE,    //内部调用临时寄存器
    TEMP,               //临时变量寄存器（在函数调用的时候这些寄存器的值可能会变）
    SAVE,               //保留变量寄存器（在函数调用的时候保证这些寄存器的值不变）
    ARGUMENT,           //参数传递寄存器（在函数调用的时候传递参数,在函数调用的过程中会被释放,也就是在函数调用的时候值可能会改变）
    RETURN_VALUE,       //函数或者表达式返回值寄存器
    RETURN_ADDR,        //返回地址寄存器
    BREAK_OFF,          //中断函数返回值寄存器（不能用做别的用途）
    FLAGS,              //标志寄存器
    PC,                 //程序计数器
};

//寄存器状态
enum class reg_state
{
    EMPTY,              //该寄存器是空的，还没有被使用
    USED,               //该寄存器被使用了
    ALLOCATING,         //该寄存器正在被分配
};

//寄存器中存储的变量状态
enum class reg_var_state
{
    DIRTY,                  //该寄存器存储的变量是脏值（即寄存器的值和对应的变量的内存值不一致，寄存器的值是最新的）
    NOT_DIRTY,              //该寄存器存储的变量不是脏值
    ADDR,                   //该寄存器存储的是变量的地址
};

//寄存器所属的处理器
enum class reg_processor
{
    CPU,
    VFP
};

//寄存器信息
struct reg
{
    reg(string name,reg_index index,enum reg_attr attr,bool writable,size_t size,enum reg_processor processor):name(name),index(index),attr(attr),writable(writable),state(reg_state::EMPTY),size(size),attached_const(false),processor(processor)
    {

    };

    void clear()
    {
        state=reg_state::EMPTY;
        var_datas.clear();
        attached_const=false;
    };

    void add_value(OAA const_value)
    {
        state=reg_state::USED;
        attached_const=true;
        const_data=const_value;
    };

    void add_value(struct ic_data * var)
    {
        state=reg_state::USED;
        if(var_datas.find(var)==var_datas.end())
        {
            var_datas.insert(make_pair(var,reg_var_state::NOT_DIRTY));
        }
    };

    void add_addr(struct ic_data * var)
    {
        state=reg_state::USED;
        if(var_datas.find(var)==var_datas.end())
        {
            var_datas.insert(make_pair(var,reg_var_state::ADDR));
        }
    };

    void set_value_DIRTY(struct ic_data * var)
    {
        if(var_datas.find(var)!=var_datas.end() && var_datas.at(var)==reg_var_state::NOT_DIRTY)
        {
            var_datas.at(var)=reg_var_state::DIRTY;
        }
    };

    void set_value_NOT_DIRTY(struct ic_data * var)
    {
        if(var_datas.find(var)!=var_datas.end() && var_datas.at(var)==reg_var_state::DIRTY)
        {
            var_datas.at(var)=reg_var_state::NOT_DIRTY;
        }
    };

    void remove_value(OAA const_value)
    {
        attached_const=false;
        if(var_datas.empty())
        {
            state=reg_state::EMPTY;
        }
    };

    void remove_value(struct ic_data * var)
    {
        if(var_datas.find(var)!=var_datas.end() && var_datas.at(var)!=reg_var_state::ADDR)
        {
            var_datas.erase(var);
        }
        if(var_datas.empty() && attached_const==false)
        {
            state=reg_state::EMPTY;
        }
    };

    void remove_addr(struct ic_data * var)
    {
        if(var_datas.find(var)!=var_datas.end() && var_datas.at(var)==reg_var_state::ADDR)
        {
            var_datas.erase(var);
        }
        if(var_datas.empty() && attached_const==false)
        {
            state=reg_state::EMPTY;
        }
    };

    bool has_the_only_DIRTY_value(struct ic_data * var) const
    {
        bool res=false;
        for(auto var_data:var_datas)
        {
            if(var_data.second==reg_var_state::DIRTY)
            {
                if(var_data.first==var)
                {
                    res=true;
                }
                else
                {
                    return false;
                }
            }
        }
        return res;
    };

    bool has_no_DIRTY_value() const
    {
        for(auto var_data:var_datas)
        {
            if(var_data.second==reg_var_state::DIRTY)
            {
                return false;
            }
        }
        return true;
    };

    bool operator <(const struct reg & a) const
    {
        return (this->index<a.index);
    };

    bool operator >(const struct reg & a) const
    {
        return (this->index>a.index);
    };

    bool operator <=(const struct reg & a) const
    {
        return (this->index<=a.index);
    };

    bool operator >=(const struct reg & a) const
    {
        return (this->index>=a.index);
    };

    bool operator ==(const struct reg & a) const
    {
        return (this->index==a.index);
    };

    bool operator !=(const struct reg & a) const
    {
        return (this->index!=a.index);
    };

    string name;                //名字
    reg_index index;            //编号
    enum reg_attr attr;         //属性
    bool writable;              //是否可写
    size_t size;                //寄存器大小（单位是bit）
    enum reg_processor processor;                           //所属的处理器
    enum reg_state state;       //状态
    bool attached_const;        //是否存放了常数
    //相关数据
    map<struct ic_data *,enum reg_var_state> var_datas;
    OAA const_data;
};

//标志寄存器的信息
enum class flag_in_flag_reg
{
    NEGATIVE,                   //结果是否大于0
    ZERO,                       //结果是否是0
    CARRY,                      //结果是否进位
    OVERFLOW,                   //结果是否溢出
    SATURATION,                 //结果是否发生了饱和
};

//标志寄存器信息
struct flag_reg
{
    flag_reg():name("")
    {
        
    };

    flag_reg(string name,size_t flag_num_in_flag_reg,...):name(name)
    {
        
    };

    //在当前的标志寄存器的所有标志位和某一个变量之间建立联系
    void attach_var(struct ic_data * var,int cared_flag)
    {
        related_data.first=var;
        related_data.second=cared_flag;
    };

    string name;                //名字
    pair<struct ic_data *,int> related_data;          //标志位寄存器中的所有标志对应的数据
};

struct regs_info
{
    struct flag_reg flag_reg;                      //标志寄存器
    map<reg_index,struct reg> reg_indexs;          //寄存器编号和寄存器之间的映射
    map<string,reg_index> reg_names;               //寄存器名字和寄存器编号之间的映射

    map<struct ic_data *,set<reg_index> > var_value_regs_map;           //存储变量的值和寄存器之间的映射关系的map
    map<OAA,set<reg_index> > const_value_regs_map;                      //存储常数的值和寄存器之间的映射关系的map
    map<struct ic_data *,set<reg_index> > var_addr_regs_map;            //存储变量的地址和寄存器之间的映射关系的map

    list<set<reg_index> > current_instructions_involved_regs;             //存储当前的指令所用到的寄存器

    list<reg_index> available_CPU_regs;             //所有可用的（即临时或者参数）CPU寄存器
    list<reg_index>::iterator current_CPU_reg;
    list<reg_index> available_VFP_regs;             //所有可用的（即临时或者参数）VFP寄存器
    list<reg_index>::iterator current_VFP_reg;

    //构造函数
    regs_info();
    regs_info(set<struct reg> regs,struct flag_reg flag_reg);
    //清空所有的寄存器信息
    void clear();
    //把一个变量的值和一个寄存器建立关联
    void attach_value_to_reg(struct ic_data * var,reg_index reg);
    //把一个常量的值和一个寄存器建立关联
    void attach_value_to_reg(OAA const_value,reg_index reg);
    //把一个变量的地址和一个寄存器建立关联
    void attach_addr_to_reg(struct ic_data * var,reg_index reg);
    //把一个变量的值和一个寄存器解除关联
    void unattach_value_to_reg(struct ic_data * var,reg_index reg);
    //把一个常量的值和一个寄存器解除关联
    void unattach_value_to_reg(OAA const_value,reg_index reg);
    //把一个变量的地址和一个寄存器解除关联
    void unattach_addr_to_reg(struct ic_data * var,reg_index reg);
    //把一个寄存器中的所有变量地址全部和该寄存器解除关联（一般也就只有一个）
    void unattach_reg_s_all_addr(reg_index reg);
    //把一个寄存器上的所有数据和该寄存器解除关联
    void unattach_reg_s_all_data(reg_index reg);
    //获取存放着某一个常数值的所有寄存器
    set<reg_index> get_const_owned_value_regs(OAA const_value);
    //获取存放着某一个变量值的所有寄存器
    set<reg_index> get_var_owned_value_regs(struct ic_data * var);
    //获取存放着某一个变量地址的所有寄存器
    set<reg_index> get_var_owned_addr_regs(struct ic_data * var);
    //获取下一个可用的CPU寄存器
    reg_index next_available_CPU_reg();
    //获取下一个可用的CPU寄存器
    reg_index next_available_VFP_reg();
    //获取当前寄存器堆中最大的寄存器编号
    reg_index get_max_reg_index() const;
    //获取CPU参数寄存器的个数
    size_t get_CPU_ARGUMENT_reg_num() const;
    //获取VFP参数寄存器的个数
    size_t get_VFP_ARGUMENT_reg_num() const;
    //查看一个寄存器是否是ARGUMENT寄存器
    inline bool is_ARGUMENT_REG(reg_index reg) const
    {
        return reg_indexs.at(reg).attr==reg_attr::ARGUMENT;
    }
};

//寄存器管理器
class Register_manager:public Asm_generator_component
{

protected:
    //有效位
    bool is_init_successful_;

    //寄存器信息
    struct regs_info regs_info_;

    //当前正在处理的函数
    struct ic_func_flow_graph * current_func_;

    //获取一个函数在被调用的时候需要用到的寄存器
    list<reg_index> get_func_s_regs_holding_f_params_by_func_name(string func_name);

    //将某一个寄存器设置为正在被当前的指令所使用
    void set_got_by_current_instruction(reg_index reg);

    //查看一个寄存器是否正在被当前的指令所使用
    bool is_got_by_current_instruction(reg_index reg);

    //获取一个函数存放在内存中的函数形参
    list<struct ic_data * > get_func_s_f_params_in_memory(struct ic_func * func);

    //新分配一个空闲的寄存器
    virtual reg_index allocate_idle_reg(reg_processor processor,bool get_an_empty_reg=false);

    //为某一个常量获取一个寄存器
    virtual reg_index get_reg_for_const(OAA const_data,enum reg_processor processor,bool get_an_empty_reg=false);

    //把一个数组的所有脏值元素全部写回内存，并且将其设置为非脏值
    virtual void store_array_s_all_DIRTY_members_and_set_them_NOT_DIRTY(struct ic_data * array,bool deal_with_const_offset_member=true);

    //为了读取某一个变量而获取一个寄存器
    virtual reg_index get_reg_for_reading_var(struct ic_data * var,enum reg_processor processor);

    //在把某一个变量写入寄存器并设置为脏值之前进行相应的处理
    virtual void before_writing_var_value(struct ic_data * var);

    //为了写某一个变量而获取一个寄存器
    virtual reg_index get_reg_for_writing_var(struct ic_data * var,enum reg_processor processor);

    //为了读取某一个变量的地址而获取一个寄存器
    virtual reg_index get_reg_for_var_addr(struct ic_data * var,enum reg_processor processor);

    //将一个指定的寄存器设置为新分配
    virtual bool allocate_designated_reg(reg_index reg);

    //为了某一个int常量的值而获取一个特定的寄存器
    virtual void get_designated_reg_for_const(reg_index reg,OAA const_data);

    //为了读取某一个变量而获取一个特定的寄存器
    virtual void get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var);

    //为了写某一个变量而获取一个特定的寄存器
    virtual void get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var);

    //事件处理
    virtual void handle_FUNC_DEFINE(struct ic_func * func);
    virtual void handle_START_FUNC(struct ic_func_flow_graph * func);
    virtual void handle_END_FUNC();
    virtual void handle_CLEAR_FLAG();
    virtual void handle_CHANGE_FLAGS_FOR_VAR(struct ic_data * var,int cared_flag);
    virtual void handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params);
    virtual void handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC(list<struct ic_data * > * r_params,list<reg_index> * r_param_regs);
    virtual void handle_RETURN_VAR(struct ic_data * var);
    virtual void handle_BEFORE_CALL_FUNC(struct ic_data * return_value);
    virtual void handle_START_INSRUCTION(set<reg_index> * regs_unaccessible);
    virtual void handle_END_INSTRUCTION();
    virtual void handle_FUNC_RET();
    virtual void handle_SAVE_REGS_WHEN_CALLING_FUNC(struct ic_func * func,list<struct ic_data * > * r_params);
    virtual void handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC(string func_name,list<struct ic_data * > * r_params);
    virtual void handle_RET_FROM_CALLED_FUNC(struct ic_func * func,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    virtual void handle_RET_FROM_CALLED_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    virtual void handle_ATTACH_CONST_TO_REG(OAA const_data,reg_index reg);
    virtual void handle_ATTACH_VAR_VALUE_TO_REG(struct ic_data * var_data,reg_index reg);
    virtual void handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(struct ic_data * var_data,reg_index reg);
    virtual void handle_UNATTACH_REG_S_ALL_DATA(reg_index reg);
    // virtual void handle_DISABLE_ALL_ADDR_REGS_RELATED_WITH_SP();
    virtual void handle_DEAL_WITH_PARMAS_IN_ARGUMENT_REGS();
    virtual struct event handle_GET_REG_BY_NAME(string name);
    virtual struct event handle_IS_REG_EFFECTIVE(reg_index reg);
    virtual struct event handle_GET_FUNC_S_F_PARAMS_IN_REGS(struct ic_func * func);
    virtual struct event handle_GET_CPU_REGS_HOLDING_F_PARAMS_WHEN_ENTERING_FUNC(struct ic_func * func);
    virtual struct event handle_GET_VFP_REGS_HOLDING_F_PARAMS_WHEN_ENTERING_FUNC(struct ic_func * func);
    virtual struct event handle_IS_CPU_REG(reg_index reg);
    virtual struct event handle_IS_VFP_REG(reg_index reg);
    virtual struct event handle_CHECK_CONST_VALUE_OWN_CPU_REG(OAA const_value);
    virtual struct event handle_GET_CONST_VALUE_S_CPU_REG(OAA const_value);
    virtual struct event handle_ALLOCATE_IDLE_CPU_REG();
    virtual struct event handle_GET_SP_REG();
    virtual struct event handle_GET_FP_REG();
    virtual struct event handle_GET_LR_REG();
    virtual struct event handle_GET_PC_REG();
    virtual struct event handle_GET_APSR_REG();
    virtual struct event handle_GET_FPSCR_REG();
    virtual struct event handle_GET_R0_REG();
    virtual struct event handle_GET_R1_REG();
    virtual struct event handle_GET_S0_REG();
    virtual struct event handle_IS_ARGUMENT_REG(reg_index reg);
    virtual struct event handle_IS_TEMP_REG(reg_index reg);
    virtual struct event handle_GET_REG_BYTE_SIZE(reg_index reg);
    virtual struct event handle_GET_ALL_ARGUMENT_REGS();
    virtual struct event handle_GET_CPU_ARGUMENT_REG_NUM();
    virtual struct event handle_GET_VFP_ARGUMENT_REG_NUM();
    virtual struct event handle_CHECK_VAR_ATTACHED_TO_FLAG(struct ic_data * var);
    virtual struct event handle_GET_VAR_CARED_FLAG();
    virtual struct event handle_GET_ALL_CONTEXT_SAVED_CPU_REGS(struct ic_func * func);
    virtual struct event handle_GET_ALL_CONTEXT_SAVED_VFP_REGS(struct ic_func * func);
    virtual struct event handle_GET_CPU_REG_FOR_CONST(OAA const_data);
    virtual struct event handle_GET_AN_EMPTY_CPU_REG_FOR_CONST(OAA const_data);
    virtual struct event handle_GET_VFP_REG_FOR_CONST(OAA const_data);
    virtual struct event handle_GET_CPU_REG_FOR_READING_VAR(struct ic_data * var_data);
    virtual struct event handle_GET_CPU_REG_FOR_WRITING_VAR(struct ic_data * var_data);
    virtual struct event handle_GET_VFP_REG_FOR_READING_VAR(struct ic_data * var_data);
    virtual struct event handle_GET_VFP_REG_FOR_WRITING_VAR(struct ic_data * var_data);
    virtual struct event handle_GET_ADDR_REG(struct ic_data * var_data);
    virtual void handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block)=0;
    virtual void handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block)=0;
    virtual void handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block)=0;

public:
    Register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    virtual ~Register_manager();

    //检查该寄存器管理员是否初始化成功
    bool is_init_successful();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__REGISTER_MANAGER_H