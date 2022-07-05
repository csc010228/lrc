/*
 *
 *  register_manager.cpp
 *  寄存器管理器
 *  Creator:Sichao Chen
 *  Create time:2022/1/27
 *
*/
#include "register_manager.h"

//===================================== class regs_info =====================================//

regs_info::regs_info()
{
    ;
}

regs_info::regs_info(set<struct reg> regs,struct flag_reg flag_reg):flag_reg(flag_reg)
{
    for(auto reg:regs)
    {
        if(reg.processor==reg_processor::CPU && reg.writable==true && (reg.attr==reg_attr::ARGUMENT || reg.attr==reg_attr::TEMP))
        {
            available_CPU_regs.push_back(reg.index);
        }
        else if(reg.processor==reg_processor::VFP && reg.writable==true && (reg.attr==reg_attr::ARGUMENT || reg.attr==reg_attr::TEMP))
        {
            available_VFP_regs.push_back(reg.index);
        }
        reg_indexs.insert(make_pair(reg.index,reg));
        reg_names.insert(make_pair(reg.name,reg.index));
    }
    current_CPU_reg=available_CPU_regs.begin();
    current_VFP_reg=available_VFP_regs.begin();
}

void regs_info::clear()
{
    var_value_regs_map.clear();
    const_value_regs_map.clear();
    var_addr_regs_map.clear();
    for(auto reg:reg_indexs)
    {
        reg.second.clear();
    }
    flag_reg.related_data.first=nullptr;
    current_instructions_involved_regs.clear();
    current_CPU_reg=available_CPU_regs.begin();
    current_VFP_reg=available_VFP_regs.begin();
}

void regs_info::attach_value_to_reg(struct ic_data * var,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    unattach_reg_s_all_addr(reg);
    map_set_insert(var_value_regs_map,var,reg);
    designated_reg.add_value(var);
}

void regs_info::attach_value_to_reg(OAA const_value,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    unattach_reg_s_all_addr(reg);
    map_set_insert(const_value_regs_map,const_value,reg);
    designated_reg.add_value(const_value);
}

void regs_info::attach_addr_to_reg(struct ic_data * var,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    unattach_reg_s_all_addr(reg);
    map_set_insert(var_addr_regs_map,var,reg);
    designated_reg.add_addr(var);
}

void regs_info::unattach_value_to_reg(struct ic_data * var,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    map_set_erase(var_value_regs_map,var,reg);
    designated_reg.remove_value(var);
}

void regs_info::unattach_value_to_reg(OAA const_value,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    map_set_erase(const_value_regs_map,const_value,reg);
    designated_reg.remove_value(const_value);
}

void regs_info::unattach_addr_to_reg(struct ic_data * var,reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    map_set_erase(var_addr_regs_map,var,reg);
    designated_reg.remove_addr(var);
}

void regs_info::unattach_reg_s_all_addr(reg_index reg)
{
    map<struct ic_data *,enum reg_var_state> backup;
    struct reg & designated_reg=reg_indexs.at(reg);
    backup=designated_reg.var_datas;
    for(auto var_data:backup)
    {
        if(var_data.second==reg_var_state::ADDR)
        {
            unattach_addr_to_reg(var_data.first,reg);
        }
    }
}

void regs_info::unattach_reg_s_all_data(reg_index reg)
{
    struct reg & designated_reg=reg_indexs.at(reg);
    if(designated_reg.attached_const)
    {
        unattach_value_to_reg(designated_reg.const_data,reg);
    }
    for(auto var_data:designated_reg.var_datas)
    {
        switch(var_data.second)
        {
            case reg_var_state::ADDR:
                map_set_erase(var_addr_regs_map,var_data.first,reg);
                break;
            case reg_var_state::DIRTY:
            case reg_var_state::NOT_DIRTY:
                map_set_erase(var_value_regs_map,var_data.first,reg);
                break;
            default:
                break;
        }
    }
    designated_reg.var_datas.clear();
    designated_reg.state=reg_state::EMPTY;
}

set<reg_index> regs_info::get_const_owned_value_regs(OAA const_value)
{
    if(const_value_regs_map.find(const_value)!=const_value_regs_map.end())
    {
        return const_value_regs_map.at(const_value);
    }
    return set<reg_index>();
}

set<reg_index> regs_info::get_var_owned_value_regs(struct ic_data * var)
{
    if(var_value_regs_map.find(var)!=var_value_regs_map.end())
    {
        return var_value_regs_map.at(var);
    }
    return set<reg_index>();
}

set<reg_index> regs_info::get_var_owned_addr_regs(struct ic_data * var)
{
    if(var_addr_regs_map.find(var)!=var_addr_regs_map.end())
    {
        return var_addr_regs_map.at(var);
    }
    return set<reg_index>();
}

reg_index regs_info::next_available_CPU_reg()
{
    reg_index res=(*current_CPU_reg);
    current_CPU_reg++;
    if(current_CPU_reg==available_CPU_regs.end())
    {
        current_CPU_reg=available_CPU_regs.end();
    }
    return res;
}

reg_index regs_info::next_available_VFP_reg()
{
    reg_index res=(*current_VFP_reg);
    current_VFP_reg++;
    if(current_VFP_reg==available_VFP_regs.end())
    {
        current_VFP_reg=available_VFP_regs.end();
    }
    return res;
}

//==========================================================================//



//===================================== class Register_manager =====================================//

