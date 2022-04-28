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
    NOT_USED,           //该寄存器没有被使用
    USED,               //该寄存器被使用了
    DIRTY_VALUE,        //该寄存器是脏值的（即寄存器的值和对应的变量的内存值不一致，寄存器的值是最新的）
};

//寄存器相关数据的类型
enum class reg_related_data_type
{
    NONE,
    VAR,
    ADDR,
    CONST_INT,
    CONST_FLOAT
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
    reg(string name,reg_index index,enum reg_attr attr,bool writable,enum reg_state state,size_t size,enum reg_processor processor):name(name),index(index),attr(attr),writable(writable),state(state),size(size),related_data_type(reg_related_data_type::NONE),processor(processor)
    {

    };

    inline bool is_allocating() const
    {
        return (state==reg_state::USED && related_data_type==reg_related_data_type::NONE);
    }

    inline void set_allocating()
    {
        state=reg_state::USED;
        related_data_type=reg_related_data_type::NONE;
    }

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
    enum reg_state state;       //状态
    size_t size;                //寄存器大小（单位是bit）
    enum reg_related_data_type related_data_type;         //相关数据的类型
    enum reg_processor processor;                           //所属的处理器
    union          //相关数据
    {
        struct ic_data * var_data;
        int const_int_data;
        float const_float_data;
    };
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
    flag_reg(string name,size_t flag_num_in_flag_reg,...):name(name)
    {
        /*va_list argptr;
        va_start(argptr,flag_num_in_flag_reg);
        for(size_t i=0;i<flag_num_in_flag_reg;i++)
        {
            related_data.insert(make_pair(va_arg(argptr,enum flag_in_flag_reg),nullptr));
        }
        va_end(argptr);*/
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

//寄存器管理器
class Register_manager:public Asm_generator_component
{

private:
    //有效位
    bool is_init_successful_;

    //寄存器信息
    struct regs
    {
        struct flag_reg flag_reg;                      //标志寄存器
        map<reg_index,struct reg> reg_indexs;          //寄存器编号和寄存器之间的映射
        map<string,reg_index> reg_names;               //寄存器名字和寄存器编号之间的映射

        regs():flag_reg("",0)
        {

        };
    } regs_;

    //当前函数的寄存器信息
    struct func_regs_info
    {
        list<reg_index> context_saved_regs;                     //当前函数用于保存上下文的寄存器
    } current_func_regs_info_;

    //当前基本块的寄存器信息
    struct basic_blocK_regs_info
    {
        map<struct ic_data *,set<reg_index> > var_value_regs_map;         //存储变量的值和寄存器之间的映射关系的map
        map<int,set<reg_index> > const_int_value_regs_map;                //存储int常数的值和寄存器之间的映射关系的map
        map<float,set<reg_index> > const_float_value_regs_map;                //存储float常数的值和寄存器之间的映射关系的map
        map<struct ic_data *,set<reg_index> > var_addr_regs_map;          //存储变量的地址和寄存器之间的映射关系的map

        //查看某个变量的值和某个寄存器之间是否有关系
        bool check_var_value_reg(struct ic_data * var,reg_index reg)
        {
            return (var_value_regs_map.find(var)!=var_value_regs_map.end() && var_value_regs_map.at(var).find(reg)!=var_value_regs_map.at(var).end());
        };

        //建立某个变量的值和某个寄存器之间的关系
        void build_var_value_reg(struct ic_data * var,reg_index reg)
        {
            set<reg_index> empty;
            if(var_value_regs_map.find(var)==var_value_regs_map.end())
            {
                var_value_regs_map.insert(make_pair(var,empty));
            }

            if(var_value_regs_map.at(var).find(reg)==var_value_regs_map.at(var).end())
            {
                var_value_regs_map.at(var).insert(reg);
            }
        };

        //解除某个变量的值和某个寄存器之间的关系
        void relieve_var_value_reg(struct ic_data * var,reg_index reg)
        {
            if(var_value_regs_map.find(var)!=var_value_regs_map.end())
            {
                if(var_value_regs_map.at(var).find(reg)!=var_value_regs_map.at(var).end())
                {
                    var_value_regs_map.at(var).erase(reg);
                }
                
                if(var_value_regs_map.at(var).empty())
                {
                    var_value_regs_map.erase(var);
                }
            }
        };

        //随机获取一个和某一个变量的值有关的寄存器
        reg_index randomly_get_var_value_reg(struct ic_data * var)
        {
            reg_index res;
            if(var_value_regs_map.find(var)!=var_value_regs_map.end() && !var_value_regs_map.at(var).empty())
            {
                res=*(var_value_regs_map.at(var).begin());
            }
            return res;
        };

        //查看某个int常量的值和某个寄存器之间是否有关系
        bool check_const_int_value_reg(int const_int,reg_index reg)
        {
            return (const_int_value_regs_map.find(const_int)!=const_int_value_regs_map.end() && const_int_value_regs_map.at(const_int).find(reg)!=const_int_value_regs_map.at(const_int).end());
        };

        //建立某个int常量的值和某个寄存器之间的关系
        void build_const_int_value_reg(int const_int,reg_index reg)
        {
            set<reg_index> empty;
            if(const_int_value_regs_map.find(const_int)==const_int_value_regs_map.end())
            {
                const_int_value_regs_map.insert(make_pair(const_int,empty));
            }
            
            if(const_int_value_regs_map.at(const_int).find(reg)==const_int_value_regs_map.at(const_int).end())
            {
                const_int_value_regs_map.at(const_int).insert(reg);
            }
        };

        //解除某个int常量的值和某个寄存器之间的关系
        void relieve_const_int_value_reg(int const_int,reg_index reg)
        {
            if(const_int_value_regs_map.find(const_int)!=const_int_value_regs_map.end())
            {
                if(const_int_value_regs_map.at(const_int).find(reg)!=const_int_value_regs_map.at(const_int).end())
                {
                    const_int_value_regs_map.at(const_int).erase(reg);
                }
                
                if(const_int_value_regs_map.at(const_int).empty())
                {
                    const_int_value_regs_map.erase(const_int);
                }
            }
        };

        //随机获取一个和某一个int常量的值有关的寄存器
        reg_index randomly_get_const_int_value_reg(int const_int)
        {
            reg_index res;
            if(const_int_value_regs_map.find(const_int)!=const_int_value_regs_map.end() && !const_int_value_regs_map.at(const_int).empty())
            {
                res=*(const_int_value_regs_map.at(const_int).begin());
            }
            return res;
        };

        //查看某个float常量的值和某个寄存器之间是否有关系
        bool check_const_float_value_reg(float const_float,reg_index reg)
        {
            return (const_float_value_regs_map.find(const_float)!=const_float_value_regs_map.end() && const_float_value_regs_map.at(const_float).find(reg)!=const_float_value_regs_map.at(const_float).end());
        };

        //建立某个int常量的值和某个寄存器之间的关系
        void build_const_float_value_reg(float const_float,reg_index reg)
        {
            set<reg_index> empty;
            if(const_float_value_regs_map.find(const_float)==const_float_value_regs_map.end())
            {
                const_float_value_regs_map.insert(make_pair(const_float,empty));
            }
            
            if(const_float_value_regs_map.at(const_float).find(reg)==const_float_value_regs_map.at(const_float).end())
            {
                const_float_value_regs_map.at(const_float).insert(reg);
            }
        };

        //解除某个float常量的值和某个寄存器之间的关系
        void relieve_const_float_value_reg(float const_float,reg_index reg)
        {
            if(const_float_value_regs_map.find(const_float)!=const_float_value_regs_map.end())
            {
                if(const_float_value_regs_map.at(const_float).find(reg)!=const_float_value_regs_map.at(const_float).end())
                {
                    const_float_value_regs_map.at(const_float).erase(reg);
                }
                
                if(const_float_value_regs_map.at(const_float).empty())
                {
                    const_float_value_regs_map.erase(const_float);
                }
            }
        };

        //随机获取一个和某一个float常量的值有关的寄存器
        reg_index randomly_get_const_float_value_reg(float const_float)
        {
            reg_index res;
            if(const_float_value_regs_map.find(const_float)!=const_float_value_regs_map.end() && !const_float_value_regs_map.at(const_float).empty())
            {
                res=*(const_float_value_regs_map.at(const_float).begin());
            }
            return res;
        };

        //查看某个变量的地址和某个寄存器之间是否有关系
        bool check_var_addr_reg(struct ic_data * var,reg_index reg)
        {
            return (var_addr_regs_map.find(var)!=var_addr_regs_map.end() && var_addr_regs_map.at(var).find(reg)!=var_addr_regs_map.at(var).end());
        };

        //建立某个变量的地址和某个寄存器之间的关系
        void build_var_addr_reg(struct ic_data * var,reg_index reg)
        {
            set<reg_index> empty;
            if(var_addr_regs_map.find(var)==var_addr_regs_map.end())
            {
                var_addr_regs_map.insert(make_pair(var,empty));
            }

            if(var_addr_regs_map.at(var).find(reg)==var_addr_regs_map.at(var).end())
            {
                var_addr_regs_map.at(var).insert(reg);
            }
        };

        //解除某个变量的地址和某个寄存器之间的关系
        void relieve_var_addr_reg(struct ic_data * var,reg_index reg)
        {
            if(var_addr_regs_map.find(var)!=var_addr_regs_map.end())
            {
                if(var_addr_regs_map.at(var).find(reg)!=var_addr_regs_map.at(var).end())
                {
                    var_addr_regs_map.at(var).erase(reg);
                }
                
                if(var_addr_regs_map.at(var).empty())
                {
                    var_addr_regs_map.erase(var);
                }
            }
        };

        //随机获取一个和某一个变量的地址有关的寄存器
        reg_index randomly_get_var_addr_reg(struct ic_data * var)
        {
            reg_index res;
            if(var_addr_regs_map.find(var)!=var_addr_regs_map.end() && !var_addr_regs_map.at(var).empty())
            {
                res=*(var_addr_regs_map.at(var).begin());
            }
            return res;
        };
    } current_basic_block_info_;

    //当前正在生成的指令所用到的寄存器的信息
    struct instructions_regs_info
    {
        list<set<reg_index> > involved_regs;             //存储当前的指令所用到的寄存器
    } current_instructions_regs_info_;

    //把当前的寄存器信息全部清空
    void clear();

    //将一个指定的寄存器设置为新分配
    bool allocate_designated_reg(reg_index reg);

    //新分配一个空闲的寄存器
    reg_index allocate_idle_reg(reg_processor processor);

    //将某一个寄存器设置为正在被当前的指令所使用
    void set_got_by_current_instruction(reg_index reg);

    //查看一个寄存器是否正在被当前的指令所使用
    bool is_got_by_current_instruction(reg_index reg);

    //设置某一个寄存器的新的状态为NOT_USED
    bool set_reg_NOT_USED(reg_index reg);

    //把某一个寄存器设置为被某一个变量的值所使用
    bool set_reg_USED_for_value(reg_index reg,struct ic_data * var_data);

    //把某一个寄存器设置为被一个int常数所使用
    bool set_reg_USED_for_value(reg_index reg,int const_int_data);

    //把某一个寄存器设置为被一个float常数所使用
    bool set_reg_USED_for_value(reg_index reg,float const_float_data);

    //把某一个寄存器设置为被某一个变量的地址所使用
    bool set_reg_USED_for_addr(reg_index reg,struct ic_data * var_data);

    //设置某一个寄存器的新的状态为DIRTY_VALUE
    bool set_reg_DIRTY_VALUE(reg_index reg);

    //把一个DIRTY_VALUE寄存器设置为USED
    bool set_reg_USED(reg_index reg);

    //查看某一个int常数是否占有一个寄存器来存储它的值
    bool check_const_own_value_reg(int const_int);

    //查看某一个float常数是否占有一个寄存器来存储它的值
    bool check_const_own_value_reg(float const_float);

    //查看某一个变量是否占有一个寄存器来存储它的值
    bool check_var_own_value_reg(struct ic_data * var);

    //查看某一个变量是否占有一个寄存器来存储它的地址
    bool check_var_own_addr_reg(struct ic_data * var);

    //为某一个int常量获取一个寄存器
    reg_index get_reg_for_const(int const_int);

    //为某一个float常量获取一个寄存器
    reg_index get_reg_for_const(float const_float);

    //为了读取某一个变量而获取一个寄存器
    reg_index get_reg_for_reading_var(struct ic_data * var);

    //为了写某一个变量而获取一个寄存器
    reg_index get_reg_for_writing_var(struct ic_data * var);

    //为了读取某一个变量的地址而获取一个寄存器
    reg_index get_reg_for_var_addr(struct ic_data * var);

    //为了某一个int常量的值而获取一个特定的寄存器
    bool get_designated_reg_for_const(reg_index reg,int const_int);

    //为了某一个float常量的值而获取一个特定的寄存器
    bool get_designated_reg_for_const(reg_index reg,float const_float);

    //为了读取某一个变量而获取一个特定的寄存器
    bool get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var);

    //为了写某一个变量而获取一个特定的寄存器
    bool get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var);

