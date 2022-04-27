/*
 *
 *  register_manager.cpp
 *  寄存器管理器
 *  Creator:Sichao Chen
 *  Create time:2022/1/27
 *
*/
#include "register_manager.h"

/*
构造函数

Parameters
----------
regs:目标机器的寄存器信息
flag_reg:标志寄存器的信息
*/
Register_manager::Register_manager(set<struct reg> regs,struct flag_reg flag_reg)
{
    regs_.flag_reg=flag_reg;
    for(auto i:regs)
    {
        regs_.reg_indexs.insert(make_pair(i.index,i));
        regs_.reg_names.insert(make_pair(i.name,i.index));
    }

    is_init_successful_=true;
}

/*
检查该寄存器管理员是否初始化成功
*/
bool Register_manager::is_init_successful()
{
    return is_init_successful_;
}

/*
把当前的寄存器信息全部清空
*/
void Register_manager::clear()
{
    for(auto i:regs_.reg_indexs)
    {
        i.second.state=reg_state::NOT_USED;
        i.second.related_data_type=reg_related_data_type::NONE;
    }
    current_basic_block_info_.const_int_value_regs_map.clear();
    current_basic_block_info_.const_float_value_regs_map.clear();
    current_basic_block_info_.var_addr_regs_map.clear();
    current_basic_block_info_.var_value_regs_map.clear();
    current_instructions_regs_info_.involved_regs.clear();
    regs_.flag_reg.related_data.first=nullptr;
}

/*
将一个指定的寄存器设置为新分配

Parameters
----------
reg:要获取的寄存器编号

Return
------
如果获取成功就返回true，否则返回false
*/
bool Register_manager::allocate_designated_reg(reg_index reg)
{
    bool res=false;
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    pair<struct ic_data *,reg_index> * event_data;
    enum reg_related_data_type related_data_type;
    enum reg_state state;
    struct ic_data * var;
    int const_int_data;
    float const_float_data;
    //如果要分配的寄存器正在被目前所生成的指令所使用，那么就无法将其获取
    if(is_got_by_current_instruction(reg))
    {
        return res;
    }
    //如果当前要分配的寄存器正在被分配，也无法分配
    if(!designated_reg.is_allocating())
    {
        related_data_type=designated_reg.related_data_type;
        state=designated_reg.state;
        var=designated_reg.var_data;
        const_int_data=designated_reg.const_int_data;
        const_float_data=designated_reg.const_float_data;
        //如果一个寄存器的状态是USED，但是关联的数据是NONE，那么该寄存器正在被分配
        designated_reg.set_allocating();
        
        if(related_data_type==reg_related_data_type::CONST_INT)
        {
            current_basic_block_info_.relieve_const_int_value_reg(const_int_data,reg);
        }
        else if(related_data_type==reg_related_data_type::CONST_FLOAT)
        {
            current_basic_block_info_.relieve_const_float_value_reg(const_float_data,reg);
        }
        else if(related_data_type==reg_related_data_type::VAR)
        {
            if(var->is_tmp_var())
            {
                //如果此时要被分配的寄存器中原本存放在临时变量的值的话，那么需要先将其入栈保存
                event_data=new pair<struct ic_data *,reg_index>;
                event_data->first=var;
                event_data->second=reg;
                notify(event(event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK,(void *)event_data));
                delete event_data;
            }
            else if(state==reg_state::DIRTY_VALUE)
            {
                //如果此时要获取的寄存器中存储的变量值是脏值，那么就要把该寄存器写回到内存中
                event_data=new pair<struct ic_data *,reg_index>(var,reg);
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
            }
            current_basic_block_info_.relieve_var_value_reg(var,reg);
        }
        else if(related_data_type==reg_related_data_type::ADDR)
        {
            current_basic_block_info_.relieve_var_addr_reg(var,reg);
        }

        res=true;
    }

    return res;
}

/*
新分配一个空闲的寄存器

Return
------
返回获取到的寄存器的编号
*/
reg_index Register_manager::allocate_idle_reg(reg_processor processor)
{
    reg_index res;
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.processor==processor && allocate_designated_reg(i.first))
        {
            res=i.first;
            break;
        }
    }
    return res;
}

/*
将某一个寄存器设置为正在被当前的指令所使用

Parameters
----------
reg:要获取的寄存器
*/
void Register_manager::set_got_by_current_instruction(reg_index reg)
{
    if(current_instructions_regs_info_.involved_regs.size()>0)
    {
        current_instructions_regs_info_.involved_regs.front().insert(reg);
    }
}

/*
检查一个寄存器是否正在被当前的指令所使用

Parameters
----------
reg:要检查的寄存器

Return
------
如果是的话，返回true，否则返回false
*/
bool Register_manager::is_got_by_current_instruction(reg_index reg)
{
    for(auto i:current_instructions_regs_info_.involved_regs)
    {
        if(i.find(reg)!=i.end())
        {
            return true;
        }
    }
    return false;
}

/*
设置某一个寄存器为NOT_USED

Parameters
----------
reg:要设置的寄存器

Return
------
如果设置成功返回true，否则返回false
*/
bool Register_manager::set_reg_NOT_USED(reg_index reg)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.state!=reg_state::NOT_USED)
    {
        allocate_designated_reg(reg);
        designated_reg.state=reg_state::NOT_USED;
        designated_reg.related_data_type=reg_related_data_type::NONE;
    }
    return true;
}