/*
构造函数

Parameters
----------
regs:目标机器的寄存器信息
flag_reg:标志寄存器的信息
*/
Register_manager::Register_manager(set<struct reg> regs,struct flag_reg flag_reg)
{
    regs_info_=regs_info(regs,flag_reg);
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
    struct reg & designated_reg=regs_info_.reg_indexs.at(reg);
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    //如果要分配的寄存器正在被目前所生成的指令所使用，或者当前要分配的寄存器正在被分配，那么就无法将其获取
    if(!is_got_by_current_instruction(reg) && designated_reg.state!=reg_state::ALLOCATING)
    {
        designated_reg.state=reg_state::ALLOCATING;
        temp=designated_reg.var_datas;
        //需要先把寄存器中需要写回内存的数据进行写回
        //这里目前必须先把临时变量进栈，再把局部变量写回，因为临时变量的进栈会改变栈顶指针的位置，而局部变量的写回不会
        // for(auto var_data:temp)
        // {
        //     if(var_data.first->is_tmp_var() && var_data.second==reg_var_state::NOT_DIRTY && !notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data)
        //     {
        //         event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg);
        //         notify(event(event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK,(void *)event_data));
        //         delete event_data;
        //     }
        // }
        for(auto var_data:temp)
        {
            //如果一个寄存器关联着若干个变量，那么需要对这些变量进行判断，把需要写回内存的进行写回
            if(var_data.first->is_tmp_var() && var_data.second==reg_var_state::NOT_DIRTY)
            {
                event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg);
                // if(notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data)
                // {
                //     notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                // }
                // else
                // {
                //     notify(event(event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK,(void *)event_data));
                // }
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
            }
            else if(var_data.second==reg_var_state::DIRTY)
            {
                regs_info_.reg_indexs.at(reg).set_value_NOT_DIRTY(var_data.first);
                //如果此时要获取的寄存器中存储的变量值是脏值，那么就要把该寄存器写回到内存中
                event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg);
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
            }
        }
        //将这个寄存器上所有关联的常量值，变量值，变量地址全都解除关联
        regs_info_.unattach_reg_s_all_data(reg);
        res=true;
    }

    return res;
}

/*
新分配一个空闲的寄存器

Prarmaters
----------
process:要获取的寄存器的类型
get_an_empty_reg:如果指定为true，表示要获取一个没有被使用过的寄存器

Return
------
返回获取到的寄存器的编号
*/
reg_index Register_manager::allocate_idle_reg(reg_processor processor,bool get_an_empty_reg)
{
    reg_index res;
    // switch(processor)
    // {
    //     case reg_processor::CPU:
    //         do
    //         {
    //             res=regs_info_.next_available_CPU_reg();
    //         }while(!allocate_designated_reg(res));
    //         break;
    //     case reg_processor::VFP:
    //         do
    //         {
    //             res=regs_info_.next_available_VFP_reg();
    //         }while(!allocate_designated_reg(res));
    //         break;
    //     default:
    //         break;
    // }

    
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.processor==processor && (reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP) && (!get_an_empty_reg || reg.second.state==reg_state::EMPTY) && allocate_designated_reg(reg.first))
        {
            res=reg.first;
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
    if(!regs_info_.current_instructions_involved_regs.empty())
    {
        regs_info_.current_instructions_involved_regs.front().insert(reg);
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
    for(auto i:regs_info_.current_instructions_involved_regs)
    {
        if(i.find(reg)!=i.end())
        {
            return true;
        }
    }
    return false;
}


/*
为某一个常量获取一个寄存器

Parameters
----------
const_data:要获取的常数值
processor:要获取的寄存器所在的处理器
get_an_empty_reg:是否要获取一个没有使用过的寄存器

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_const(OAA const_data,enum reg_processor processor,bool get_an_empty_reg)
{
    pair<OAA,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    set<reg_index> suspicious_regs;
    reg_index reg;

    suspicious_regs=regs_info_.get_const_owned_value_regs(const_data);
    //检查此时是否有寄存器中已经存放了这个常数
    if(suspicious_regs.empty())
    {
        //如果没有的话，那么就给该常数分配一个新的寄存器，并将该常数的值写入即可
        reg=allocate_idle_reg(processor,get_an_empty_reg);
        //立刻把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
        event_data_1=new pair<OAA,reg_index>(const_data,reg);
        notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data_1));
        delete event_data_1;
    }
    else
    {
        //如果有的话再判断存放了这些常数的寄存器中是否有我们指定的处理器上的寄存器
        for(auto suspicious_reg:suspicious_regs)
        {
            if(regs_info_.reg_indexs.at(suspicious_reg).processor==processor)
            {
                reg=suspicious_reg;
                //立刻把该寄存器设置为被当前的指令所使用
                set_got_by_current_instruction(reg);
                goto end;
            }
        }
        reg=allocate_idle_reg(processor,get_an_empty_reg);
        //立刻把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
        event_data_2=new pair<reg_index,reg_index>((*suspicious_regs.begin()),reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
        delete event_data_2;
    }
    regs_info_.attach_value_to_reg(const_data,reg);
end:
    return reg;
}

/*
为了读取某一个变量而获取一个寄存器

Parameters
----------
var:要读取的变量
processor:要获取的寄存器所在的处理器

Return
------
返回获取到的寄存器编号
*/
reg_index Register_manager::get_reg_for_reading_var(struct ic_data * var,enum reg_processor processor)
{
    reg_index reg;
    set<reg_index> suspicious_regs;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    map<struct ic_data *,enum reg_var_state> temp;
    struct ic_data * belong_array;

    if(var->is_const() && !var->is_array_var())
    {
        return get_reg_for_const(var->get_value(),processor);
    }

    suspicious_regs=regs_info_.get_var_owned_value_regs(var);
    //首先检查此时是否有寄存器中已经存放了这个变量
    if(suspicious_regs.empty())
    {
        //如果没有的话，需要判断这个变量是不是一个数组取元素，如果是的话，那么需要先遍历所有的寄存器，将该数组的所有数组取元素的脏值先写回内存
        if(var->is_array_member())
        {
            belong_array=var->get_belong_array();
            for(auto reg_info:regs_info_.reg_indexs)
            {
                temp=reg_info.second.var_datas;
                for(auto var_data:temp)
                {
                    if(var_data.first->is_array_member() && !var_data.first->is_array_var() && var_data.first->get_belong_array()==belong_array && var_data.second==reg_var_state::DIRTY)
                    {
                        reg_info.second.set_value_NOT_DIRTY(var_data.first);
                        event_data_1=new pair<struct ic_data *,reg_index>(var_data.first,reg_info.first);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data_1));
                        delete event_data_1;
                    }
                }
            }
        }
        //再给该变量分配一个新的寄存器，并将该变量的值写入即可
        reg=allocate_idle_reg(processor);
        //必须立刻把该寄存器设置为被当前的指令所使用，因为后面的LOAD_VAR_TO_REG也可能需要分配寄存器
        set_got_by_current_instruction(reg);
        event_data_1=new pair<struct ic_data *,reg_index>(var,reg);
        notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data_1));
        delete event_data_1;
    }
    else
    {
        //如果有的话再判断存放了这些常数的寄存器中是否有我们指定的处理器上的寄存器
        for(auto suspicious_reg:suspicious_regs)
        {
            if(regs_info_.reg_indexs.at(suspicious_reg).processor==processor)
            {
                reg=suspicious_reg;
                //立刻把该寄存器设置为被当前的指令所使用
                set_got_by_current_instruction(reg);
                goto end;
            }
        }
        reg=allocate_idle_reg(processor);
        //立刻把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
        event_data_2=new pair<reg_index,reg_index>((*suspicious_regs.begin()),reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
        delete event_data_2;
    }
    regs_info_.unattach_reg_s_all_data(reg);
    regs_info_.attach_value_to_reg(var,reg);
end:
    return reg;
}

