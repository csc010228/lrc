/*
 *
 *  global_register_manager.h
 *  全局寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/6
 *
*/
#ifndef __GLOBAL_REGISTER_MANAGER_H
#define __GLOBAL_REGISTER_MANAGER_H

#include"register_manager.h"
#include"arm_asm.h"

//虚拟寄存器存储的数据种类
enum class virtual_related_data_type
{
    CONST,                  //常量
    VAR_VALUE,              //变量值
    VAR_ADDR                //变量地址
};

//虚拟寄存器
struct virtual_reg
{
    virtual_reg(enum reg_processor processor):is_effective(false),is_var_value_drity(false),processor(processor),data_type(virtual_related_data_type::CONST)
    {

    };

    //设置改虚拟寄存器相关的数据
    void set_related_data(OAA const_value);

    //设置改虚拟寄存器相关的数据
    void set_related_data(struct ic_data * var,enum virtual_related_data_type value_ro_addr);

    union
    {
        struct ic_data * related_var;              //和该虚拟寄存器相关的变量
        OAA related_const;                          //和该虚拟寄存器相关的常量
    };
    bool is_effective;                                          //该虚拟寄存器此时是否有效
    bool is_var_value_drity;                                    //该虚拟寄存器中存放的变量的值是否是脏值
    enum reg_processor processor;                               //该虚拟寄存器所属的处理器
    enum virtual_related_data_type data_type;                   //该虚拟寄存器相关的变量的状态
};

//虚拟寄存器堆
struct virtual_regs_info
{
    map<reg_index,struct virtual_reg> reg_indexs;                           //虚拟寄存器以及编号
    map<enum reg_processor,map<OAA,reg_index> > const_value_to_reg_map;                  //常量的值和虚拟寄存器之间的映射
    map<enum reg_processor,map<struct ic_data *,reg_index> > var_value_to_reg_map;       //变量的值和虚拟寄存器之间的映射
    map<enum reg_processor,map<struct ic_data *,reg_index> > var_addr_to_reg_map;        //变量的地址和虚拟寄存器之间的映射
    reg_index max_physical_reg_index;                                       //物理寄存器堆中最大的寄存器编号
    reg_index next_virtaul_reg_index;                                       //下一个要分配的虚拟寄存器编号
    map<enum reg_processor,set<reg_index> > new_temps;                      //在本次溢出的时候生成的新的寄存器

    virtual_regs_info():max_physical_reg_index(0),next_virtaul_reg_index(0)
    {

    };

    virtual_regs_info(reg_index max_physical_reg_index):max_physical_reg_index(max_physical_reg_index),next_virtaul_reg_index(max_physical_reg_index+1)
    {

    };

    //清空虚拟寄存器堆
    void clear();

    //将所有的虚拟寄存器设置为无效
    void set_all_regs_ineffective();

    //分配一个新的虚拟寄存器
    reg_index allocate_reg(enum reg_processor processor);

    //查看一个常量的值是否占有一个虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_const_value_own_a_reg(OAA const_value,enum reg_processor processor) const;

    //查看一个常量的值是否占有任何虚拟寄存器
    bool check_const_value_own_regs(OAA const_value,bool effective=true) const;

    //查看一个变量的值是否占有一个虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_var_value_own_a_reg(struct ic_data * var,enum reg_processor processor) const;

    //查看一个变量的值是否占有任何虚拟寄存器
    bool check_var_value_own_regs(struct ic_data * var,bool effective=true) const;

    //查看一个变量的地址是否占有一个虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_var_addr_own_a_reg(struct ic_data * var,enum reg_processor processor) const;

    //查看一个常量的值是否占有一个指定的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_const_value_own_the_reg(OAA const_value,reg_index reg) const;

    //查看一个变量的值是否占有一个指定的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_var_value_own_the_reg(struct ic_data * var,reg_index reg) const;

    //查看一个变量的地址是否占有一个指定的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    bool check_var_addr_own_the_reg(struct ic_data * var,reg_index reg) const;

    //获取一个常量占有的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    reg_index get_const_value_s_reg(OAA const_value,enum reg_processor processor) const;

    //获取一个常量占有的所有虚拟寄存器
    set<reg_index> get_const_value_s_regs(OAA const_value,bool effective=true) const;
    
    //获取一个变量的值占有的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    reg_index get_var_value_s_reg(struct ic_data * var,enum reg_processor processor) const;

    //获取一个变量的值占有的所有虚拟寄存器
    set<reg_index> get_var_value_s_regs(struct ic_data * var,bool effective=true) const;

    //获取一个变量的地址占有的虚拟寄存器（对应的虚拟寄存器此时不一定有效）
    reg_index get_var_addr_s_reg(struct ic_data * var,enum reg_processor processor) const;

    //把一个虚拟寄存器和常数进行关联（关联之后的虚拟寄存器中的数据是有效的）
    void attach_const_value_to_reg(OAA const_value,reg_index reg);

    //把一个虚拟寄存器和变量的值进行关联（关联之后的虚拟寄存器中的数据是有效的）
    void attach_var_value_to_reg(struct ic_data * var_value,reg_index reg);

    //把一个虚拟寄存器和变量的地址进行关联（关联之后的虚拟寄存器中的数据是有效的）
    void attach_var_addr_to_reg(struct ic_data * var,reg_index reg);
    
    //查看一个虚拟寄存器此时是否有效
    bool is_reg_effective(reg_index reg) const;

    //把一个虚拟寄存器设置为无效（但是该虚拟寄存器仍旧和它的数据绑定）
    void set_reg_ineffective(reg_index reg);