/*
把若干个寄存器设置为被某一个变量的值所使用

Parameters
----------
regs:要设置的寄存器
var_data:要设置的变量

Return
------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_USED_for_value(reg_index reg,struct ic_data * var_data)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.is_allocating())
    {
        //只能将正在分配的寄存器设置为USED
        designated_reg.state=reg_state::USED;
        designated_reg.related_data_type=reg_related_data_type::VAR;
        designated_reg.var_data=var_data;
        current_basic_block_info_.build_var_value_reg(var_data,reg);
        return true;
    }
    return false;
    
}

/*
把某一个寄存器设置为被一个int常数的值所使用

Parameters
----------
reg:要设置的寄存器
const_int_data:要设置的int常数值

Return
------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_USED_for_value(reg_index reg,int const_int_data)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.is_allocating())
    {
        //只能将正在分配的寄存器设置为USED
        designated_reg.state=reg_state::USED;
        designated_reg.related_data_type=reg_related_data_type::CONST_INT;
        designated_reg.const_int_data=const_int_data;
        current_basic_block_info_.build_const_int_value_reg(const_int_data,reg);
        return true;
    }
    return false;
}

/*
把某一个寄存器设置为被一个float常数的值所使用

Parameters
----------
reg:要设置的寄存器
const_float_data:要设置的float常数值

Return
------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_USED_for_value(reg_index reg,float const_float_data)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.is_allocating())
    {
        //只能将正在分配的寄存器设置为USED
        designated_reg.state=reg_state::USED;
        designated_reg.related_data_type=reg_related_data_type::CONST_FLOAT;
        designated_reg.const_float_data=const_float_data;
        current_basic_block_info_.build_const_float_value_reg(const_float_data,reg);
        return true;
    }
    return false;
}

/*
把若干个寄存器设置为被某一个变量的地址所使用

Parameters
----------
regs:要设置的寄存器
var_data:要设置的变量

Return
------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_USED_for_addr(reg_index reg,struct ic_data * var_data)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.is_allocating())
    {
        //只能将正在分配的寄存器设置为USED
        designated_reg.state=reg_state::USED;
        designated_reg.related_data_type=reg_related_data_type::ADDR;
        designated_reg.var_data=var_data;
        current_basic_block_info_.build_var_addr_reg(var_data,reg);
        return true;
    }
    return false;
}

/*
设置某一个寄存器的新的状态为DIRTY_VALUE

Parameters
----------
reg:要设置的寄存器

Return
------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_DIRTY_VALUE(reg_index reg)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    bool res=true;
    if(designated_reg.state==reg_state::USED && designated_reg.related_data_type==reg_related_data_type::VAR)
    {
        designated_reg.state=reg_state::DIRTY_VALUE;
    }
    else
    {
        res=false;
    }
    return res;
}

/*
把一个DIRTY_VALUE寄存器设置为USED
也就是把该脏值寄存器的内容写回到内存中，但是并不改变该寄存器的内容，实现内存和寄存器的数据同步

Parameters
----------
reg:要设置为USED的DIRTY_VALUE寄存器

Reutnrn
-------
如果设置成功，就返回true，否则返回false
*/
bool Register_manager::set_reg_USED(reg_index reg)
{
    bool res=false;
    struct reg & DIRTY_VALUE_reg=regs_.reg_indexs.at(reg);
    struct ic_data * var;
    if(DIRTY_VALUE_reg.state==reg_state::DIRTY_VALUE)
    {
        var=DIRTY_VALUE_reg.var_data;
        allocate_designated_reg(reg);
        DIRTY_VALUE_reg.state=reg_state::USED;
        DIRTY_VALUE_reg.related_data_type=reg_related_data_type::VAR;
        DIRTY_VALUE_reg.var_data=var;
        current_basic_block_info_.build_var_value_reg(var,reg);
        res=true;
    }
    return res;
}

/*
查看某一个int常数是否占有一个寄存器来存储它的值

Parameters
----------
const_int:要检查的int常数值

Return
------
如果是的话返回true，并随机返回一个对应的寄存器编号，反之返回false
*/
bool Register_manager::check_const_own_value_reg(int const_int)
{
    bool res=false;
    if(current_basic_block_info_.const_int_value_regs_map.find(const_int)!=current_basic_block_info_.const_int_value_regs_map.end())
    {
        for(auto i:current_basic_block_info_.const_int_value_regs_map.at(const_int))
        {
            if(!regs_.reg_indexs.at(i).is_allocating())
            {
                res=true;
                break;
            }
        }
    }
    return res;
}

/*
查看某一个float常数是否占有一个寄存器来存储它的值

Parameters
----------
const_float:要检查的float常数值

Return
------
如果是的话返回true，并随机返回一个对应的寄存器编号，反之返回false
*/
bool Register_manager::check_const_own_value_reg(float const_float)
{
    bool res=false;
    if(current_basic_block_info_.const_float_value_regs_map.find(const_float)!=current_basic_block_info_.const_float_value_regs_map.end())
    {
        for(auto i:current_basic_block_info_.const_float_value_regs_map.at(const_float))
        {
            if(!regs_.reg_indexs.at(i).is_allocating())
            {
                res=true;
                break;
            }
        }
    }
    return res;
}

/*
查看某一个变量是否占有一个寄存器来存储它的值

Parameters
----------
var:要检查的变量

Return
------
如果是的话返回true，并随机返回一个对应的寄存器编号，反之返回false
*/
bool Register_manager::check_var_own_value_reg(struct ic_data * var)
{
    bool res=false;
    if(current_basic_block_info_.var_value_regs_map.find(var)!=current_basic_block_info_.var_value_regs_map.end())
    {
        for(auto i:current_basic_block_info_.var_value_regs_map.at(var))
        {
            if(!regs_.reg_indexs.at(i).is_allocating())
            {
                res=true;
                break;
            }
        }
    }
    return res;
}

/*
查看某一个变量是否占有一个寄存器来存储它的地址

Parameters
----------
var:要检查的变量

Return
------
如果是的话返回true，并随机返回一个对应的寄存器编号，反之返回false
*/
bool Register_manager::check_var_own_addr_reg(struct ic_data * var)
{
    bool res=false;
    if(current_basic_block_info_.var_addr_regs_map.find(var)!=current_basic_block_info_.var_addr_regs_map.end())
    {
        for(auto i:current_basic_block_info_.var_addr_regs_map.at(var))
        {
            if(!regs_.reg_indexs.at(i).is_allocating())
            {
                res=true;
                break;
            }
        }
    }
    return res;
}