/*
在把某一个变量写入寄存器并设置为脏值之前将和它关联的所有脏值写回

Parameters
----------
var:要写入寄存器的变量
*/
void Register_manager::store_DIRTY_values_before_writing_var(struct ic_data * var)
{
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    set<struct ic_data * > written_back_vars;
    bool is_var_array_member;
    struct ic_data * array,* temp_var;
    //在对某一个变量进行更改之前，需要遍历此时所有的寄存器中的所有脏值，查看其中的变量值是否和当前要更改的变量有关，如果有关的话，需要将其写回内存，并且和寄存器解除关联
    //例如有一个二维数组是a[b][]，这在函数形参中是被允许的
    //那么此时假设要更改的变量是b，而此时的某一个DIRTY_VALUE寄存器中存放着a[2][4]
    //那么当b被更改完之后，a[2][4]可能就不再指向原本的值了，因此在b更改之前必须先将其写回
    //同时，如果这个变量是一个数组取元素，那么就需要遍历此时所有的寄存器中的所有脏值，查看其中和该变量属于同一个数组的数组取元素是否有可能与其冲突，如果有可能的话，必须先将其写回
    //假设此时有一个变量a[b]要写入寄存器，同时此时还有一个脏值变量a[c]在另一个寄存器中
    //那么此时需要先把a[c]写回，再把a[b]写入寄存器
    //如果不这样做的话，那么在程序运行的过程中可能会出现b==c的情况，此时a[b]和a[c]其实指向的是同一块内存
    //就会出现a[b]和a[c]都是脏值的情况
    //那么最终到底是哪一个脏值先写回内存呢？这就不好判断了
    is_var_array_member=(var->is_array_member() && !var->is_array_var());
    if(is_var_array_member)
    {
        array=var->get_belong_array();
    }
    for(auto reg_info:regs_info_.reg_indexs)
    {
        if(reg_info.second.state==reg_state::USED && !reg_info.second.var_datas.empty())
        {
            temp=reg_info.second.var_datas;
            for(auto var_data:temp)
            {
                temp_var=var_data.first;
                if(var_data.second==reg_var_state::DIRTY && 
                temp_var!=var && 
                (temp_var->check_ic_data_related(var) || 
                (is_var_array_member && (temp_var->is_array_member() && !temp_var->is_array_var()) && temp_var->get_belong_array()==array && !(var->get_offset()->is_const() && temp_var->get_offset()->is_const() && var->get_offset()->get_value().int_data!=temp_var->get_offset()->get_value().int_data))))
                {
                    if(written_back_vars.find(var_data.first)==written_back_vars.end())
                    {
                        regs_info_.reg_indexs.at(reg_info.first).set_value_NOT_DIRTY(var_data.first);
                        event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg_info.first);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                        written_back_vars.insert(var_data.first);
                    }
                    regs_info_.unattach_value_to_reg(var_data.first,reg_info.first);
                }
            }
        }
    }
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
reg_index Register_manager::get_reg_for_writing_var(struct ic_data * var,enum reg_processor processor)
{
    reg_index reg;
    bool tag=false;
    set<reg_index> suspicious_regs;

    if(var->is_const() && !var->is_array_var())
    {
        return get_reg_for_const(var->get_value(),processor);
    }
    
    store_DIRTY_values_before_writing_var(var);

    suspicious_regs=regs_info_.get_var_owned_value_regs(var);
    //查看此时是否有寄存器已经存放了该变量的值
    if(suspicious_regs.empty())
    {
        //如果没有的话，那么需要新分配一个空闲寄存器，置为脏值即可
        reg=allocate_idle_reg(processor);
        regs_info_.attach_value_to_reg(var,reg);
    }
    else
    {
        //如果有的话，那么只需要从这些寄存器中选一个符合指定处理器的寄存器，并将其设置为脏值即可，而其他剩余的寄存器只需要被设置为无效，无需写回
        for(auto suspicious_reg:suspicious_regs)
        {
            if(regs_info_.reg_indexs.at(suspicious_reg).processor==processor && tag==false)
            {
                tag=true;
                reg=suspicious_reg;
                continue;
            }
            regs_info_.unattach_value_to_reg(var,suspicious_reg);
        }
        if(tag==false)
        {
            //如果所有存放该变量值的寄存器都没有符合我们指定的处理器的话，那么就分配一个新的寄存器给这个变量即可
            reg=allocate_idle_reg(processor);
            regs_info_.attach_value_to_reg(var,reg);
        }
    }
    //如果是写一个临时变量，那么我们也不会将这个临时变量存放的寄存器标记为脏值，而只是标记为被使用了而已，因为临时变量只能赋值一次
    if(!var->is_tmp_var())
    {
        regs_info_.reg_indexs.at(reg).set_value_DIRTY(var);
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
    set<reg_index> suspicious_regs;
    reg_index reg;

    suspicious_regs=regs_info_.get_var_owned_addr_regs(var);
    //查看该变量的地址是否占有寄存器
    if(suspicious_regs.empty())
    {
        //否则的话，分配一个新的空闲寄存器，将地址写入该寄存器
        //默认都把地址放在CPU寄存器中
        reg=allocate_idle_reg(reg_processor::CPU);
        //必须立刻把该寄存器设置为被当前的指令所使用，因为后面的WRITE_ADDR_TO_REG也可能需要分配寄存器
        set_got_by_current_instruction(reg);
        event_data=new pair<struct ic_data *,reg_index>(var,reg);
        notify(event(event_type::WRITE_ADDR_TO_REG,(void *)event_data));
        delete event_data;
        regs_info_.attach_addr_to_reg(var,reg);
    }
    else
    {
        reg=(*suspicious_regs.begin());
        //把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
    }
    return reg;
}

/*
为了读取某一个常量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
const_value:要读取的常量值
*/
void Register_manager::get_designated_reg_for_const(reg_index reg,OAA const_value)
{
    struct reg & designated_reg=regs_info_.reg_indexs.at(reg);
    pair<reg_index,reg_index> * event_data_1;
    pair<OAA,reg_index> * event_data_2;
    reg_index from_reg;
    set<reg_index> * regs_unaccessible;
    set<reg_index> suspicious_regs;

    suspicious_regs=regs_info_.get_const_owned_value_regs(const_value);
    //查看该常量是否占有寄存器
    if(suspicious_regs.empty())
    {
        //如果没有的话，将指定的寄存器中的内容写回，然后把变量写入该寄存器即可
        allocate_designated_reg(reg);
        //必须立刻把该寄存器设置为被当前的指令所使用，因为后面的WRITE_CONST_TO_REG也有可能需要分配寄存器
        set_got_by_current_instruction(reg);
        event_data_2=new pair<OAA,reg_index>(const_value,reg);
        notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data_2));
        delete event_data_2;
    }
    else
    {
        //如果有的话，再查看这里面有没有我们指定的寄存器
        if(suspicious_regs.find(reg)==suspicious_regs.end())
        {
            //如果有的话，就无需进行任何操作，如果没有的话，需要先把目标寄存器中的内容写回，然后在寄存器之间进行数据的转移
            from_reg=(*(suspicious_regs.begin()));
            regs_unaccessible=new set<reg_index>;
            regs_unaccessible->insert(from_reg);
            notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
            delete regs_unaccessible;
            allocate_designated_reg(reg);
            event_data_1=new pair<reg_index,reg_index>(from_reg,reg);
            notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_1));
            delete event_data_1;
            notify(event(event_type::END_INSTRUCTION,nullptr));
        }
        //把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
    }
    regs_info_.attach_value_to_reg(const_value,reg);
}