    //把一个虚拟寄存器设置为有效（但是该虚拟寄存器仍旧和它的数据绑定）
    void set_reg_effective(reg_index reg);

    //查看一个虚拟寄存器中存储的值是否是脏值
    bool is_reg_DIRTY(reg_index reg) const;

    //把一个寄存器中的某一个变量值设置为脏值（如果此时对应的虚拟寄存器是无效的，自动将其设置为有效）
    void set_reg_s_value_DIRTY(reg_index reg);

    //把一个寄存器中的某一个变量值设置为非脏值（如果此时对应的虚拟寄存器是无效的，自动将其设置为有效）
    void set_reg_s_value_NOT_DIRTY(reg_index reg);
    
    //获取一个寄存器所属的处理器
    enum reg_processor get_reg_s_processor(reg_index reg) const;

    //查看一个寄存器是不是物理寄存器
    bool is_physical_reg(reg_index reg) const;

    //在溢出的时候分配新的虚拟寄存器
    reg_index new_temp_for_const(OAA const_data,enum reg_processor processor);
    reg_index new_temp_for_var_value(struct ic_data * var,enum reg_processor processor);
    reg_index new_temp_for_var_addr(struct ic_data * var,enum reg_processor processor);

    //查看某一个寄存器是否是溢出的时候新分配的寄存器
    bool is_reg_new_temp(reg_index reg) const;
};

class Global_register_manager:public Register_manager
{
protected:
    //虚拟寄存器堆
    struct virtual_regs_info virtual_regs_info_;

    //新分配一个空闲的寄存器
    reg_index allocate_idle_reg(reg_processor processor,bool get_an_empty_reg=false);

    //为某一个常量获取一个寄存器
    reg_index get_reg_for_const(OAA const_data,enum reg_processor processor,bool get_an_empty_reg=false);

    //把一个数组的所有脏值元素全部写回内存，并且将其设置为非脏值
    void store_array_s_all_DIRTY_members_and_set_them_NOT_DIRTY(struct ic_data * array,bool deal_with_const_offset_member=true);

    //为了读取某一个变量而获取一个寄存器
    reg_index get_reg_for_reading_var(struct ic_data * var,enum reg_processor processor);

    //在把某一个变量写入寄存器并设置为脏值之前进行相应的处理
    void before_writing_var_value(struct ic_data * var);

    //为了写某一个变量而获取一个寄存器
    reg_index get_reg_for_writing_var(struct ic_data * var,enum reg_processor processor);

    //为了读取某一个变量的地址而获取一个寄存器
    reg_index get_reg_for_var_addr(struct ic_data * var,enum reg_processor processor);

    //为了某一个int常量的值而获取一个特定的寄存器
    void get_designated_reg_for_const(reg_index reg,OAA const_data);

    //为了读取某一个变量而获取一个特定的寄存器
    void get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var);

    //为了写某一个变量而获取一个特定的寄存器
    void get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var);

    //事件处理
    void handle_START_FUNC(struct ic_func_flow_graph * func);
    void handle_END_FUNC();
    void handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block);
    void handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block);
    void handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block);
    void handle_FUNC_RET();
    void handle_SAVE_REGS_WHEN_CALLING_FUNC(struct ic_func * func,list<struct ic_data * > * r_params);
    void handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC(string func_name,list<struct ic_data * > * r_params);
    void handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params);
    void handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC(list<struct ic_data * > * r_params,list<reg_index> * r_param_regs);
    void handle_RET_FROM_CALLED_FUNC(struct ic_func * func,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    void handle_RET_FROM_CALLED_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    void handle_ATTACH_CONST_TO_REG(OAA const_data,reg_index reg);
    void handle_ATTACH_VAR_VALUE_TO_REG(struct ic_data * var_data,reg_index reg);
    void handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(struct ic_data * var_data,reg_index reg);
    void handle_UNATTACH_REG_S_ALL_DATA(reg_index reg);
    // void handle_DISABLE_ALL_ADDR_REGS_RELATED_WITH_SP();
    void handle_DEAL_WITH_PARMAS_IN_ARGUMENT_REGS();
    struct event handle_IS_CPU_REG(reg_index reg);
    struct event handle_IS_VFP_REG(reg_index reg);
    struct event handle_CHECK_CONST_VALUE_OWN_CPU_REG(OAA const_value);
    struct event handle_GET_CONST_VALUE_S_CPU_REG(OAA const_value);
    struct event handle_ALLOCATE_IDLE_CPU_REG();
    struct event handle_GET_CPU_REG_FOR_CONST(OAA const_data);
    struct event handle_GET_AN_EMPTY_CPU_REG_FOR_CONST(OAA const_data);
    struct event handle_GET_VFP_REG_FOR_CONST(OAA const_data);
    struct event handle_GET_CPU_REG_FOR_READING_VAR(struct ic_data * var_data);
    struct event handle_GET_CPU_REG_FOR_WRITING_VAR(struct ic_data * var_data);
    struct event handle_GET_VFP_REG_FOR_READING_VAR(struct ic_data * var_data);
    struct event handle_GET_VFP_REG_FOR_WRITING_VAR(struct ic_data * var_data);
    struct event handle_GET_ADDR_REG(struct ic_data * var_data);
    struct event handle_IS_REG_EFFECTIVE(reg_index reg);

public:
    Global_register_manager(set<struct reg> regs,struct flag_reg flag_reg);

    ~Global_register_manager();
};

#endif //__GLOBAL_REGISTER_MANAGER_H