/*
为某一个int常量获取一个寄存器

Parameters
----------
const_int:要获取的常数值

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_const(int const_int)
{
    pair<int,reg_index> * event_data;
    reg_index reg;

    //首先检查此时是否有寄存器中已经存放了这个常数，如果有的话直接返回即可
    if(!check_const_own_value_reg(const_int))
    {
        //如果没有的话，那么就给该常数分配一个新的寄存器，并将该常数的值写入即可
        reg=allocate_idle_reg(reg_processor::CPU);
        event_data=new pair<int,reg_index>(const_int,reg);
        notify(event(event_type::WRITE_CONST_INT_TO_REG,(void *)event_data));
        delete event_data;
        set_reg_USED_for_value(reg,const_int);
    }
    else
    {
        reg=current_basic_block_info_.randomly_get_const_int_value_reg(const_int);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);

    return reg;
}

/*
为某一个float常量获取一个寄存器

Parameters
----------
const_int:要获取的常数值

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_const(float const_float)
{
    pair<float,reg_index> * event_data;
    reg_index reg;

    //首先检查此时是否有寄存器中已经存放了这个常数，如果有的话直接返回即可
    if(!check_const_own_value_reg(const_float))
    {
        //如果没有的话，那么就给该常数分配一个新的寄存器，并将该常数的值写入即可
        reg=allocate_idle_reg(reg_processor::VFP);
        event_data=new pair<float,reg_index>(const_float,reg);
        notify(event(event_type::WRITE_CONST_FLOAT_TO_REG,(void *)event_data));
        delete event_data;
        set_reg_USED_for_value(reg,const_float);
    }
    else
    {
        reg=current_basic_block_info_.randomly_get_const_float_value_reg(const_float);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);

    return reg;
}

/*
为了读取某一个变量而获取一个寄存器

Parameters
----------
var:要读取的变量

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_reading_var(struct ic_data * var)
{
    reg_index reg;
    pair<struct ic_data *,reg_index> * event_data;

    if(var->is_const() && !var->is_array_var())
    {
        switch(var->get_data_type())
        {
            case language_data_type::INT:
                return get_reg_for_const(var->get_value().int_data);
                break;
            case language_data_type::FLOAT:
                return get_reg_for_const(var->get_value().float_data);
                break;
            default:
                break;
        }
    }

    //首先检查此时是否有寄存器中已经存放了这个变量，如果有的话直接返回即可
    if(!check_var_own_value_reg(var))
    {
        //如果没有的话，那么就给该变量分配一个新的寄存器，并将该变量的值写入即可
        if(var->get_data_type()==language_data_type::INT || var->is_array_var())
        {
            reg=allocate_idle_reg(reg_processor::CPU);
        }
        else if(var->get_data_type()==language_data_type::FLOAT)
        {
            reg=allocate_idle_reg(reg_processor::VFP);
        }
        event_data=new pair<struct ic_data *,reg_index>;
        event_data->first=var;
        event_data->second=reg;
        notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data));
        delete event_data;
        set_reg_USED_for_value(reg,var);
    }
    else
    {
        reg=current_basic_block_info_.randomly_get_var_value_reg(var);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return reg;
}

/*
为了写某一个变量而获取一个寄存器

Parameters
----------
var:要写的变量

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_writing_var(struct ic_data * var)
{
    reg_index reg;
    
    //在对某一个变量进行更改之前，需要遍历此时所有的DIRTY_VALUE寄存器，查看其中的变量值是否和当前要更改的变量有关，如果有关的话，需要先将其写回，并将其置为NOT_USED
    //例如有一个二维数组是a[b][]，这在函数形参中是被允许的
    //那么此时假设要更改的变量是b，而此时的某一个DIRTY_VALUE寄存器中存放着a[2][4]
    //那么当b被更改完之后，a[2][4]可能就不再指向原本的值了，因此在b更改之前必须先将其写回
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.state==reg_state::DIRTY_VALUE && i.second.var_data->check_ic_data_related(var) && i.second.var_data!=var)
        {
            set_reg_NOT_USED(i.first);
        }
    }

    //查看此时是否有寄存器已经存放了该变量的值
    if(!check_var_own_value_reg(var))
    {
        //如果没有的话，那么需要新分配一个空闲寄存器，置为脏值即可
        if(var->get_data_type()==language_data_type::INT || var->is_array_var())
        {
            reg=allocate_idle_reg(reg_processor::CPU);
        }
        else if(var->get_data_type()==language_data_type::FLOAT)
        {
            reg=allocate_idle_reg(reg_processor::VFP);
        }
        set_reg_USED_for_value(reg,var);
    }
    else
    {
        //如果有的话，那么只需要从这些寄存器中选一个，并将其设置为脏值即可，而其他剩余的寄存器只需要被设置为无效，无需写回
        reg=current_basic_block_info_.randomly_get_var_value_reg(var);
        for(auto i:current_basic_block_info_.var_value_regs_map.at(var))
        {
            if(reg!=i)
            {
                regs_.reg_indexs.at(i).state=reg_state::NOT_USED;
                regs_.reg_indexs.at(i).related_data_type=reg_related_data_type::NONE;
            }
        }
        current_basic_block_info_.var_value_regs_map.at(var).clear();
        current_basic_block_info_.build_var_value_reg(var,reg);
    }
    //目前，我们把存放临时变量的寄存器全部都设置为USED，哪怕是要写这个临时变量也不设置为DIRTY_VALUE
    if(!var->is_tmp_var() && !var->is_const())
    {
        set_reg_DIRTY_VALUE(reg);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return reg;
}

/*
为了读取某一个变量的地址而获取一个寄存器

Parameters
----------
var:要获取地址的变量

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_var_addr(struct ic_data * var)
{
    pair<struct ic_data *,reg_index> * event_data;
    reg_index reg;
    //如果该变量已经占有了寄存器，那么随机返回一个即可
    if(!check_var_own_addr_reg(var))
    {
        //否则的话，分配一个新的空闲寄存器，将地址写入该寄存器
        reg=allocate_idle_reg(reg_processor::CPU);
        event_data=new pair<struct ic_data *,reg_index>(var,reg);
        notify(event(event_type::WRITE_ADDR_TO_REG,(void *)event_data));
        delete event_data;
        set_reg_USED_for_addr(reg,var);
    }
    else
    {
        reg=current_basic_block_info_.randomly_get_var_addr_reg(var);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return reg;
}

/*
为了读取某一个int常量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
const_int:要读取的int常量

Return
------
如果获取成功返回true，否则返回false
*/
bool Register_manager::get_designated_reg_for_const(reg_index reg,int const_int)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    pair<reg_index,reg_index> * event_data_1;
    pair<int,reg_index> * event_data_2;
    reg_index from_reg;
    set<reg_index> * regs_unaccessible;
    //首先查看该变量是不是正在被分配，如果是的话，就获取失败
    if(designated_reg.is_allocating())
    {
        return false;
    }
    else if(is_got_by_current_instruction(reg))
    {
        //如果该寄存器正在被当前的指令所使用
        //并且此时被获取的寄存器也是被该变量所占用，那么就获取成功，否则会获取失败
        if(!current_basic_block_info_.check_const_int_value_reg(const_int,reg))
        {
            return false;
        }
    }
    else if(check_const_own_value_reg(const_int))
    {
        //如果此时有寄存器中已经存放了该变量的值
        if(!current_basic_block_info_.check_const_int_value_reg(const_int,reg))
        {
            //再判断这些寄存器里面有没有我们指定的寄存器
            //如果有的话，就无需进行任何操作，如果没有的话，需要先把目标寄存器中的内容写回，然后在寄存器之间进行数据的转移
            from_reg=current_basic_block_info_.randomly_get_const_int_value_reg(const_int);
            regs_unaccessible=new set<reg_index>;
            regs_unaccessible->insert(from_reg);
            notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
            delete regs_unaccessible;
            if(allocate_designated_reg(reg))
            {
                event_data_1=new pair<reg_index,reg_index>(from_reg,reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_CPU_REGS,(void *)event_data_1));
                delete event_data_1;
                set_reg_USED_for_value(reg,const_int);
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            else
            {
                notify(event(event_type::END_INSTRUCTION,nullptr));
                return false;
            }
        }
    }
    else
    {
        //将指定的寄存器中的内容写回，然后把变量写入该寄存器即可
        if(allocate_designated_reg(reg))
        {
            event_data_2=new pair<int,reg_index>(const_int,reg);
            notify(event(event_type::WRITE_CONST_INT_TO_REG,(void *)event_data_2));
            delete event_data_2;
            set_reg_USED_for_value(reg,const_int);
        }
        else
        {
            return false;
        }
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return true;
}