/*
为了读取某一个变量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
var:要读取的变量
*/
void Register_manager::get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var)
{
    struct reg & designated_reg=regs_info_.reg_indexs.at(reg);
    pair<reg_index,reg_index> * event_data_1;
    pair<struct ic_data *,reg_index> * event_data_2;
    pair<struct ic_data *,reg_index> * event_data_3;
    reg_index from_reg;
    set<reg_index> * regs_unaccessible;
    set<reg_index> suspicious_regs;
    map<struct ic_data *,enum reg_var_state> temp;
    struct ic_data * belong_array;

    //判断该变量是否是常数
    if(var->is_const() && !var->is_array_var())
    {
        get_designated_reg_for_const(reg,var->get_value());
        return;
    }
    
    suspicious_regs=regs_info_.get_var_owned_value_regs(var);
    //查看该变量是否占有寄存器
    if(suspicious_regs.empty())
    {
        //如果没有的话，需要判断这个变量是不是一个数组取元素
        //如果是的话，那么需要先遍历所有的寄存器，将和该数组取元素拥有相同所属数组的的所有数组取元素的脏值先写回内存
        if(var->is_array_member())
        {
            belong_array=var->get_belong_array();
            for(auto reg_info:regs_info_.reg_indexs)
            {
                temp=reg_info.second.var_datas;
                for(auto var_data:temp)
                {
                    if(var_data.first->is_array_member() && !var_data.first->is_array_var() && var_data.first->get_belong_array()==belong_array && var_data.second==reg_var_state::DIRTY)
                    {
                        reg_info.second.set_value_NOT_DIRTY(var_data.first);
                        event_data_3=new pair<struct ic_data *,reg_index>(var_data.first,reg_info.first);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data_3));
                        delete event_data_3;
                    }
                }
            }
        }
        //再将指定的寄存器中的内容写回，然后把变量写入该寄存器即可
        allocate_designated_reg(reg);
        //必须立刻把该寄存器设置为被当前的指令所使用，因为后面的LOAD_VAR_TO_REG也有可能需要分配寄存器
        set_got_by_current_instruction(reg);
        event_data_2=new pair<struct ic_data *,reg_index>(var,reg);
        notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data_2));
        delete event_data_2;
        regs_info_.unattach_reg_s_all_data(reg);
    }
    else
    {
        //如果有的话，判断这些寄存器中是否有我们指定的寄存器
        if(suspicious_regs.find(reg)==suspicious_regs.end())
        {
            //如果有的话，就无需进行任何操作，如果没有的话，需要先把目标寄存器中的内容写回，然后在寄存器之间进行数据的转移
            from_reg=(*(suspicious_regs.begin()));
            regs_unaccessible=new set<reg_index>;
            regs_unaccessible->insert(from_reg);
            notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
            delete regs_unaccessible;
            allocate_designated_reg(reg);
            event_data_1=new pair<reg_index,reg_index>(from_reg,reg);
            notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_1));
            delete event_data_1;
            notify(event(event_type::END_INSTRUCTION,nullptr));
        }
        //把该寄存器设置为被当前的指令所使用
        set_got_by_current_instruction(reg);
    }
    regs_info_.attach_value_to_reg(var,reg);
}