    //事件处理
    void handle_FUNC_DEFINE(struct ic_func * func);
    void handle_END_FUNC();
    struct event handle_READY_TO_PUSH_LR_AND_FP_REGS(struct ic_func * func);
    struct event handle_READY_TO_PUSH_CONTEXT_SAVED_TEMP_CPU_REGS(struct ic_func * func);
    struct event handle_READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS(struct ic_func * func);
    struct event handle_GET_SP_REG();
    struct event handle_GET_FP_REG();
    struct event handle_GET_LR_REG();
    struct event handle_GET_PC_REG();
    struct event handle_IS_CPU_REG(reg_index reg);
    struct event handle_IS_VFP_REG(reg_index reg);
    struct event handle_IS_ARGUMENT_REG(reg_index reg);
    struct event handle_IS_TEMP_REG(reg_index reg);
    struct event handle_GET_REG_BYTE_SIZE(reg_index reg);
    struct event handle_READY_TO_PUSH_F_PARAM_CPU_REGS(list<struct ic_data * > * f_params);
    struct event handle_READY_TO_PUSH_F_PARAM_VFP_REGS(list<struct ic_data * > * f_params);
    void handle_END_BASIC_BLOCK();
    void handle_END_BASIC_BLOCK_WITHOUT_FLAG();
    void handle_CLEAR_FLAG();
    void handle_RETURN_VAR(struct ic_data * var);
    struct event handle_GET_REG_FOR_INT_CONST(int const_int_data);
    struct event handle_GET_REG_FOR_READING_VAR(struct ic_data * var_data);
    struct event handle_GET_REG_FOR_WRITING_VAR(struct ic_data * var_data);
    struct event handle_GET_ADDR_REG(struct ic_data * var_data);
    void handle_START_INSRUCTION(set<reg_index> * regs_unaccessible);
    void handle_END_INSTRUCTION();
    void handle_FUNC_RET();
    void handle_SAVE_REGS_WHEN_CALLING_FUNC();
    void handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC();
    void handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params);
    void handle_RET_FROM_CALLED_FUNC(struct ic_data * return_value,reg_index return_reg);
    void handle_RET_FROM_CALLED_ABI_FUNC(struct ic_data * return_value,reg_index return_reg);
    struct event handle_CHECK_VAR_OWN_VALUE_REG(struct ic_data * var);
    struct event handle_CHECK_CONST_INT_OWN_VALUE_REG(int const_int);
    struct event handle_GET_VAR_S_VALUE_REG(struct ic_data * var);
    struct event handle_GET_CONST_INT_S_VALUE_REG(int const_int);
    struct event handle_GET_R0_REG();
    struct event handle_GET_R1_REG();
    struct event handle_GET_S0_REG();
    void handle_CHANGE_FLAGS_FOR_VAR(struct ic_data * var,int cared_flag);
    struct event handle_CHECK_VAR_ATTACHED_TO_FLAG(struct ic_data * var);
    struct event handle_GET_VAR_CARED_FLAG();
    struct event handle_ALLOCATE_IDLE_CPU_REG();
    void handle_ATTACH_CONST_INT_TO_REG(int const_int,reg_index reg);

public:
    //构造函数
    Register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    //检查该寄存器管理员是否初始化成功
    bool is_init_successful();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__REGISTER_MANAGER_H