/*
为了读取某一个float常量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
const_float:要读取的float常量

Return
------
如果获取成功返回true，否则返回false
*/
bool Register_manager::get_designated_reg_for_const(reg_index reg,float const_float)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    pair<reg_index,reg_index> * event_data_1;
    pair<float,reg_index> * event_data_2;
    reg_index from_reg;
    set<reg_index> * regs_unaccessible;
    //首先查看该变量是不是正在被分配，如果是的话，就获取失败
    if(designated_reg.is_allocating())
    {
        return false;
    }
    else if(is_got_by_current_instruction(reg))
    {
        //如果该寄存器正在被当前的指令所使用
        //并且此时被获取的寄存器也是被该变量所占用，那么就获取成功，否则会获取失败
        if(!current_basic_block_info_.check_const_float_value_reg(const_float,reg))
        {
            return false;
        }
    }
    else if(check_const_own_value_reg(const_float))
    {
        //如果此时有寄存器中已经存放了该变量的值
        if(!current_basic_block_info_.check_const_float_value_reg(const_float,reg))
        {
            //再判断这些寄存器里面有没有我们指定的寄存器
            //如果有的话，就无需进行任何操作，如果没有的话，需要先把目标寄存器中的内容写回，然后在寄存器之间进行数据的转移
            from_reg=current_basic_block_info_.randomly_get_const_float_value_reg(const_float);
            regs_unaccessible=new set<reg_index>;
            regs_unaccessible->insert(from_reg);
            notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
            delete regs_unaccessible;
            if(allocate_designated_reg(reg))
            {
                event_data_1=new pair<reg_index,reg_index>(from_reg,reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_CPU_REGS,(void *)event_data_1));
                delete event_data_1;
                set_reg_USED_for_value(reg,const_float);
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            else
            {
                notify(event(event_type::END_INSTRUCTION,nullptr));
                return false;
            }
        }
    }
    else
    {
        //将指定的寄存器中的内容写回，然后把变量写入该寄存器即可
        if(allocate_designated_reg(reg))
        {
            event_data_2=new pair<float,reg_index>(const_float,reg);
            notify(event(event_type::WRITE_CONST_FLOAT_TO_REG,(void *)event_data_2));
            delete event_data_2;
            set_reg_USED_for_value(reg,const_float);
        }
        else
        {
            return false;
        }
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return true;
}

/*
为了读取某一个变量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
var:要读取的变量

Return
------
如果获取成功返回true，否则返回false
*/
bool Register_manager::get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    pair<reg_index,reg_index> * event_data_1;
    pair<struct ic_data *,reg_index> * event_data_2;
    reg_index from_reg;
    set<reg_index> * regs_unaccessible;
    if(var->is_const() && !var->is_array_var())
    {
        switch(var->get_data_type())
        {
            case language_data_type::INT:
                return get_designated_reg_for_const(reg,var->get_value().int_data);
                break;
            case language_data_type::FLOAT:
                return get_designated_reg_for_const(reg,var->get_value().float_data);
                break;
            default:
                break;
        }
    }
    //首先查看该变量是不是正在被分配，如果是的话，就获取失败
    if(designated_reg.is_allocating())
    {
        return false;
    }
    else if(is_got_by_current_instruction(reg))
    {
        //如果该寄存器正在被当前的指令所使用
        //并且此时被获取的寄存器也是被该变量所占用，那么就获取成功，否则会获取失败
        if(!current_basic_block_info_.check_var_value_reg(var,reg))
        {
            return false;
        }
    }
    else if(check_var_own_value_reg(var))
    {
        //如果此时有寄存器中已经存放了该变量的值
        if(!current_basic_block_info_.check_var_value_reg(var,reg))
        {
            //再判断这些寄存器里面有没有我们指定的寄存器
            //如果有的话，就无需进行任何操作，如果没有的话，需要先把目标寄存器中的内容写回，然后在寄存器之间进行数据的转移
            from_reg=current_basic_block_info_.randomly_get_var_value_reg(var);
            regs_unaccessible=new set<reg_index>;
            regs_unaccessible->insert(from_reg);
            notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
            delete regs_unaccessible;
            if(allocate_designated_reg(reg))
            {
                event_data_1=new pair<reg_index,reg_index>(from_reg,reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_CPU_REGS,(void *)event_data_1));
                delete event_data_1;
                set_reg_USED_for_value(reg,var);
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            else
            {
                notify(event(event_type::END_INSTRUCTION,nullptr));
                return false;
            }
        }
    }
    else
    {
        //将指定的寄存器中的内容写回，然后把变量写入该寄存器即可
        if(allocate_designated_reg(reg))
        {
            event_data_2=new pair<struct ic_data *,reg_index>(var,reg);
            notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data_2));
            delete event_data_2;
            set_reg_USED_for_value(reg,var);
        }
        else
        {
            return false;
        }
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return true;
}