/*
为了写某一个变量而获取一个特定的寄存器

Parameters
----------
reg:指定的寄存器
var:要写入的变量
*/
void Register_manager::get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var)
{
    struct reg & designated_reg=regs_info_.reg_indexs.at(reg);
    set<reg_index> suspicious_regs;

    if(var->is_const() && !var->is_array_var())
    {
        return get_designated_reg_for_const(reg,var->get_value());
    }
    
    store_DIRTY_values_before_writing_var(var);

    suspicious_regs=regs_info_.get_var_owned_value_regs(var);
    //查看此时是否有寄存器存放了该变量的值
    if(suspicious_regs.empty())
    {
        //如果没有的话，就获取一个新的寄存器即可
        allocate_designated_reg(reg);
        regs_info_.attach_value_to_reg(var,reg);
    }
    else
    {
        //如果有的话，再查看这些寄存器中是否有我们指定的寄存器
        if(suspicious_regs.find(reg)==suspicious_regs.end())
        {
            //如果没有我们指定的寄存器的话，那么需要将这些寄存器和该变量的值解除关联，无需写回内存，然后再获取指定的寄存器即可
            for(auto suspicious_reg:suspicious_regs)
            {
                regs_info_.unattach_value_to_reg(var,suspicious_reg);
            }
            allocate_designated_reg(reg);
            regs_info_.attach_value_to_reg(var,reg);
        }
        else
        {
            //如果有我们指定的寄存器的话，那么只需要将其余的寄存器和该变量的值解除关联即可，无需写回内存
            for(auto suspicious_reg:suspicious_regs)
            {
                if(suspicious_reg!=reg)
                {
                    regs_info_.unattach_value_to_reg(var,suspicious_reg);
                }
            }
        }
    }
    if(!var->is_tmp_var())
    {
        designated_reg.set_value_DIRTY(var);
    }
    //把该寄存器设置为被当前的指令所使用
    set_got_by_current_instruction(reg);
}

struct event Register_manager::handle_GET_CPU_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::CPU));
}

struct event Register_manager::handle_GET_AN_EMPTY_CPU_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::CPU,true));
}

struct event Register_manager::handle_GET_VFP_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::VFP));
}

struct event Register_manager::handle_GET_CPU_REG_FOR_READING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_reading_var(var_data,reg_processor::CPU));
}

struct event Register_manager::handle_GET_CPU_REG_FOR_WRITING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_writing_var(var_data,reg_processor::CPU));
}

struct event Register_manager::handle_GET_VFP_REG_FOR_READING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_reading_var(var_data,reg_processor::VFP));
}

struct event Register_manager::handle_GET_VFP_REG_FOR_WRITING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_writing_var(var_data,reg_processor::VFP));
}

struct event Register_manager::handle_GET_ADDR_REG(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_var_addr(var_data));
}

struct event Register_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS(struct ic_func * func)
{
    struct event res(event_type::RESPONSE_POINTER,nullptr);
    reg_index lr=regs_info_.reg_names.at("lr"),fp=regs_info_.reg_names.at("fp");
    list<reg_index> * regs=new list<reg_index>;
    //目前是把所有的整型临时寄存器和lr寄存器全部进栈
    for(auto i:regs_info_.reg_indexs)
    {
        if(i.second.attr==reg_attr::TEMP && i.second.processor==reg_processor::CPU)
        {
            regs->push_back(i.first);
        }
    }
    regs->push_back(fp);
    regs->push_back(lr);
    res.pointer_data=(void *)regs;
    notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,(void *)regs));
    return res;
}

struct event Register_manager::handle_READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS(struct ic_func * func)
{
    struct event res;
    list<reg_index> * regs=new list<reg_index>;
    //目前是把所有的浮点临时寄存器全部进栈
    for(auto i:regs_info_.reg_indexs)
    {
        if(i.second.attr==reg_attr::TEMP && i.second.processor==reg_processor::VFP)
        {
            regs->push_back(i.first);
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
            f_param_regs->push_back(regs_info_.reg_names.at("r"+to_string(int_f_params_num)));
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
        if(i->get_data_type()==language_data_type::FLOAT && !i->is_array_var())
        {
            if(float_f_params_num>=16)
            {
                break;
            }
            f_param_regs->push_back(regs_info_.reg_names.at("s"+to_string(float_f_params_num)));
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
    for(auto f_param:(*func->f_params))
    {
        if(f_param->get_data_type()==language_data_type::INT || f_param->is_array_var())
        {
            //整型参数和数组参数都是放在r0-r3中
            if(int_f_params_num<4)
            {
                reg=regs_info_.reg_names.at("r"+to_string(int_f_params_num));
                regs_info_.attach_value_to_reg(f_param,reg);
                regs_info_.reg_indexs.at(reg).set_value_DIRTY(f_param);
                int_f_params_num++;
            }
        }
        else if(f_param->get_data_type()==language_data_type::FLOAT)
        {
            //非数组的浮点类型参数放在s0-s15中
            if(float_f_params_num<16)
            {
                reg=regs_info_.reg_names.at("s"+to_string(float_f_params_num));
                regs_info_.attach_value_to_reg(f_param,reg);
                regs_info_.reg_indexs.at(reg).set_value_DIRTY(f_param);
                float_f_params_num++;
            }
        }
    }
}

void Register_manager::handle_END_FUNC()
{
    regs_info_.clear();
}

void Register_manager::handle_END_BASIC_BLOCK()
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG();
    handle_CLEAR_FLAG();
}

void Register_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG()
{
    set<struct ic_data * > written_back_vars;
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    //结束基本块的时候，需要查看此时是否有还未写回内存的脏值，如果有的话，需要全部写回
    for(auto reg:regs_info_.reg_indexs)
    {
        if((reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP) && reg.second.state==reg_state::USED)
        {
            //遍历所有的参数寄存器和临时寄存器，将需要写回内存的数值进行写回
            //目前不把临时变量写回寄存器，因为我们目前规定临时变量的作用域不会跨越基本块
            temp=reg.second.var_datas;
            for(auto var_data:temp)
            {
                if(var_data.second==reg_var_state::DIRTY || (var_data.first->is_tmp_var() && var_data.second==reg_var_state::NOT_DIRTY && notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data))
                {
                    if(written_back_vars.find(var_data.first)==written_back_vars.end())
                    {
                        regs_info_.reg_indexs.at(reg.first).set_value_NOT_DIRTY(var_data.first);
                        event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg.first);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                        written_back_vars.insert(var_data.first);
                    }
                    reg.second.set_value_NOT_DIRTY(var_data.first);
                }
            }
        }
    }
    //最后还要再遍历一次所有的寄存器，将此时所有的寄存器置成EMPTY
    //这些寄存器是在上面的将寄存器写回内存的时候用到的
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP)
        {
            regs_info_.unattach_reg_s_all_data(reg.first);
        }
    }
}

void Register_manager::handle_CLEAR_FLAG()
{
    regs_info_.flag_reg.related_data.first=nullptr;
}

struct event Register_manager::handle_GET_SP_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("sp"));
}

struct event Register_manager::handle_GET_FP_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("fp"));
}

struct event Register_manager::handle_GET_LR_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("lr"));
}

struct event Register_manager::handle_GET_PC_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("pc"));
}

struct event Register_manager::handle_GET_APSR_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("APSR_nzcv"));
}

struct event Register_manager::handle_GET_FPSCR_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("FPSCR"));
}

struct event Register_manager::handle_IS_CPU_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_info_.reg_indexs.at(reg).processor==reg_processor::CPU);
}

struct event Register_manager::handle_IS_VFP_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_info_.reg_indexs.at(reg).processor==reg_processor::VFP);
}

struct event Register_manager::handle_IS_ARGUMENT_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_info_.reg_indexs.at(reg).attr==reg_attr::ARGUMENT);
}

struct event Register_manager::handle_IS_TEMP_REG(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,regs_info_.reg_indexs.at(reg).attr==reg_attr::TEMP);
}

struct event Register_manager::handle_GET_REG_BYTE_SIZE(reg_index reg)
{
    return event(event_type::RESPONSE_INT,(int)(regs_info_.reg_indexs.at(reg).size/8));
}

void Register_manager::handle_RETURN_VAR(struct ic_data * var)
{
    reg_index return_reg;
    switch(var->get_data_type())
    {
        case language_data_type::INT:
            return_reg=regs_info_.reg_names.at("r0");
            break;
        case language_data_type::FLOAT:
            return_reg=regs_info_.reg_names.at("s0");
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
    if(regs_unaccessible)
    {
        regs_info_.current_instructions_involved_regs.push_front(*regs_unaccessible);
    }
    else
    {
        regs_info_.current_instructions_involved_regs.push_front(set<reg_index>());
    }
}

void Register_manager::handle_END_INSTRUCTION()
{
    regs_info_.current_instructions_involved_regs.pop_front();
}

void Register_manager::handle_FUNC_RET()
{
    struct ic_data * var;
    set<struct ic_data * > written_back_vars;
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.state==reg_state::USED)
        {
            temp=reg.second.var_datas;
            for(auto var_data:temp)
            {
                if(var_data.second==reg_var_state::DIRTY)
                {
                    var=var_data.first;
                    if(written_back_vars.find(var)==written_back_vars.end() && (var->is_global() || (var->is_array_member() && (var->get_belong_array()->is_global() || var->get_belong_array()->is_f_param()))))
                    {
                        //函数退出时，只需要将以下的脏值写回到内存中
                        //全局变量
                        //所属数组为全局变量的数组取元素变量
                        //所属数组为函数形参的数组取元素变量
                        regs_info_.reg_indexs.at(reg.first).set_value_NOT_DIRTY(var);
                        event_data=new pair<struct ic_data *,reg_index>(var,reg.first);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                        written_back_vars.insert(var);
                    }
                    //至于其他的脏值，就无需将其写回
                    //但是仍旧需要将它们设置为NOT_DIRTY，而不是EMPTY，以免到时候函数的返回值需要用到它们
                    reg.second.set_value_NOT_DIRTY(var);
                }
            }
        }
    }
}

void Register_manager::handle_SAVE_REGS_WHEN_CALLING_FUNC()
{
    set<struct ic_data * > written_back_vars;
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    bool tag;
    for(auto reg:regs_info_.reg_indexs)
    {
        //在函数调用的时候需要保存回内存的数据有：
        //参数寄存器中的DIRTY_VALUE数据和NOT_DIRTY的临时变量
        //被调用的函数可能使用到的全局变量
        //被调用函数可能会用到的数组形参对应的实参的数组取元素
        //和上述两类变量有关的变量
        //会把这些寄存器中的值设置为NOT_DIRTY，而不是EMPTY，因为之后写入参数的时候可能会用到它们
        
        //目前先把所有寄存器中的所有的DIRTY_VALUE值全部进行写回保存，并把它们设置成NOT_DIRTY，因为之后写入参数的时候可能会用到它们
        //同时把所有参数寄存器中的临时变量写回内存
        temp=reg.second.var_datas;
        tag=(reg.second.attr==reg_attr::ARGUMENT);
        for(auto var_data:temp)
        {
            if(var_data.second==reg_var_state::NOT_DIRTY && var_data.first->is_tmp_var() && tag)
            {
                event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg.first);
                // if(notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data)
                // {
                //     notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                // }
                // else
                // {
                //     notify(event(event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK,(void *)event_data));
                // }
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
            }
        }
        for(auto var_data:temp)
        {
            if(var_data.second==reg_var_state::DIRTY || (var_data.first->is_tmp_var() && var_data.second==reg_var_state::NOT_DIRTY && notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data))
            {
                reg.second.set_value_NOT_DIRTY(var_data.first);
                if(written_back_vars.find(var_data.first)==written_back_vars.end())
                {
                    event_data=new pair<struct ic_data *,reg_index>(var_data.first,reg.first);
                    notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                    delete event_data;
                    written_back_vars.insert(var_data.first);
                }
            }
        }
    }
}

void Register_manager::handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC()
{
    handle_SAVE_REGS_WHEN_CALLING_FUNC();
}