/*
为了写某一个变量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
var:要写入的变量

Return
------
如果获取成功返回true，否则返回false
*/
bool Register_manager::get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    
    //在对某一个变量进行更改之前，需要遍历此时所有的DIRTY_VALUE寄存器，查看其中的变量值是否和当前要更改的变量有关，如果有关的话，需要先将其写回，并将其置为NOT_USED
    //例如有一个二维数组是a[b][]，这在函数形参中是被允许的
    //那么此时假设要更改的变量是b，而此时的某一个DIRTY_VALUE寄存器中存放着a[2][4]
    //那么当b被更改完之后，a[2][4]可能就不再指向原本的值了，因此在b更改之前必须先将其写回
    // event_data=new pair<struct ic_data *,reg_index>;
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.state==reg_state::DIRTY_VALUE && i.second.var_data->check_ic_data_related(var) && i.second.var_data!=var)
        {
            set_reg_NOT_USED(i.first);
        }
    }

    //首先查看该变量是不是正在被分配，如果是的话，就获取失败
    if(designated_reg.is_allocating())
    {
        return false;
    }
    if(is_got_by_current_instruction(reg))
    {
        //如果该寄存器正在被当前的指令所使用
        //并且此时被获取的寄存器也是被该变量所占用，同时没有别的寄存器中存放了该变量的值，那么就获取成功，否则会获取失败
        if(!current_basic_block_info_.check_var_value_reg(var,reg) || (check_var_own_value_reg(var) && current_basic_block_info_.var_value_regs_map.at(var).size()>1))
        {
            return false;
        }
    }
    else if(check_var_own_value_reg(var))
    {
        //如果此时有寄存器中已经存放了该变量的值
        //再判断这些寄存器里面有没有我们指定的寄存器
        if(!current_basic_block_info_.check_var_value_reg(var,reg))
        {
            //如果没有的话，就把原本所有存放该变量的所有的寄存器置为无效，且无需写回
            //然后把指定的寄存器设置为被该变量占有即可
            //如果有的话，就把原本所有存放该变量的其余的寄存器置为无效，且无需写回
            if(allocate_designated_reg(reg))
            {
                set_reg_USED_for_value(reg,var);
            }
            else
            {
                return false;
            }
        }

        for(auto i:current_basic_block_info_.var_value_regs_map.at(var))
        {
            if(i!=reg)
            {
                regs_.reg_indexs.at(i).state=reg_state::NOT_USED;
                regs_.reg_indexs.at(i).related_data_type=reg_related_data_type::NONE;
                current_basic_block_info_.relieve_var_value_reg(var,i);
            }
        }
    }
    else
    {
        //将指定的寄存器中的内容写回即可
        if(allocate_designated_reg(reg))
        {
            set_reg_USED_for_value(reg,var);
        }
        else
        {
            return false;
        }
    }
    if(!var->is_tmp_var())
    {
        set_reg_DIRTY_VALUE(reg);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
    return true;
}

struct event Register_manager::handle_GET_REG_FOR_INT_CONST(int const_int_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_int_data));
}

struct event Register_manager::handle_GET_REG_FOR_READING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_reading_var(var_data));
}

struct event Register_manager::handle_GET_REG_FOR_WRITING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_writing_var(var_data));
}

struct event Register_manager::handle_GET_ADDR_REG(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_var_addr(var_data));
}

struct event Register_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(struct ic_func * func)
{
    struct event res;
    list<reg_index> * regs=new list<reg_index>;
    //目前是把所有的整型临时寄存器和lr寄存器全部进栈
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::TEMP && i.second.processor==reg_processor::CPU)
        {
            regs->push_back(i.first);
            current_func_regs_info_.context_saved_regs.push_back(i.first);
        }
    }
    regs->push_back(regs_.reg_names.at("lr"));
    current_func_regs_info_.context_saved_regs.push_back(regs_.reg_names.at("lr"));
    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)regs;
    notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,(void *)regs));
    return res;
}

struct event Register_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS(struct ic_func * func)
{
    struct event res;
    list<reg_index> * regs=new list<reg_index>;
    //目前是把所有的浮点临时寄存器全部进栈
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::TEMP && i.second.processor==reg_processor::VFP)
        {
            regs->push_back(i.first);
            current_func_regs_info_.context_saved_regs.push_back(i.first);
        }
    }
    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)regs;
    notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS,(void *)regs));
    return res;
}

struct event Register_manager::handle_READY_TO_PUSH_F_PARAM_CPU_REGS(list<struct ic_data * > * f_params)
{
    struct event res;
    size_t int_f_params_num=0;
    list<reg_index> * f_param_regs=new list<reg_index>;

    //默认函数参数每一个都是4bytes
    for(auto i:(*f_params))
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            if(int_f_params_num>=4)
            {
                break;
            }
            f_param_regs->push_back(regs_.reg_names.at("r"+to_string(int_f_params_num)));
            int_f_params_num++;
        }
    }

    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)f_param_regs;
    return res;
}

struct event Register_manager::handle_READY_TO_PUSH_F_PARAM_VFP_REGS(list<struct ic_data * > * f_params)
{
    struct event res;
    size_t float_f_params_num=0;
    list<reg_index> * f_param_regs=new list<reg_index>;

    //默认函数参数每一个都是4bytes
    for(auto i:(*f_params))
    {
        if(i->get_data_type()==language_data_type::FLOAT)
        {
            if(float_f_params_num>=16)
            {
                break;
            }
            f_param_regs->push_back(regs_.reg_names.at("s"+to_string(float_f_params_num)));
            float_f_params_num++;
        }
    }

    res.type=event_type::RESPONSE_POINTER;
    res.pointer_data=(void *)f_param_regs;
    return res;
}

void Register_manager::handle_FUNC_DEFINE(struct ic_func * func)
{
    size_t int_f_params_num=0,float_f_params_num=0;
    reg_index reg;
    //函数定义的时候，前4*32bits的整型参数已经被放入r0-r3的寄存器了，前16*32bits的浮点型参数也已经被放入s0-s15寄存器了，此时只需要初始化它们的信息即可
    //默认参数大小都是4bytes大小的
    for(auto i:(*func->f_params))
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            //整型参数和数组参数都是放在r0-r3中
            if(int_f_params_num<4)
            {
                reg=regs_.reg_names.at("r"+to_string(int_f_params_num));
                regs_.reg_indexs.at(reg).state=reg_state::USED;
                regs_.reg_indexs.at(reg).related_data_type=reg_related_data_type::VAR;
                regs_.reg_indexs.at(reg).var_data=i;
                current_basic_block_info_.build_var_value_reg(i,reg);
                int_f_params_num++;
            }
        }
        else if(i->get_data_type()==language_data_type::FLOAT)
        {
            //非数组的浮点类型参数放在s0-s15中
            if(float_f_params_num<16)
            {
                reg=regs_.reg_names.at("s"+to_string(float_f_params_num));
                regs_.reg_indexs.at(reg).state=reg_state::USED;
                regs_.reg_indexs.at(reg).related_data_type=reg_related_data_type::VAR;
                regs_.reg_indexs.at(reg).var_data=i;
                current_basic_block_info_.build_var_value_reg(i,reg);
                float_f_params_num++;
            }
        }
    }
}