void Register_manager::handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params)
{
    size_t int_r_params_num=0,float_r_params_num=0;
    reg_index argument_reg;
    set<reg_index> * unaccessible_regs;
    unaccessible_regs=new set<reg_index>;
    for(auto r_param:(*r_params))
    {
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(int_r_params_num<4)
            {
                argument_reg=regs_info_.reg_names.at("r"+to_string(int_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num<16)
            {
                argument_reg=regs_info_.reg_names.at("s"+to_string(float_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
    }
    delete unaccessible_regs;
}

void Register_manager::handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC(list<struct ic_data * > * r_params,list<reg_index> * r_param_regs)
{
    size_t cpu_r_params_num=0,vfp_r_params_num=0;
    reg_index argument_reg;
    set<reg_index> * unaccessible_regs;
    unaccessible_regs=new set<reg_index>;
    list<reg_index>::iterator r_param_reg;
    if(r_param_regs)
    {
        r_param_reg=r_param_regs->begin();
    }
    for(auto r_param:(*r_params))
    {
        if(r_param_regs && r_param_reg!=r_param_regs->end())
        {
            if(regs_info_.reg_indexs.at(*r_param_reg).processor==reg_processor::CPU && cpu_r_params_num<4)
            {
                cpu_r_params_num++;
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var((*r_param_reg),r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert((*r_param_reg));
            }
            else if(regs_info_.reg_indexs.at(*r_param_reg).processor==reg_processor::VFP && vfp_r_params_num<16)
            {
                vfp_r_params_num++;
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var((*r_param_reg),r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert((*r_param_reg));
            }
            r_param_reg++;
        }
        else if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(cpu_r_params_num<4)
            {
                argument_reg=regs_info_.reg_names.at("r"+to_string(cpu_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(vfp_r_params_num<16)
            {
                argument_reg=regs_info_.reg_names.at("s"+to_string(vfp_r_params_num++));
                notify(event(event_type::START_INSTRUCTION,(void *)unaccessible_regs));
                get_designated_reg_for_reading_var(argument_reg,r_param);
                notify(event(event_type::END_INSTRUCTION,nullptr));
                unaccessible_regs->insert(argument_reg);
            }
        }
    }
    delete unaccessible_regs;
}

void Register_manager::handle_BEFORE_CALL_FUNC(struct ic_data * return_value)
{
    if(return_value)
    {
        store_DIRTY_values_before_writing_var(return_value);
    }
}

void Register_manager::handle_RET_FROM_CALLED_FUNC(struct ic_data * return_value,reg_index return_reg)
{
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.attr==reg_attr::ARGUMENT && reg.second.state==reg_state::USED)
        {
            //先把所有参数寄存器的内容全部置空
            regs_info_.unattach_reg_s_all_data(reg.first);
        }
        else if(reg.second.attr==reg_attr::TEMP && reg.second.state==reg_state::USED)
        {
            //同时要把存放如下几种类型的变量的临时寄存器从NOT_DIRTY设置为EMPTY（因为这些变量可能会在函数调用的过程中被改变）（这里不会再有DIRTY_VALUE的寄存器了）：
            //调用的函数可能会改变的全局变量
            //调用的函数可能会改变的数组形参对应的实参取元素
            //和上述两类变量有关的变量

            //目前是把存放着全局变量和数值取元素都和它们的临时寄存器解除关联
            for(auto var_data:reg.second.var_datas)
            {
                if(var_data.second!=reg_var_state::ADDR && (var_data.first->is_array_member() || var_data.first->is_global()))
                {
                    regs_info_.unattach_value_to_reg(var_data.first,reg.first);
                }
            }
        }
    }
    //再把返回值寄存器中放入返回值
    if(return_value)
    {
        regs_info_.attach_value_to_reg(return_value,return_reg);
        if(!return_value->is_tmp_var())
        {
            regs_info_.reg_indexs.at(return_reg).set_value_DIRTY(return_value);
        }
    }
}

void Register_manager::handle_RET_FROM_CALLED_ABI_FUNC(struct ic_data * return_value,reg_index return_reg)
{
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.attr==reg_attr::ARGUMENT && reg.second.state==reg_state::USED)
        {
            //只需要把所有参数寄存器的内容全部置空
            regs_info_.unattach_reg_s_all_data(reg.first);
        }
    }
    //再把返回值寄存器中放入返回值
    if(return_value)
    {
        regs_info_.attach_value_to_reg(return_value,return_reg);
        if(!return_value->is_tmp_var())
        {
            regs_info_.reg_indexs.at(return_reg).set_value_DIRTY(return_value);
        }
    }
}

struct event Register_manager::handle_CHECK_CONST_VALUE_OWN_CPU_REG(OAA const_value)
{
    if(regs_info_.const_value_regs_map.find(const_value)!=regs_info_.const_value_regs_map.end())
    {
        for(auto reg:regs_info_.const_value_regs_map.at(const_value))
        {
            if(regs_info_.reg_indexs.at(reg).processor==reg_processor::CPU)
            {
                return event(event_type::RESPONSE_BOOL,(bool)true);
            }
        }
    }
    return event(event_type::RESPONSE_BOOL,(bool)false);
}

struct event Register_manager::handle_GET_CONST_VALUE_S_CPU_REG(OAA const_value)
{
    reg_index res;
    if(regs_info_.const_value_regs_map.find(const_value)!=regs_info_.const_value_regs_map.end())
    {
        for(auto reg:regs_info_.const_value_regs_map.at(const_value))
        {
            if(regs_info_.reg_indexs.at(reg).processor==reg_processor::CPU)
            {
                res=reg;
            }
            break;
        }
    }
    return event(event_type::RESPONSE_INT,(int)res);
}

struct event Register_manager::handle_GET_R0_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("r0"));
}

struct event Register_manager::handle_GET_R1_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("r1"));
}

struct event Register_manager::handle_GET_S0_REG()
{
    return event(event_type::RESPONSE_INT,(int)regs_info_.reg_names.at("s0"));;
}

void Register_manager::handle_CHANGE_FLAGS_FOR_VAR(struct ic_data * var,int cared_flag)
{
    regs_info_.flag_reg.attach_var(var,cared_flag);
}

struct event Register_manager::handle_CHECK_VAR_ATTACHED_TO_FLAG(struct ic_data * var)
{
    return event(event_type::RESPONSE_BOOL,(bool)(regs_info_.flag_reg.related_data.first==var));
}

struct event Register_manager::handle_GET_VAR_CARED_FLAG()
{
    return event(event_type::RESPONSE_INT,regs_info_.flag_reg.related_data.second);   
}

struct event Register_manager::handle_ALLOCATE_IDLE_CPU_REG()
{
    return event(event_type::RESPONSE_INT,(int)allocate_idle_reg(reg_processor::CPU));
}

void Register_manager::handle_ATTACH_CONST_TO_REG(OAA const_data,reg_index reg)
{
    regs_info_.attach_value_to_reg(const_data,reg);
}

void Register_manager::handle_ATTACH_VAR_VALUE_TO_REG(struct ic_data * var_data,reg_index reg)
{
    regs_info_.attach_value_to_reg(var_data,reg);
}

void Register_manager::handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(struct ic_data * var,reg_index reg)
{
    set<reg_index> suspicious_regs;

    store_DIRTY_values_before_writing_var(var);

    suspicious_regs=regs_info_.get_var_owned_value_regs(var);
    for(auto suspicious_reg:suspicious_regs)
    {
        regs_info_.unattach_value_to_reg(var,suspicious_reg);
    }
    regs_info_.attach_value_to_reg(var,reg);
    regs_info_.reg_indexs.at(reg).set_value_DIRTY(var);
}

void Register_manager::handle_UNATTACH_REG_S_ALL_DATA(reg_index reg)
{
    regs_info_.unattach_reg_s_all_data(reg);
}

struct event Register_manager::handle_GET_ALL_ARGUMENT_REGS()
{
    set<reg_index> * argument_reg=new set<reg_index>;
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.attr==reg_attr::ARGUMENT)
        {
            argument_reg->insert(reg.first);
        }
    }
    return event(event_type::RESPONSE_POINTER,(void *)argument_reg);
}

struct event Register_manager::handle_GET_CPU_ARGUMENT_REG_NUM()
{
    size_t cpu_argument_reg_num=0;
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.processor==reg_processor::CPU && reg.second.attr==reg_attr::ARGUMENT)
        {
            cpu_argument_reg_num++;
        }
    }
    return event(event_type::RESPONSE_INT,(void *)cpu_argument_reg_num);
}

struct event Register_manager::handle_GET_VFP_ARGUMENT_REG_NUM()
{
    size_t vfp_argument_reg_num=0;
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.processor==reg_processor::VFP && reg.second.attr==reg_attr::ARGUMENT)
        {
            vfp_argument_reg_num++;
        }
    }
    return event(event_type::RESPONSE_INT,(void *)vfp_argument_reg_num);
}

void Register_manager::handle_DISABLE_ALL_ADDR_REG()
{
    for(auto reg:regs_info_.reg_indexs)
    {
        regs_info_.unattach_reg_s_all_addr(reg.first);
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
        case event_type::READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS:
            response=handle_READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS((struct ic_func *)event.pointer_data);
            break;
        case event_type::GET_SP_REG:
            response=handle_GET_SP_REG();
            break;
        case event_type::GET_FP_REG:
            response=handle_GET_FP_REG();
            break;
        case event_type::GET_LR_REG:
            response=handle_GET_LR_REG();
            break;
        case event_type::GET_PC_REG:
            response=handle_GET_PC_REG();
            break;
        case event_type::GET_APSR_REG:
            response=handle_GET_APSR_REG();
            break;
        case event_type::GET_FPSCR_REG:
            response=handle_GET_FPSCR_REG();
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
        case event_type::GET_CPU_REG_FOR_CONST:
            response=handle_GET_CPU_REG_FOR_CONST(*((OAA *)event.pointer_data));
            break;
        case event_type::GET_AN_EMPTY_CPU_REG_FOR_CONST:
            response=handle_GET_AN_EMPTY_CPU_REG_FOR_CONST(*((OAA *)event.pointer_data));
            break;
        case event_type::GET_VFP_REG_FOR_CONST:
            response=handle_GET_VFP_REG_FOR_CONST(*((OAA *)event.pointer_data));
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
        case event_type::GET_CPU_REG_FOR_READING_VAR:
            response=handle_GET_CPU_REG_FOR_READING_VAR((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_CPU_REG_FOR_WRITING_VAR:
            response=handle_GET_CPU_REG_FOR_WRITING_VAR((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_VFP_REG_FOR_READING_VAR:
            response=handle_GET_VFP_REG_FOR_READING_VAR((struct ic_data *)event.pointer_data);
            break;
        case event_type::GET_VFP_REG_FOR_WRITING_VAR:
            response=handle_GET_VFP_REG_FOR_WRITING_VAR((struct ic_data *)event.pointer_data);
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
        case event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC:
            handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC(((pair<list<struct ic_data * > *,list<reg_index> *> *)event.pointer_data)->first,((pair<list<struct ic_data * > *,list<reg_index> *> *)event.pointer_data)->second);
            break;
        case event_type::BEFORE_CALL_FUNC:
            handle_BEFORE_CALL_FUNC((struct ic_data *)event.pointer_data);
            break;
        case event_type::RET_FROM_CALLED_FUNC:
            handle_RET_FROM_CALLED_FUNC(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::RET_FROM_CALLED_ABI_FUNC:
            handle_RET_FROM_CALLED_ABI_FUNC(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::CHECK_CONST_INT_VALUE_OWN_CPU_REG:
            response=handle_CHECK_CONST_VALUE_OWN_CPU_REG(event.int_data);
            break;
        case event_type::GET_CONST_VALUE_S_CPU_REG:
            response=handle_GET_CONST_VALUE_S_CPU_REG(event.int_data);
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
        case event_type::ATTACH_CONST_TO_REG:
            handle_ATTACH_CONST_TO_REG(((pair<OAA,reg_index> *)event.pointer_data)->first,((pair<OAA,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::ATTACH_VAR_VALUE_TO_REG:
            handle_ATTACH_VAR_VALUE_TO_REG(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY:
            handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::UNATTACH_REG_S_ALL_DATA:
            handle_UNATTACH_REG_S_ALL_DATA((reg_index)event.int_data);
            break;
        case event_type::GET_ALL_ARGUMENT_REGS:
            response=handle_GET_ALL_ARGUMENT_REGS();
            break;
        case event_type::GET_CPU_ARGUMENT_REG_NUM:
            response=handle_GET_CPU_ARGUMENT_REG_NUM();
            break;
        case event_type::GET_VFP_ARGUMENT_REG_NUM:
            response=handle_GET_VFP_ARGUMENT_REG_NUM();
            break;
        case event_type::DISABLE_ALL_ADDR_REG:
            handle_DISABLE_ALL_ADDR_REG();
            break;
        default:
            break;
    }
    
    return response;
}

//==========================================================================//