void Register_manager::handle_END_FUNC()
{
    clear();
}

void Register_manager::handle_END_BASIC_BLOCK()
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG();
    handle_CLEAR_FLAG();
}

void Register_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG()
{
    //结束基本块的时候，需要查看此时是否有还未写回内存的脏值，如果有的话，需要全部写回
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::ARGUMENT || i.second.attr==reg_attr::TEMP)
        {
            //对于那些存储临时变量的寄存器，不必将其写回内存了
            //因为目前我们规定临时变量的作用域不会跨越基本块
            if(i.second.state==reg_state::USED && i.second.related_data_type==reg_related_data_type::VAR && i.second.var_data->is_tmp_var())
            {
                current_basic_block_info_.relieve_var_value_reg(i.second.var_data,i.first);
                regs_.reg_indexs.at(i.first).state=reg_state::NOT_USED;
                regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::NONE;
            }
            else
            {
                set_reg_NOT_USED(i.first);
            }
        }
    }
    //最后还要再遍历一次所有的寄存器，将此时的USED的寄存器置成NOT_USED
    //这些寄存器是在上面的将寄存器写回内存的时候用到的
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::ARGUMENT || i.second.attr==reg_attr::TEMP)
        {
            set_reg_NOT_USED(i.first);
        }
    }
}

void Register_manager::handle_CLEAR_FLAG()
{
    regs_.flag_reg.related_data.first=nullptr;
}

struct event Register_manager::handle_GET_SP_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("sp"));
}

struct event Register_manager::handle_GET_LR_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("lr"));
}

struct event Register_manager::handle_GET_PC_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("pc"));
}

struct event Register_manager::handle_IS_CPU_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_.reg_indexs.at(reg).processor==reg_processor::CPU);
}

struct event Register_manager::handle_IS_VFP_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_.reg_indexs.at(reg).processor==reg_processor::VFP);
}

struct event Register_manager::handle_IS_ARGUMENT_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_.reg_indexs.at(reg).attr==reg_attr::ARGUMENT);
}

struct event Register_manager::handle_IS_TEMP_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_.reg_indexs.at(reg).attr==reg_attr::TEMP);
}

struct event Register_manager::handle_GET_REG_BYTE_SIZE(reg_index reg)
{
    return event(event_type::RESPONSE_INT,(int)(regs_.reg_indexs.at(reg).size/8));
}

void Register_manager::handle_RETURN_VAR(struct ic_data * var)
{
    reg_index return_reg;
    switch(var->get_data_type())
    {
        case language_data_type::INT:
            return_reg=regs_.reg_names.at("r0");
            break;
        case language_data_type::FLOAT:
            return_reg=regs_.reg_names.at("s0");
            break;
        default:
            break;
    }
    if(var)
    {
        get_designated_reg_for_reading_var(return_reg,var);
    }
}

void Register_manager::handle_START_INSRUCTION(set<reg_index> * regs_unaccessible)
{
    set<reg_index> empty;
    if(regs_unaccessible)
    {
        current_instructions_regs_info_.involved_regs.push_back(*regs_unaccessible);
    }
    else
    {
        current_instructions_regs_info_.involved_regs.push_back(empty);
    }
}

void Register_manager::handle_END_INSTRUCTION()
{
    current_instructions_regs_info_.involved_regs.pop_back();
}

void Register_manager::handle_FUNC_RET()
{
    struct ic_data * var;
    for(auto i:regs_.reg_indexs)
    {
        var=i.second.var_data;
        if(i.second.state==reg_state::DIRTY_VALUE)
        {
            if(var->is_global() || (var->is_array_member() && (var->get_belong_array()->is_global() || var->get_belong_array()->is_f_param())))
            {
                //函数退出时，只需要将以下的脏值写回到内存中
                //全局变量
                //所属数组为全局变量的数组取元素变量
                //所属数组为函数形参的数组取元素变量
                set_reg_USED(i.first);
            }
            else
            {
                //至于其他的脏值，就无需将其写回
                //但是仍旧需要将它们设置为USED，以免到时候函数的返回值需要用到它们
                i.second.state=reg_state::USED;
            }
        }
    }
}

void Register_manager::handle_SAVE_REGS_WHEN_CALLING_FUNC()
{
    struct ic_data * tmp_var;
    for(auto i:regs_.reg_indexs)
    {
        //目前先把所有的DIRTY_VALUE寄存器和USED寄存器全部进行写回保存(之所以要把USED的寄存器也写回是因为里面可能是临时变量)
        //把寄存器设置成USED，因为之后写入参数的时候可能会用到它们
        if(i.second.state==reg_state::DIRTY_VALUE)
        {
            set_reg_USED(i.first);
        }
        else if(i.second.state==reg_state::USED && i.second.related_data_type==reg_related_data_type::VAR && i.second.var_data->is_tmp_var())
        {
            tmp_var=i.second.var_data;
            set_reg_NOT_USED(i.first);
            regs_.reg_indexs.at(i.first).state=reg_state::USED;
            regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::VAR;
            regs_.reg_indexs.at(i.first).var_data=tmp_var;
            current_basic_block_info_.build_var_value_reg(tmp_var,i.first);
        }
    }
}

void Register_manager::handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC()
{
    struct ic_data * tmp_var;
    for(auto i:regs_.reg_indexs)
    {
        //只需要把参数寄存器中的DIRTY_VALUE寄存器和USED寄存器全部进行写回保存(之所以要把USED的寄存器也写回是因为里面可能是临时变量)即可
        //把寄存器设置成USED，因为之后写入参数的时候可能会用到它们
        if(i.second.attr!=reg_attr::ARGUMENT)
        {
            continue;
        }
        if(i.second.state==reg_state::DIRTY_VALUE)
        {
            set_reg_USED(i.first);
        }
        else if(i.second.state==reg_state::USED && i.second.related_data_type==reg_related_data_type::VAR && i.second.var_data->is_tmp_var())
        {
            tmp_var=i.second.var_data;
            set_reg_NOT_USED(i.first);
            regs_.reg_indexs.at(i.first).state=reg_state::USED;
            regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::VAR;
            regs_.reg_indexs.at(i.first).var_data=tmp_var;
            current_basic_block_info_.build_var_value_reg(tmp_var,i.first);
        }
    }
}

void Register_manager::handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params)
{
    size_t int_r_params_num=0,float_r_params_num=0;
    reg_index argument_reg;
    set<reg_index> * unaccessible_regs;
    unaccessible_regs=new set<reg_index>;
    for(auto i:(*r_params))
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            if(int_r_params_num<4)
            {
                argument_reg=regs_.reg_names.at("r"+to_string(int_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,i);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
        else if(i->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num<16)
            {
                argument_reg=regs_.reg_names.at("s"+to_string(float_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,i);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
    }
    delete unaccessible_regs;
}

void Register_manager::handle_RET_FROM_CALLED_FUNC(struct ic_data * return_value,reg_index return_reg)
{
    struct ic_data * var;
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::ARGUMENT && (i.second.state==reg_state::USED || i.second.state==reg_state::DIRTY_VALUE))
        {
            //先把所有参数寄存器的内容全部置空
            switch(i.second.related_data_type)
            {
                case reg_related_data_type::CONST_INT:
                    current_basic_block_info_.relieve_const_int_value_reg(i.second.const_int_data,i.first);
                    break;
                case reg_related_data_type::VAR:
                    current_basic_block_info_.relieve_var_value_reg(i.second.var_data,i.first);
                    break;
                case reg_related_data_type::ADDR:
                    current_basic_block_info_.relieve_var_addr_reg(i.second.var_data,i.first);
                    break;
                default:
                    break;
            }
            regs_.reg_indexs.at(i.first).state=reg_state::NOT_USED;
            regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::NONE;
        }
        else if(i.second.attr==reg_attr::TEMP && i.second.state==reg_state::USED && i.second.related_data_type==reg_related_data_type::VAR)
        {
            //同时要把存放如下几种类型的变量的寄存器从USED设置为NOT_USED（因为这些变量可能会在函数调用的过程中被改变）（这里不会再有DIRTY_VALUE的寄存器了）：
            //全局变量
            //数组取元素
            //和上述两类变量有关的变量
            var=i.second.var_data;
            if(var->is_global() || var->is_array_member())
            {
                current_basic_block_info_.relieve_var_value_reg(i.second.var_data,i.first);
                regs_.reg_indexs.at(i.first).state=reg_state::NOT_USED;
                regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::NONE;
            }
        }
    }
    //再把返回值寄存器中放入返回值
    if(return_value)
    {
        if(return_value->is_tmp_var())
        {
            regs_.reg_indexs.at(return_reg).state=reg_state::USED;
        }
        else
        {
            regs_.reg_indexs.at(return_reg).state=reg_state::DIRTY_VALUE;
        }
        regs_.reg_indexs.at(return_reg).related_data_type=reg_related_data_type::VAR;
        regs_.reg_indexs.at(return_reg).var_data=return_value;
        current_basic_block_info_.build_var_value_reg(return_value,return_reg);
    }
}

void Register_manager::handle_RET_FROM_CALLED_ABI_FUNC(struct ic_data * return_value,reg_index return_reg)
{
    struct ic_data * var;
    for(auto i:regs_.reg_indexs)
    {
        if(i.second.attr==reg_attr::ARGUMENT && (i.second.state==reg_state::USED || i.second.state==reg_state::DIRTY_VALUE))
        {
            //把所有参数寄存器的内容全部置空
            switch(i.second.related_data_type)
            {
                case reg_related_data_type::CONST_INT:
                    current_basic_block_info_.relieve_const_int_value_reg(i.second.const_int_data,i.first);
                    break;
                case reg_related_data_type::VAR:
                    current_basic_block_info_.relieve_var_value_reg(i.second.var_data,i.first);
                    break;
                case reg_related_data_type::ADDR:
                    current_basic_block_info_.relieve_var_addr_reg(i.second.var_data,i.first);
                    break;
                default:
                    break;
            }
            regs_.reg_indexs.at(i.first).state=reg_state::NOT_USED;
            regs_.reg_indexs.at(i.first).related_data_type=reg_related_data_type::NONE;
        }
    }
    //再把返回值寄存器中放入返回值
    if(return_value)
    {
        if(return_value->is_tmp_var())
        {
            regs_.reg_indexs.at(return_reg).state=reg_state::USED;
        }
        else
        {
            regs_.reg_indexs.at(return_reg).state=reg_state::DIRTY_VALUE;
        }
        regs_.reg_indexs.at(return_reg).related_data_type=reg_related_data_type::VAR;
        regs_.reg_indexs.at(return_reg).var_data=return_value;
        current_basic_block_info_.build_var_value_reg(return_value,return_reg);
    }
}

struct event Register_manager::handle_CHECK_VAR_OWN_VALUE_REG(struct ic_data * var)
{
    return event(event_type::RESPONSE_BOOL,(bool)(current_basic_block_info_.var_value_regs_map.find(var)!=current_basic_block_info_.var_value_regs_map.end()));
}

struct event Register_manager::handle_CHECK_CONST_INT_OWN_VALUE_REG(int const_int)
{
    return event(event_type::RESPONSE_BOOL,(bool)(current_basic_block_info_.const_int_value_regs_map.find(const_int)!=current_basic_block_info_.const_int_value_regs_map.end()));
}

struct event Register_manager::handle_GET_VAR_S_VALUE_REG(struct ic_data * var)
{
    return event(event_type::RESPONSE_INT,(int)(*(current_basic_block_info_.var_value_regs_map.at(var).begin())));
}

struct event Register_manager::handle_GET_CONST_INT_S_VALUE_REG(int const_int)
{
    return event(event_type::RESPONSE_INT,(int)current_basic_block_info_.randomly_get_const_int_value_reg(const_int));
}

struct event Register_manager::handle_GET_R0_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("r0"));
}

struct event Register_manager::handle_GET_R1_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("r1"));
}

struct event Register_manager::handle_GET_S0_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_.reg_names.at("s0"));;
}

void Register_manager::handle_CHANGE_FLAGS_FOR_VAR(struct ic_data * var,int cared_flag)
{
    regs_.flag_reg.attach_var(var,cared_flag);
}

struct event Register_manager::handle_CHECK_VAR_ATTACHED_TO_FLAG(struct ic_data * var)
{
    return event(event_type::RESPONSE_BOOL,(bool)(regs_.flag_reg.related_data.first==var));
}

struct event Register_manager::handle_GET_VAR_CARED_FLAG()
{
    return event(event_type::RESPONSE_INT,regs_.flag_reg.related_data.second);   
}

struct event Register_manager::handle_ALLOCATE_IDLE_CPU_REG()
{
    return event(event_type::RESPONSE_INT,(int)allocate_idle_reg(reg_processor::CPU));
}

void Register_manager::handle_ATTACH_CONST_INT_TO_REG(int const_int,reg_index reg)
{
    struct reg & designated_reg=regs_.reg_indexs.at(reg);
    if(designated_reg.is_allocating())
    {
        designated_reg.state=reg_state::USED;
        designated_reg.related_data_type=reg_related_data_type::CONST_INT;
        designated_reg.const_int_data=const_int;
        current_basic_block_info_.build_const_int_value_reg(const_int,reg);
        set_got_by_current_instruction(reg);
    }
}

/*
事件处理函数(由中介者进行调用)

Parameters
----------
event:要处理的事件信息

Return
------
处理完事件之后返回响应信息
*/
struct event Register_manager::handler(struct event event)
{
    struct event response;

    switch(event.type)
    {
        case event_type::FUNC_DEFINE:
            handle_FUNC_DEFINE((struct ic_func *)event.pointer_data);
            break;
        case event_type::END_FUNC:
            handle_END_FUNC();
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS:
            response=handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS((struct ic_func *)event.pointer_data);
            break;
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS:
            response=handle_READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS((struct ic_func *)event.pointer_data);
            break;
        case event_type::GET_SP_REG:
            response=handle_GET_SP_REG();
            break;
        case event_type::GET_LR_REG:
            response=handle_GET_LR_REG();
            break;
        case event_type::GET_PC_REG:
            response=handle_GET_PC_REG();
            break;
        case event_type::IS_CPU_REG:
            response=handle_IS_CPU_REG((reg_index)event.int_data);
            break;
        case event_type::IS_VFP_REG:
            response=handle_IS_VFP_REG((reg_index)event.int_data);
            break;
        case event_type::IS_ARGUMENT_REG:
            response=handle_IS_ARGUMENT_REG((reg_index)event.int_data);
            break;
        case event_type::IS_TEMP_REG:
            response=handle_IS_TEMP_REG((reg_index)event.int_data);
            break;
        case event_type::GET_REG_BYTE_SIZE:
            response=handle_GET_REG_BYTE_SIZE(event.int_data);
            break;
        case event_type::READY_TO_PUSH_F_PARAM_CPU_REGS:
            response=handle_READY_TO_PUSH_F_PARAM_CPU_REGS(((struct ic_func *)event.pointer_data)->f_params);
            break;
        case event_type::READY_TO_PUSH_F_PARAM_VFP_REGS:
            response=handle_READY_TO_PUSH_F_PARAM_VFP_REGS(((struct ic_func *)event.pointer_data)->f_params);
            break;
        case event_type::GET_REG_FOR_INT_CONST:
            response=handle_GET_REG_FOR_INT_CONST((int)event.int_data);
            break;
        case event_type::END_BASIC_BLOCK:
            handle_END_BASIC_BLOCK();
            break;
        case event_type::END_BASIC_BLOCK_WITHOUT_FLAG:
            handle_END_BASIC_BLOCK_WITHOUT_FLAG();
            break;
        case event_type::CLEAR_FLAG:
            handle_CLEAR_FLAG();
            break;
        case event_type::RETURN_VAR:
            handle_RETURN_VAR((struct ic_data * )event.pointer_data);
            break;
        case event_type::GET_REG_FOR_READING_VAR:
            response=handle_GET_REG_FOR_READING_VAR((struct ic_data * )event.pointer_data);
            break;
        case event_type::GET_REG_FOR_WRITING_VAR:
            response=handle_GET_REG_FOR_WRITING_VAR((struct ic_data * )event.pointer_data);
            break;
        case event_type::GET_ADDR_REG:
            response=handle_GET_ADDR_REG((struct ic_data * )event.pointer_data);
            break;
        case event_type::START_INSTRUCTION:
            handle_START_INSRUCTION((set<reg_index> *)event.pointer_data);
            break;
        case event_type::END_INSTRUCTION:
            handle_END_INSTRUCTION();
            break;
        case event_type::FUNC_RET:
            handle_FUNC_RET();
            break;
        case event_type::SAVE_REGS_WHEN_CALLING_FUNC:
            handle_SAVE_REGS_WHEN_CALLING_FUNC();
            break;
        case event_type::SAVE_REGS_WHEN_CALLING_ABI_FUNC:
            handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC();
            break;
        case event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC:
            handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC((list<struct ic_data * > *)event.pointer_data);
            break;
        case event_type::RET_FROM_CALLED_FUNC:
            handle_RET_FROM_CALLED_FUNC(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::RET_FROM_CALLED_ABI_FUNC:
            handle_RET_FROM_CALLED_ABI_FUNC(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::CHECK_VAR_OWN_VALUE_REG:
            response=handle_CHECK_VAR_OWN_VALUE_REG((struct ic_data *)event.pointer_data);
            break;
        case event_type::CHECK_CONST_INT_OWN_VALUE_REG:
            response=handle_CHECK_CONST_INT_OWN_VALUE_REG(event.int_data);
            break;
        case event_type::GET_VAR_S_VALUE_REG:
            response=handle_GET_VAR_S_VALUE_REG((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_CONST_INT_S_VALUE_REG:
            response=handle_GET_CONST_INT_S_VALUE_REG(event.int_data);
            break;
        case event_type::GET_R0_REG:
            response=handle_GET_R0_REG();
            break;
        case event_type::GET_R1_REG:
            response=handle_GET_R1_REG();
            break;
        case event_type::GET_S0_REG:
            response=handle_GET_S0_REG();
            break;
        case event_type::CHANGE_FLAGS_FOR_VAR:
            handle_CHANGE_FLAGS_FOR_VAR(((pair<struct ic_data *,int> *)event.pointer_data)->first,((pair<struct ic_data *,int> *)event.pointer_data)->second);
            break;
        case event_type::CHECK_VAR_ATTACHED_TO_FLAG:
            response=handle_CHECK_VAR_ATTACHED_TO_FLAG((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_VAR_CARED_FLAG:
            response=handle_GET_VAR_CARED_FLAG();
            break;
        case event_type::ALLOCATE_IDLE_CPU_REG:
            response=handle_ALLOCATE_IDLE_CPU_REG();
            break;
        case event_type::ATTACH_CONST_INT_TO_REG:
            handle_ATTACH_CONST_INT_TO_REG(((pair<int,reg_index> *)event.pointer_data)->first,((pair<int,reg_index> *)event.pointer_data)->second);
            break;
        default:
            break;
    }
    
    return response;
}