/*
 *
 *  global_register_manager.cpp
 *  全局寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/6
 *
*/
#include "global_register_manager.h"
#include "util.h"

//===================================== struct virtual_reg =====================================//

void virtual_reg::set_related_data(OAA const_value)
{
    related_const=const_value;
    data_type=virtual_related_data_type::CONST;
    is_effective=true;
}

void virtual_reg::set_related_data(struct ic_data * var,enum virtual_related_data_type value_or_addr)
{
    related_var=var;
    data_type=value_or_addr;
    is_effective=true;
}

//==========================================================================//



//===================================== struct virtual_regs_info =====================================//

void virtual_regs_info::clear()
{
    reg_indexs.clear();
    const_value_to_reg_map.clear();
    var_value_to_reg_map.clear();
    var_addr_to_reg_map.clear();
    next_virtaul_reg_index=max_physical_reg_index+1;
    new_temps.clear();
}

void virtual_regs_info::set_all_regs_ineffective()
{
    for(auto & reg:reg_indexs)
    {
        reg.second.is_effective=false;
        reg.second.is_var_value_drity=false;
    }
}

bool virtual_regs_info::check_const_value_own_a_reg(OAA const_value,enum reg_processor processor) const
{
    return map_map_find(const_value_to_reg_map,processor,const_value);
}

bool virtual_regs_info::check_const_value_own_regs(OAA const_value,bool effective) const
{
    if(check_const_value_own_a_reg(const_value,reg_processor::CPU) && (!effective || is_reg_effective(get_const_value_s_reg(const_value,reg_processor::CPU))))
    {
        return true;
    }
    if(check_const_value_own_a_reg(const_value,reg_processor::VFP) && (!effective || is_reg_effective(get_const_value_s_reg(const_value,reg_processor::VFP))))
    {
        return true;
    }
    return false;
}

bool virtual_regs_info::check_var_value_own_a_reg(struct ic_data * var,enum reg_processor processor) const
{
    return map_map_find(var_value_to_reg_map,processor,var);
}

bool virtual_regs_info::check_var_value_own_regs(struct ic_data * var,bool effective) const
{
    if(check_var_value_own_a_reg(var,reg_processor::CPU) && (!effective || is_reg_effective(get_var_value_s_reg(var,reg_processor::CPU))))
    {
        return true;
    }
    if(check_var_value_own_a_reg(var,reg_processor::VFP) && (!effective || is_reg_effective(get_var_value_s_reg(var,reg_processor::VFP))))
    {
        return true;
    }
    return false;
}

bool virtual_regs_info::check_var_addr_own_a_reg(struct ic_data * var,enum reg_processor processor) const
{
    return map_map_find(var_addr_to_reg_map,processor,var);
}

bool virtual_regs_info::check_const_value_own_the_reg(OAA const_value,reg_index reg) const
{
    return reg_indexs.at(reg).data_type==virtual_related_data_type::CONST && reg_indexs.at(reg).related_const==const_value;
}

bool virtual_regs_info::check_var_value_own_the_reg(struct ic_data * var,reg_index reg) const
{
    return reg_indexs.at(reg).data_type==virtual_related_data_type::VAR_VALUE && reg_indexs.at(reg).related_var==var;
}

bool virtual_regs_info::check_var_addr_own_the_reg(struct ic_data * var,reg_index reg) const
{
    return reg_indexs.at(reg).data_type==virtual_related_data_type::VAR_ADDR && reg_indexs.at(reg).related_var==var;
}

reg_index virtual_regs_info::get_const_value_s_reg(OAA const_value,enum reg_processor processor) const
{
    return map_map_at(const_value_to_reg_map,processor,const_value);
}

set<reg_index> virtual_regs_info::get_const_value_s_regs(OAA const_value,bool effective) const
{
    set<reg_index> res;
    reg_index reg;
    if(check_const_value_own_a_reg(const_value,reg_processor::CPU))
    {
        reg=get_const_value_s_reg(const_value,reg_processor::CPU);
        if(!effective || is_reg_effective(reg))
        {
            res.insert(reg);
        }
    }
    if(check_const_value_own_a_reg(const_value,reg_processor::VFP))
    {
        reg=get_const_value_s_reg(const_value,reg_processor::VFP);
        if(!effective || is_reg_effective(reg))
        {
            res.insert(reg);
        }
    }
    return res;
}
    
reg_index virtual_regs_info::get_var_value_s_reg(struct ic_data * var,enum reg_processor processor) const
{
    return map_map_at(var_value_to_reg_map,processor,var);
}

set<reg_index> virtual_regs_info::get_var_value_s_regs(struct ic_data * var,bool effective) const
{
    set<reg_index> res;
    reg_index reg;
    if(check_var_value_own_a_reg(var,reg_processor::CPU))
    {
        reg=get_var_value_s_reg(var,reg_processor::CPU);
        if(!effective || is_reg_effective(reg))
        {
            res.insert(reg);
        }
    }
    if(check_var_value_own_a_reg(var,reg_processor::VFP))
    {
        reg=get_var_value_s_reg(var,reg_processor::VFP);
        if(!effective || is_reg_effective(reg))
        {
            res.insert(reg);
        }
    }
    return res;
}

reg_index virtual_regs_info::get_var_addr_s_reg(struct ic_data * var,enum reg_processor processor) const
{
    return map_map_at(var_addr_to_reg_map,processor,var);
}

reg_index virtual_regs_info::allocate_reg(enum reg_processor processor)
{
    reg_index res=next_virtaul_reg_index++;
    reg_indexs.insert(make_pair(res,virtual_reg(processor)));
    return res;
}

void virtual_regs_info::attach_const_value_to_reg(OAA const_value,reg_index reg)
{
    reg_indexs.at(reg).set_related_data(const_value);
    map_map_insert(const_value_to_reg_map,get_reg_s_processor(reg),const_value,reg);
}

void virtual_regs_info::attach_var_value_to_reg(struct ic_data * var_value,reg_index reg)
{
    reg_indexs.at(reg).set_related_data(var_value,virtual_related_data_type::VAR_VALUE);
    map_map_insert(var_value_to_reg_map,get_reg_s_processor(reg),var_value,reg);
}

void virtual_regs_info::attach_var_addr_to_reg(struct ic_data * var,reg_index reg)
{
    reg_indexs.at(reg).set_related_data(var,virtual_related_data_type::VAR_ADDR);
    map_map_insert(var_addr_to_reg_map,get_reg_s_processor(reg),var,reg);
}

enum reg_processor virtual_regs_info::get_reg_s_processor(reg_index reg) const
{
    return reg_indexs.at(reg).processor;
}

bool virtual_regs_info::is_physical_reg(reg_index reg) const
{
    return reg<=max_physical_reg_index;
}

void virtual_regs_info::set_reg_s_value_DIRTY(reg_index reg)
{
    struct virtual_reg & designated_reg=reg_indexs.at(reg);
    if(designated_reg.data_type==virtual_related_data_type::VAR_VALUE)
    {
        set_reg_effective(reg);
        designated_reg.is_var_value_drity=true;
    }
}

void virtual_regs_info::set_reg_s_value_NOT_DIRTY(reg_index reg)
{
    struct virtual_reg & designated_reg=reg_indexs.at(reg);
    if(designated_reg.data_type==virtual_related_data_type::VAR_VALUE)
    {
        designated_reg.is_var_value_drity=false;
    }
}

bool virtual_regs_info::is_reg_DIRTY(reg_index reg) const
{
    return is_reg_effective(reg) && reg_indexs.at(reg).is_var_value_drity;
}

bool virtual_regs_info::is_reg_effective(reg_index reg) const
{
    return reg_indexs.at(reg).is_effective;
}

void virtual_regs_info::set_reg_ineffective(reg_index reg)
{
    reg_indexs.at(reg).is_effective=false;
    reg_indexs.at(reg).is_var_value_drity=false;
}

void virtual_regs_info::set_reg_effective(reg_index reg)
{
    reg_indexs.at(reg).is_effective=true;
    //reg_indexs.at(reg).is_var_value_drity=false;
}

reg_index virtual_regs_info::new_temp_for_const(OAA const_data,enum reg_processor processor)
{
    reg_index res=next_virtaul_reg_index++;
    reg_indexs.insert(make_pair(res,virtual_reg(processor)));
    reg_indexs.at(res).set_related_data(const_data);
    map_set_insert(new_temps,processor,res);
    return res;
}

reg_index virtual_regs_info::new_temp_for_var_value(struct ic_data * var,enum reg_processor processor)
{
    reg_index res=next_virtaul_reg_index++;
    reg_indexs.insert(make_pair(res,virtual_reg(processor)));
    reg_indexs.at(res).set_related_data(var,virtual_related_data_type::VAR_VALUE);
    map_set_insert(new_temps,processor,res);
    return res;
}

reg_index virtual_regs_info::new_temp_for_var_addr(struct ic_data * var,enum reg_processor processor)
{
    reg_index res=next_virtaul_reg_index++;
    reg_indexs.insert(make_pair(res,virtual_reg(processor)));
    reg_indexs.at(res).set_related_data(var,virtual_related_data_type::VAR_ADDR);
    map_set_insert(new_temps,processor,res);
    return res;
}

bool virtual_regs_info::is_reg_new_temp(reg_index reg) const
{
    return map_set_find(new_temps,reg_indexs.at(reg).processor,reg);
}

//==========================================================================//



//===================================== class Global_register_manager =====================================//

Global_register_manager::Global_register_manager(set<struct reg> regs,struct flag_reg flag_reg):Register_manager(regs,flag_reg),virtual_regs_info_(regs_info_.get_max_reg_index())
{
    
}

Global_register_manager::~Global_register_manager()
{

}

reg_index Global_register_manager::allocate_idle_reg(reg_processor processor,bool get_an_empty_reg)
{
    //分配的空闲寄存器都是虚拟的
    return virtual_regs_info_.allocate_reg(processor);
}

reg_index Global_register_manager::get_reg_for_const(OAA const_data,enum reg_processor processor,bool get_an_empty_reg)
{
    reg_index res;
    set<reg_index> physical_regs;
    pair<OAA,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    //首先查看该常量是否占有一个虚拟寄存器
    if(virtual_regs_info_.check_const_value_own_a_reg(const_data,processor))
    {
        //如果此时该常量占有一个虚拟寄存器，那么再查看该虚拟寄存器此时是否有效
        res=virtual_regs_info_.get_const_value_s_reg(const_data,processor);
        if(virtual_regs_info_.is_reg_effective(res))
        {
            //如果此时此时该虚拟寄存器有效，那么直接将其返回即可
            return res;
        }
        else
        {
            //如果此时该虚拟寄存器无效，那么就将其设置为有效
            virtual_regs_info_.set_reg_effective(res);
        }
    }
    else
    {
        //如果此时该常量不占有一个虚拟寄存器，那么就需要给他分配一个新的虚拟寄存器
        res=allocate_idle_reg(processor);
        virtual_regs_info_.attach_const_value_to_reg(const_data,res);
    }
    //然后查看是否有物理寄存器中存放了这个常量的值
    physical_regs=regs_info_.get_const_owned_value_regs(const_data);
    if(physical_regs.empty())
    {
        //如果此时没有任何的物理寄存器中存放了这个常量的值，那么就需要手动将数字写入寄存器中
        event_data_1=new pair<OAA,reg_index>(const_data,res);
        notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data_1));
        delete event_data_1;
    }
    else
    {
        //如果此时有物理寄存器存放了该常量的值，那么就进行一次数据转移就行了
        event_data_2=new pair<reg_index,reg_index>((*physical_regs.begin()),res);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
        delete event_data_2;
        regs_info_.unattach_value_to_reg(const_data,(*physical_regs.begin()));
    }
    return res;
}

void Global_register_manager::store_array_s_all_DIRTY_members_and_set_them_NOT_DIRTY(struct ic_data * array,bool deal_with_const_offset_member)
{
    set<struct ic_data * > * members;
    pair<struct ic_data *,reg_index> * event_data;
    set<reg_index> regs;
    //先检查物理寄存器
    Register_manager::store_array_s_all_DIRTY_members_and_set_them_NOT_DIRTY(array,deal_with_const_offset_member);
    //再检查虚拟寄存器
    members=(set<struct ic_data * > * )notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)array)).pointer_data;
    if(members)
    {
        for(auto member:*members)
        {
            if(!deal_with_const_offset_member && member->get_offset()->is_const())
            {
                continue;
            }
            regs=virtual_regs_info_.get_var_value_s_regs(member);
            for(auto reg:regs)
            {
                if(virtual_regs_info_.is_reg_DIRTY(reg))
                {
                    event_data=new pair<struct ic_data *,reg_index>(member,reg);
                    notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                    delete event_data;
                    virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg);
                }
            }
        }
    }
}

reg_index Global_register_manager::get_reg_for_reading_var(struct ic_data * var,enum reg_processor processor)
{
    reg_index res;
    set<reg_index> physical_regs;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    if(var->is_const() && !var->is_array_var())
    {
        return get_reg_for_const(var->get_value(),processor);
    }
    //首先查看该变量是否占有一个虚拟寄存器
    if(virtual_regs_info_.check_var_value_own_a_reg(var,processor))
    {
        //如果此时该变量的值占有一个虚拟寄存器，那么再查看该虚拟寄存器此时是否有效
        res=virtual_regs_info_.get_var_value_s_reg(var,processor);
        if(virtual_regs_info_.is_reg_effective(res))
        {
            //如果此时此时该虚拟寄存器有效，那么直接将其返回即可
            return res;
        }
        else
        {
            //如果此时该虚拟寄存器无效，那么就将其设置为有效
            virtual_regs_info_.set_reg_effective(res);
        }
    }
    else
    {
        //如果此时该变量的值不占有一个虚拟寄存器，那么就需要给他分配一个新的虚拟寄存器，并将它们关联起来
        res=allocate_idle_reg(processor);
        virtual_regs_info_.attach_var_value_to_reg(var,res);
    }
    //然后查看是否有物理寄存器中存放了这个变量的值
    physical_regs=regs_info_.get_var_owned_value_regs(var);
    if(physical_regs.empty())
    {
        //如果此时没有任何的物理寄存器中存放了这个变量的值
        if(var->is_array_member())
        {
            //那么需要判断这个变量是不是一个数组取元素，如果是的话，那么需要先遍历所有的寄存器，将该数组的所有数组取元素的脏值先写回内存
            store_array_s_all_DIRTY_members_and_set_them_NOT_DIRTY(var->get_belong_array(),!var->get_offset()->is_const());
        }
        //把数据从内存中load到寄存器中
        event_data_1=new pair<struct ic_data *,reg_index>(var,res);
        notify(event(event_type::LOAD_VAR_TO_REG,(void *)event_data_1));
        delete event_data_1;
    }
    else
    {
        //如果此时有物理寄存器存放了该变量的值，那么就进行一次数据转移就行了
        event_data_2=new pair<reg_index,reg_index>((*physical_regs.begin()),res);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
        delete event_data_2;
        regs_info_.unattach_value_to_reg(var,(*physical_regs.begin()));
    }
    return res;
}

void Global_register_manager::before_writing_var_value(struct ic_data * var)
{
    //在对一个变量进行写入之前，总共有下面3种情况需要处理：
    //（1）任何把变量var作为偏移量的数组元素的值和地址都需要和它所在的寄存器解除关联，并且需要将脏值写回内存
    //（2）假设函数的某一个数组形参定义的时候使用了变量var，那么该数组形参的所有数组元素的值和地址都需要和它们所在的寄存器解除关联，并且需要将脏值写回内存
    //（3）假设变量var是一个数组元素，那么需要把和它同一个所属数组的所有数组元素全都和它们的寄存器解除关联，并且需要将脏值写回内存
    set<struct ic_data * > * members;
    list<struct ic_data * > * f_params;
    pair<struct ic_data *,reg_index> * event_data;
    struct ic_data * array,* offset;
    set<reg_index> regs;
    reg_index temp;
    //先对物理寄存器中的数据进行处理
    Register_manager::before_writing_var_value(var);
    //再对虚拟寄存器中的数据进行处理
    //情况（1）
    members=(set<struct ic_data * > * )notify(event(event_type::GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET,(void *)var)).pointer_data;
    if(members)
    {
        for(auto member:*members)
        {
            regs=virtual_regs_info_.get_var_value_s_regs(member);
            for(auto reg:regs)
            {
                if(virtual_regs_info_.is_reg_DIRTY(reg))
                {
                    event_data=new pair<struct ic_data *,reg_index>(member,reg);
                    notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                    delete event_data;
                }
                virtual_regs_info_.set_reg_ineffective(reg);
            }
            if(virtual_regs_info_.check_var_addr_own_a_reg(member,reg_processor::CPU))
            {
                temp=virtual_regs_info_.get_var_addr_s_reg(member,reg_processor::CPU);
                if(virtual_regs_info_.is_reg_effective(temp))
                {
                    virtual_regs_info_.set_reg_ineffective(temp);
                }
            }
        }
    }
    //情况（2）
    f_params=(list<struct ic_data * > *)notify(event(event_type::GET_CURRENT_FUNC_S_F_PARAMS,nullptr)).pointer_data;
    if(f_params)
    {
        for(auto f_param:*f_params)
        {
            if(f_param->is_array_var() && f_param->check_ic_data_related(var))
            {
                members=(set<struct ic_data * > * )notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)f_param)).pointer_data;
                if(members)
                {
                    for(auto member:*members)
                    {
                        regs=virtual_regs_info_.get_var_value_s_regs(member);
                        for(auto reg:regs)
                        {
                            if(virtual_regs_info_.is_reg_DIRTY(reg))
                            {
                                event_data=new pair<struct ic_data *,reg_index>(member,reg);
                                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                                delete event_data;
                            }
                            virtual_regs_info_.set_reg_ineffective(reg);
                        }
                        if(virtual_regs_info_.check_var_addr_own_a_reg(member,reg_processor::CPU))
                        {
                            temp=virtual_regs_info_.get_var_addr_s_reg(member,reg_processor::CPU);
                            if(virtual_regs_info_.is_reg_effective(temp))
                            {
                                virtual_regs_info_.set_reg_ineffective(temp);
                            }
                        }
                    }
                }
            }
        }
    }
    //情况（3）
    if(var->is_array_member())
    {
        array=var->get_belong_array();
        offset=var->get_offset();
        members=(set<struct ic_data * > * )notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)array)).pointer_data;
        if(members)
        {
            for(auto member:*members)
            {
                if(member==var || (member->get_offset()->is_const() && offset->is_const() && member->get_offset()->get_value().int_data!=offset->get_value().int_data))
                {
                    continue;
                }
                regs=virtual_regs_info_.get_var_value_s_regs(member);
                for(auto reg:regs)
                {
                    if(virtual_regs_info_.is_reg_DIRTY(reg))
                    {
                        event_data=new pair<struct ic_data *,reg_index>(member,reg);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                    }
                    virtual_regs_info_.set_reg_ineffective(reg);
                }
            }
        }
    }
}

reg_index Global_register_manager::get_reg_for_writing_var(struct ic_data * var,enum reg_processor processor)
{
    reg_index res;
    set<reg_index> physical_regs,virtual_regs;
    bool tag=false;
    if(var->is_const() && !var->is_array_var())
    {
        return get_reg_for_const(var->get_value(),processor);
    }
    //先把那些和要写入的变量有关的变量与它们所在的寄存器解除关联，并且在有需要的时候进行写回
    before_writing_var_value(var);
    //查看该变量是否占有任何的虚拟寄存器
    virtual_regs=virtual_regs_info_.get_var_value_s_regs(var,false);
    for(auto reg:virtual_regs)
    {
        //如果有的话，对该变量的值占有的所有虚拟寄存器进行遍历
        if(virtual_regs_info_.get_reg_s_processor(reg)==processor)
        {
            //如果找到了指定的处理器上的虚拟寄存器，作为最终的结果
            res=reg;
            tag=true;
            virtual_regs_info_.set_reg_effective(res);
        }
        else
        {
            //对于其他的虚拟寄存器，将其设置为无效
            virtual_regs_info_.set_reg_ineffective(reg);
        }
    }
    if(!tag)
    {
        //如果此时还没有找到指定处理器上的虚拟寄存器，那么就需要新分配一个
        res=allocate_idle_reg(processor);
        virtual_regs_info_.attach_var_value_to_reg(var,res);
    }
    virtual_regs_info_.set_reg_s_value_DIRTY(res);
    //再查看这个变量是否占有物理寄存器
    physical_regs=regs_info_.get_var_owned_value_regs(var);
    for(auto physical_reg:physical_regs)
    {
        //如果有的话，需要把这些物理寄存器全部设置为无效
        regs_info_.unattach_value_to_reg(var,physical_reg);
    }
    return res;
}

reg_index Global_register_manager::get_reg_for_var_addr(struct ic_data * var,enum reg_processor processor)
{
    reg_index res;
    set<reg_index> physical_regs;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    //先查看该变量的地址是否占有一个虚拟寄存器
    if(virtual_regs_info_.check_var_addr_own_a_reg(var,processor))
    {
        //如果此时该变量的地址已经占有了一个虚拟寄存器，那么再检查是否该虚拟寄存器是否有效
        res=virtual_regs_info_.get_var_addr_s_reg(var,processor);
        if(virtual_regs_info_.is_reg_effective(res))
        {
            //如果此时此时该虚拟寄存器有效，那么直接将其返回即可
            return res;
        }
        else
        {
            //如果此时该虚拟寄存器无效，那么就将其设置为有效
            virtual_regs_info_.set_reg_effective(res);
        }
    }
    else
    {
        //如果此时该变量的地址不占有一个虚拟寄存器，那么就需要给他分配一个新的虚拟寄存器
        res=allocate_idle_reg(processor);
        virtual_regs_info_.attach_var_addr_to_reg(var,res);
    }
    //然后查看是否有物理寄存器中存放了这个变量的地址
    physical_regs=regs_info_.get_var_owned_addr_regs(var);
    if(physical_regs.empty())
    {
        //如果此时没有任何的物理寄存器中存放了这个变量的地址，那么就需要手动将地址写入该虚拟寄存器
        event_data_1=new pair<struct ic_data *,reg_index>(var,res);
        notify(event(event_type::WRITE_ADDR_TO_REG,(void *)event_data_1));
        delete event_data_1;
    }
    else
    {
        //如果此时有物理寄存器存放了该变量的地址，那么就进行一次数据转移就行了
        event_data_2=new pair<reg_index,reg_index>((*physical_regs.begin()),res);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
        delete event_data_2;
        regs_info_.unattach_addr_to_reg(var,(*physical_regs.begin()));
    }
    return res;
}

void Global_register_manager::get_designated_reg_for_const(reg_index reg,OAA const_data)
{
    //基本上可以保证这里要获取的寄存器reg都是物理寄存器
    //如果有虚拟寄存器中存储了该常量的值，并且有效的话，那么使用数据转移指令将其转移到指定的物理寄存器上即可
    //否则的话，需要将常量手动写入该物理寄存器
    set<reg_index> regs;
    pair<reg_index,reg_index> * event_data;
    if(!virtual_regs_info_.is_physical_reg(reg))
    {
        return;
    }
    if(virtual_regs_info_.check_const_value_own_regs(const_data))
    {
        regs=virtual_regs_info_.get_const_value_s_regs(const_data);
        event_data=new pair<reg_index,reg_index>(*(regs.begin()),reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
        delete event_data;
    }
    else
    {
        Register_manager::get_designated_reg_for_const(reg,const_data);
    }
}

void Global_register_manager::get_designated_reg_for_reading_var(reg_index reg,struct ic_data * var)
{
    //基本上可以保证这里要获取的寄存器reg都是物理寄存器
    //如果有虚拟寄存器中存储了该变量的值，并且有效的话，那么使用数据转移指令将其转移到指定的物理寄存器上即可
    //否则的话，需要将变量手动写入该物理寄存器
    set<reg_index> regs;
    pair<reg_index,reg_index> * event_data;
    if(var->is_const())
    {
        get_designated_reg_for_const(reg,var->get_value());
        return;
    }
    if(!virtual_regs_info_.is_physical_reg(reg))
    {
        return;
    }
    if(virtual_regs_info_.check_var_value_own_regs(var))
    {
        regs=virtual_regs_info_.get_var_value_s_regs(var);
        event_data=new pair<reg_index,reg_index>(*(regs.begin()),reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
        delete event_data;
    }
    else
    {
        Register_manager::get_designated_reg_for_reading_var(reg,var);
    }
}

void Global_register_manager::get_designated_reg_for_writing_var(reg_index reg,struct ic_data * var)
{
    //基本上可以保证这里要获取的寄存器reg都是物理寄存器
    if(var->is_const())
    {
        get_designated_reg_for_const(reg,var->get_value());
        return;
    }
    if(!virtual_regs_info_.is_physical_reg(reg))
    {
        return;
    }
    Register_manager::get_designated_reg_for_writing_var(reg,var);
}

void Global_register_manager::handle_START_FUNC(struct ic_func_flow_graph * func)
{
    set<struct ic_data * > all_datas;
    reg_index reg;
    enum reg_processor processor;
    set<struct ic_data * > * f_params_in_regs=(set<struct ic_data * > *)notify(event(event_type::GET_FUNC_S_F_PARAMS_IN_REGS,(void *)func->func)).pointer_data;
    Register_manager::handle_START_FUNC(func);
    //在函数开始的时候进行有效常量分析，有效变量分析,有效地址分析和脏值分析
    //notify(event(event_type::INEFFECTIVE_CONST_VALUE_ANALYSIS,(void *)(func)));
    notify(event(event_type::INEFFECTIVE_VALUE_ANALYSIS,(void *)(func)));
    notify(event(event_type::DIRTY_VALUE_ANALYSIS,(void *)(func)));
    //notify(event(event_type::INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS,(void *)(func)));
    //同时在函数开始的时候需要获取该函数可能涉及到的所有数据，然后为他们分配相应的虚拟寄存器
    for(auto bb:func->basic_blocks)
    {
        for(auto ins:bb->ic_sequence)
        {
            for(auto use:ins.uses)
            {
                all_datas.insert(use);
            }
            if(ins.explicit_def)
            {
                if(ins.explicit_def->is_array_member())
                {
                    all_datas.insert(ins.explicit_def->get_belong_array());
                    all_datas.insert(ins.explicit_def->get_offset());
                }
                all_datas.insert(ins.explicit_def);
            }
        }
    }
    for(auto data:all_datas)
    {
        if(data->is_const())
        {
            //对于常量，只需要为它们的值分配寄存器
            switch(data->get_data_type())
            {
                case language_data_type::INT:
                    processor=reg_processor::CPU;
                    break;
                case language_data_type::FLOAT:
                    processor=reg_processor::VFP;
                    break;
                default:
                    break;
            }
            if(!virtual_regs_info_.check_const_value_own_a_reg(data->get_value(),processor))
            {
                reg=allocate_idle_reg(processor);
                virtual_regs_info_.attach_const_value_to_reg(data->get_value(),reg);
            }
        }
        else
        {
            if((data->is_global() && !data->is_array_var()) || (data->is_f_param() && f_params_in_regs->find(data)==f_params_in_regs->end()))
            {
                //需要分配地址寄存器的变量有：
                //全局非数组变量
                //放在内存中传递的函数参数
                if(!virtual_regs_info_.check_var_addr_own_a_reg(data,reg_processor::CPU))
                {
                    reg=allocate_idle_reg(reg_processor::CPU);
                    virtual_regs_info_.attach_var_addr_to_reg(data,reg);
                }
            }
            if(data->get_data_type()==language_data_type::INT || data->is_array_var())
            {
                processor=reg_processor::CPU;
            }
            else if(data->get_data_type()==language_data_type::FLOAT)
            {
                processor=reg_processor::VFP;
            }
            if(!virtual_regs_info_.check_var_value_own_a_reg(data,processor))
            {
                reg=allocate_idle_reg(processor);
                virtual_regs_info_.attach_var_value_to_reg(data,reg);
            }
        }
    }
    delete f_params_in_regs;
}

void Global_register_manager::handle_END_FUNC()
{
    //最后把物理寄存器堆和虚拟寄存器堆清空
    Register_manager::handle_END_FUNC();
    virtual_regs_info_.clear();
}

void Global_register_manager::handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    reg_index reg;
    set<reg_index> regs;
    set<struct ic_data * > * vars;
    enum language_data_type data_type;
    set<pair<OAA,enum language_data_type > > * const_values;
    enum reg_processor processor;
    //首先需要将所有的物理寄存器的信息全部设置为无效
    regs_info_.clear();
    //在基本块开始的时候需要根据有效常量分析的结果，将变量的值和虚拟寄存器关联起来
    const_values=(set<pair<OAA,enum language_data_type > > * )notify(event(event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_CONST_VALUE_ANALYSIS_INFO,(void *)basic_block)).pointer_data;
    for(auto different_processor_s_const_value_s_regs:virtual_regs_info_.const_value_to_reg_map)
    {
        for(auto const_value_s_reg:different_processor_s_const_value_s_regs.second)
        {
            // if(const_values->find(pair<OAA,enum language_data_type>(const_value_s_reg.first,data_type))!=const_values->end())
            // {
            //     virtual_regs_info_.set_reg_ineffective(const_value_s_reg.second);
            // }
            // else
            // {
            //     virtual_regs_info_.set_reg_effective(const_value_s_reg.second);
            // }
            //先把所有的常数都设置为无效
            virtual_regs_info_.set_reg_ineffective(const_value_s_reg.second);
        }
    }
    //在基本块开始的时候需要根据有效变量分析的结果，将变量的值和虚拟寄存器关联起来
    vars=(set<struct ic_data * > * )notify(event(event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_VALUE_ANALYSIS_INFO,(void *)basic_block)).pointer_data;
    for(auto different_processor_s_var_value_s_regs:virtual_regs_info_.var_value_to_reg_map)
    {
        for(auto var_value_s_reg:different_processor_s_var_value_s_regs.second)
        {
            if(vars->find(var_value_s_reg.first)!=vars->end())
            {
                virtual_regs_info_.set_reg_ineffective(var_value_s_reg.second);
            }
            else
            {
                virtual_regs_info_.set_reg_effective(var_value_s_reg.second);
            }
            virtual_regs_info_.set_reg_s_value_NOT_DIRTY(var_value_s_reg.second);
        }
    }
    //在根据有效地址分析的结果，将变量的地址和虚拟寄存器关联起来
    vars=(set<struct ic_data * > * )notify(event(event_type::GET_BASIC_BLOCK_ENTER_S_INEFFECTIVE_NOT_ARRAY_GLOBAL_ADDR_ANALYSIS_INFO,(void *)basic_block)).pointer_data;
    for(auto different_processor_s_var_addr_s_regs:virtual_regs_info_.var_addr_to_reg_map)
    {
        for(auto var_addr_s_reg:different_processor_s_var_addr_s_regs.second)
        {
            // if(vars->find(var_addr_s_reg.first)!=vars->end())
            // {
            //     virtual_regs_info_.set_reg_ineffective(var_addr_s_reg.second);
            // }
            // else
            // {
            //     virtual_regs_info_.set_reg_effective(var_addr_s_reg.second);
            // }
            //先把所有的地址都设置为无效
            virtual_regs_info_.set_reg_ineffective(var_addr_s_reg.second);
        }
    }
    //然后再根据脏值分析的结果，将虚拟寄存器赋值为脏值
    vars=(set<struct ic_data * > * )notify(event(event_type::GET_BASIC_BLOCK_ENTER_S_DIRTY_VALUE_ANALYSIS_INFO,(void *)basic_block)).pointer_data;
    for(auto var:*vars)
    {
        if(var->is_global() && !var->is_array_var())
        {
            regs=virtual_regs_info_.get_var_value_s_regs(var);
            for(auto target_reg:regs)
            {
                if(virtual_regs_info_.is_reg_effective(target_reg))
                {
                    virtual_regs_info_.set_reg_s_value_DIRTY(target_reg);
                }
            }
        }
    }
}

void Global_register_manager::handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG(basic_block);
    handle_CLEAR_FLAG();
}

void Global_register_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block)
{
    static struct ic_basic_block * last_end_basic_block=nullptr;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<reg_index,reg_index> * event_data_2;
    set<struct ic_data * > * dirty_vars;
    map<struct ic_data * ,enum reg_var_state> temp;
    enum reg_processor processor;
    reg_index v_reg;
    set<struct ic_data * > * f_params_in_regs=(set<struct ic_data * > *)notify(event(event_type::GET_FUNC_S_F_PARAMS_IN_REGS,(void *)current_func_->func)).pointer_data;
    if(basic_block==last_end_basic_block)
    {
        return;
    }
    last_end_basic_block=basic_block;
    //先把所有的物理寄存器设置为无效，并将其赋值给虚拟寄存器
    for(auto reg:regs_info_.reg_indexs)
    {
        if((reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP) && reg.second.state==reg_state::USED)
        {
            temp=reg.second.var_datas;
            for(auto var_data:temp)
            {
                if(var_data.second==reg_var_state::DIRTY)
                {
                    processor=reg.second.processor;
                    if(virtual_regs_info_.check_var_value_own_a_reg(var_data.first,processor))
                    {
                        v_reg=virtual_regs_info_.get_var_value_s_reg(var_data.first,processor);
                    }
                    else
                    {
                        v_reg=allocate_idle_reg(processor);
                        virtual_regs_info_.attach_var_value_to_reg(var_data.first,v_reg);
                    }
                    event_data_2=new pair<reg_index,reg_index>(reg.first,v_reg);
                    notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_2));
                    delete event_data_2;
                    virtual_regs_info_.set_reg_s_value_DIRTY(v_reg);
                }
            }
        }
    }
    for(auto reg:regs_info_.reg_indexs)
    {
        if(reg.second.attr==reg_attr::ARGUMENT || reg.second.attr==reg_attr::TEMP)
        {
            regs_info_.unattach_reg_s_all_data(reg.first);
        }
    }
    //获取本基本块退出的时候仍旧可以是脏值的变量值
    dirty_vars=(set<struct ic_data * > *)notify(event(event_type::GET_CURRENT_BASIC_BLOCK_EXIT_S_DIRTY_VALUE_ANALYSIS_INFO,nullptr)).pointer_data;
    //再把此时需要写回内存的虚拟寄存器写回即可
    for(auto reg:virtual_regs_info_.reg_indexs)
    {
        if(reg.second.data_type==virtual_related_data_type::VAR_VALUE && virtual_regs_info_.is_reg_DIRTY(reg.first))
        {
            //在基本块结束的时候，把如下的变量进行写回：
            //所有的数组元素
            //需要写回内存的非数组全局变量
            //需要写回内存的放在内存中传递的函数形参
            if((reg.second.related_var->is_array_member() && !reg.second.related_var->is_array_var()) || 
            (dirty_vars->find(reg.second.related_var)==dirty_vars->end() && reg.second.related_var->is_global() && !reg.second.related_var->is_array_var()) ||
            (reg.second.related_var->is_f_param() && !reg.second.related_var->is_array_var() && f_params_in_regs->find(reg.second.related_var)==f_params_in_regs->end()))
            {
                event_data_1=new pair<struct ic_data *,reg_index>(reg.second.related_var,reg.first);
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data_1));
                delete event_data_1;
            }
        }
    }
    delete f_params_in_regs;
}

void Global_register_manager::handle_FUNC_RET()
{
    struct ic_data * var;
    pair<struct ic_data *,reg_index> * event_data;
    //对物理寄存器进行处理
    Register_manager::handle_FUNC_RET();
    //把虚拟寄存器写回内存
    for(auto reg:virtual_regs_info_.reg_indexs)
    {
        if(virtual_regs_info_.is_reg_effective(reg.first) && virtual_regs_info_.is_reg_DIRTY(reg.first))
        {
            //函数退出时，只需要将以下的脏值写回到内存中
            //全局变量
            //所属数组为全局变量的数组取元素变量
            //所属数组为函数形参的数组取元素变量
            //需要将所有的脏值设置为NOT_DIRTY，而不是无效，以免到时候函数的返回值需要用到它们
            var=reg.second.related_var;
            if(var->is_global() ||
            var->is_array_member() && !var->is_array_var() &&
            (var->get_belong_array()->is_global() ||
            var->get_belong_array()->is_f_param()))
            {
                event_data=new pair<struct ic_data *,reg_index>(var,reg.first);
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
            }
            virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg.first);
        }
    }
}

void Global_register_manager::handle_SAVE_REGS_WHEN_CALLING_FUNC(struct ic_func * func,list<struct ic_data * > * r_params)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    list<struct ic_data * > * f_params;
    set<reg_index> regs;
    set<struct ic_data * > func_use_global_vars_and_f_params,arrays_need_disposed,non_arrays_need_disposed;
    set<struct ic_data * > * array_members;
    list<struct ic_data * >::iterator f_param,r_param;
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,struct ic_data * > f_to_r_params;
    //处理物理寄存器
    Register_manager::handle_SAVE_REGS_WHEN_CALLING_FUNC(func,r_params);
    //把虚拟寄存器写回内存
    //在函数调用的时候需要写回内存并设置为NOT_DIRTY的DIRTY数据有：
    //（1）被调用的函数可能使用到的全局变量
    //（2）被调用函数可能会用到的数组形参对应的实参的数组取元素
    //（3）和上述两类变量有关的变量
    //（4）传给库函数的所有数组实参所属的数组元素
    if(func)
    {
        switch(func->type)
        {
            case func_type::PROGRAMER_DEFINED:
                f_params=func->f_params;
                if(f_params)
                {
                    f_param=f_params->begin();
                    r_param=r_params->begin();
                    while(f_param!=f_params->end() && r_param!=r_params->end())
                    {
                        f_to_r_params.insert(make_pair(*f_param,*r_param));
                        f_param++;
                        r_param++;
                    }
                }
                func_use_global_vars_and_f_params=symbol_table->get_func_use_globals_and_array_f_params(func);
                for(auto var:func_use_global_vars_and_f_params)
                {
                    if(var->is_array_var())
                    {
                        if(var->is_f_param())
                        {
                            if(f_to_r_params.at(var)->is_array_member())
                            {
                                arrays_need_disposed.insert(f_to_r_params.at(var)->get_belong_array());
                            }
                            else
                            {
                                arrays_need_disposed.insert(f_to_r_params.at(var));
                            }
                        }
                        else
                        {
                            arrays_need_disposed.insert(var);
                        }
                    }
                    else if(var->is_global())
                    {
                        non_arrays_need_disposed.insert(var);
                    }
                }
                break;
            case func_type::LIBRARY:
                //如果要调用的函数是库函数，那么需要把传给该函数的所有数组实参所属的数组元素写回
                if(func->type==func_type::LIBRARY)
                {
                    for(auto r_param:*r_params)
                    {
                        if(r_param->is_array_var())
                        {
                            if(r_param->is_array_member())
                            {
                                arrays_need_disposed.insert(r_param->get_belong_array());
                            }
                            else
                            {
                                arrays_need_disposed.insert(r_param);
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    for(auto array:arrays_need_disposed)
    {
        array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)array)).pointer_data;
        if(array_members)
        {
            for(auto member:*array_members)
            {
                regs=virtual_regs_info_.get_var_value_s_regs(member);
                for(auto reg:regs)
                {
                    if(virtual_regs_info_.is_reg_DIRTY(reg))
                    {
                        event_data=new pair<struct ic_data *,reg_index>(member,reg);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                        virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg);
                    }
                }
            }
        }
    }
    for(auto offset:non_arrays_need_disposed)
    {
        array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET,(void *)offset)).pointer_data;
        if(array_members)
        {
            for(auto member:*array_members)
            {
                regs=virtual_regs_info_.get_var_value_s_regs(member);
                for(auto reg:regs)
                {
                    if(virtual_regs_info_.is_reg_DIRTY(reg))
                    {
                        event_data=new pair<struct ic_data *,reg_index>(member,reg);
                        notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                        delete event_data;
                        virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg);
                    }
                }
            }
        }
    }
    for(auto non_array_need_disposed:non_arrays_need_disposed)
    {
        regs=virtual_regs_info_.get_var_value_s_regs(non_array_need_disposed);
        for(auto reg:regs)
        {
            if(virtual_regs_info_.is_reg_DIRTY(reg))
            {
                event_data=new pair<struct ic_data *,reg_index>(non_array_need_disposed,reg);
                notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                delete event_data;
                virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg);
            }
        }
    }
}

void Global_register_manager::handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC(string func_name,list<struct ic_data * > * r_params)
{
    set<reg_index> regs;
    set<struct ic_data * > * array_members;
    pair<struct ic_data *,reg_index> * event_data;
    Register_manager::handle_SAVE_REGS_WHEN_CALLING_ABI_FUNC(func_name,r_params);
    //对于ABI函数来说，只需要把所有调用的时候的数组实参的所有数组元素进行处理即可
    for(auto r_param:*r_params)
    {
        if(r_param->is_array_var())
        {
            if(r_param->is_array_member())
            {
                r_param=r_param->get_belong_array();
            }
            array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)r_param)).pointer_data;
            if(array_members)
            {
                for(auto member:*array_members)
                {
                    regs=virtual_regs_info_.get_var_value_s_regs(member);
                    for(auto reg:regs)
                    {
                        if(virtual_regs_info_.is_reg_DIRTY(reg))
                        {
                            event_data=new pair<struct ic_data *,reg_index>(member,reg);
                            notify(event(event_type::STORE_VAR_TO_MEM,(void *)event_data));
                            delete event_data;
                            virtual_regs_info_.set_reg_s_value_NOT_DIRTY(reg);
                        }
                    }
                }
            }
        }
    }
}

void Global_register_manager::handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC(list<struct ic_data * > * r_params)
{
    size_t int_r_params_num=0,float_r_params_num=0;
    reg_index argument_reg,v_reg;
    pair<reg_index,reg_index> * event_data;
    for(auto r_param:(*r_params))
    {
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(int_r_params_num<4)
            {
                argument_reg=regs_info_.reg_names.at("r"+to_string(int_r_params_num++));
                v_reg=get_reg_for_reading_var(r_param,reg_processor::CPU);
                event_data=new pair<reg_index,reg_index>(v_reg,argument_reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
                //get_designated_reg_for_reading_var(argument_reg,r_param);
            }
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num<16)
            {
                argument_reg=regs_info_.reg_names.at("s"+to_string(float_r_params_num++));
                v_reg=get_reg_for_reading_var(r_param,reg_processor::VFP);
                event_data=new pair<reg_index,reg_index>(v_reg,argument_reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
                //get_designated_reg_for_reading_var(argument_reg,r_param);
            }
        }
    }
}

void Global_register_manager::handle_PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC(list<struct ic_data * > * r_params,list<reg_index> * r_param_regs)
{
    size_t cpu_r_params_num=0,vfp_r_params_num=0;
    reg_index argument_reg,v_reg;
    list<reg_index>::iterator r_param_reg;
    pair<reg_index,reg_index> * event_data;
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
                //get_designated_reg_for_reading_var((*r_param_reg),r_param);
                v_reg=get_reg_for_reading_var(r_param,reg_processor::CPU);
                event_data=new pair<reg_index,reg_index>(v_reg,(*r_param_reg));
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
            }
            else if(regs_info_.reg_indexs.at(*r_param_reg).processor==reg_processor::VFP && vfp_r_params_num<16)
            {
                vfp_r_params_num++;
                //get_designated_reg_for_reading_var((*r_param_reg),r_param);
                v_reg=get_reg_for_reading_var(r_param,reg_processor::VFP);
                event_data=new pair<reg_index,reg_index>(v_reg,(*r_param_reg));
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
            }
            r_param_reg++;
        }
        else if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(cpu_r_params_num<4)
            {
                argument_reg=regs_info_.reg_names.at("r"+to_string(cpu_r_params_num++));
                //get_designated_reg_for_reading_var(argument_reg,r_param);
                v_reg=get_reg_for_reading_var(r_param,reg_processor::CPU);
                event_data=new pair<reg_index,reg_index>(v_reg,argument_reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
            }
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(vfp_r_params_num<16)
            {
                argument_reg=regs_info_.reg_names.at("s"+to_string(vfp_r_params_num++));
                //get_designated_reg_for_reading_var(argument_reg,r_param);
                v_reg=get_reg_for_reading_var(r_param,reg_processor::VFP);
                event_data=new pair<reg_index,reg_index>(v_reg,argument_reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                delete event_data;
            }
        }
    }
}

void Global_register_manager::handle_RET_FROM_CALLED_FUNC(struct ic_func * func,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    set<reg_index> regs;
    set<struct ic_data * > func_define_global_vars_and_f_params,arrays_need_disposed,non_arrays_need_disposed;
    set<struct ic_data * > * array_members;
    list<struct ic_data * > * f_params;
    list<struct ic_data * >::iterator f_param,r_param;
    map<struct ic_data * ,struct ic_data * > f_to_r_params;
    reg_index v_reg;
    enum reg_processor processor;
    pair<reg_index,reg_index> * event_data;
    Register_manager::handle_RET_FROM_CALLED_FUNC(func,r_params,return_value,return_reg);
    //处理虚拟寄存器堆
    // 把存放如下几种类型的变量的虚拟寄存器设置为无效
    // （1）调用函数可能会改变的全局变量
    // （2）调用函数可能会改变的数组形参对应的实参的所有数组取元素
    // （3）和上述两类变量有关的变量
    if(func)
    {
        f_params=func->f_params;
        if(f_params)
        {
            f_param=f_params->begin();
            r_param=r_params->begin();
            while(f_param!=f_params->end() && r_param!=r_params->end())
            {
                f_to_r_params.insert(make_pair(*f_param,*r_param));
                f_param++;
                r_param++;
            }
        }
        func_define_global_vars_and_f_params=symbol_table->get_func_def_globals_and_array_f_params(func);
        for(auto var:func_define_global_vars_and_f_params)
        {
            if(var->is_array_var())
            {
                if(var->is_f_param())
                {
                    arrays_need_disposed.insert(f_to_r_params.at(var));
                }
                else
                {
                    arrays_need_disposed.insert(var);
                }
            }
            else if(var->is_global())
            {
                non_arrays_need_disposed.insert(var);
            }
        }
    }
    for(auto array:arrays_need_disposed)
    {
        array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)array)).pointer_data;
        if(array_members)
        {
            for(auto member:*array_members)
            {
                regs=virtual_regs_info_.get_var_value_s_regs(member);
                for(auto reg:regs)
                {
                    virtual_regs_info_.set_reg_ineffective(reg);
                }
            }
        }
    }
    for(auto offset:non_arrays_need_disposed)
    {
        array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_MEMBERS_WITH_SPECIFIED_OFFSET,(void *)offset)).pointer_data;
        if(array_members)
        {
            for(auto member:*array_members)
            {
                regs=virtual_regs_info_.get_var_value_s_regs(member);
                for(auto reg:regs)
                {
                    virtual_regs_info_.set_reg_ineffective(reg);
                }
            }
        }
    }
    for(auto non_array_need_disposed:non_arrays_need_disposed)
    {
        regs=virtual_regs_info_.get_var_value_s_regs(non_array_need_disposed);
        for(auto reg:regs)
        {
            virtual_regs_info_.set_reg_ineffective(reg);
        }
    }
    //把函数返回值从物理寄存器转移到虚拟寄存器中
    if(return_value)
    {
        processor=regs_info_.reg_indexs.at(return_reg).processor;
        if(!virtual_regs_info_.check_var_value_own_a_reg(return_value,processor))
        {
            v_reg=allocate_idle_reg(processor);
            virtual_regs_info_.attach_var_value_to_reg(return_value,v_reg);
        }
        else
        {
            v_reg=virtual_regs_info_.get_var_value_s_reg(return_value,processor);
        }
        event_data=new pair<reg_index,reg_index>(return_reg,v_reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
        delete event_data;
        virtual_regs_info_.set_reg_effective(v_reg);
        virtual_regs_info_.set_reg_s_value_DIRTY(v_reg);
        regs_info_.unattach_reg_s_all_data(return_reg);
    }
}

void Global_register_manager::handle_RET_FROM_CALLED_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg)
{
    set<reg_index> regs;
    set<struct ic_data * > * array_members;
    reg_index v_reg;
    enum reg_processor processor;
    pair<reg_index,reg_index> * event_data;
    Register_manager::handle_RET_FROM_CALLED_ABI_FUNC(func_name,r_params,return_value,return_reg);
    //只需要把调用ABI函数的时候的数组实参的所有元素全都设置为无效即可
    for(auto r_param:*r_params)
    {
        if(r_param->is_array_var())
        {
            array_members=(set<struct ic_data * > *)notify(event(event_type::GET_ARRAY_S_ALL_MEMBERS_IN_CURRENT_FUNC,(void *)r_param)).pointer_data;
            if(array_members)
            {
                for(auto member:*array_members)
                {
                    regs=virtual_regs_info_.get_var_value_s_regs(member);
                    for(auto reg:regs)
                    {
                        virtual_regs_info_.set_reg_ineffective(reg);
                    }
                }
            }
        }
    }
    //把函数返回值从物理寄存器转移到虚拟寄存器中
    if(return_value)
    {
        processor=regs_info_.reg_indexs.at(return_reg).processor;
        if(!virtual_regs_info_.check_var_value_own_a_reg(return_value,processor))
        {
            v_reg=allocate_idle_reg(processor);
            virtual_regs_info_.attach_var_value_to_reg(return_value,v_reg);
        }
        else
        {
            v_reg=virtual_regs_info_.get_var_value_s_reg(return_value,processor);
        }
        event_data=new pair<reg_index,reg_index>(return_reg,v_reg);
        notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
        delete event_data;
        virtual_regs_info_.set_reg_effective(v_reg);
        virtual_regs_info_.set_reg_s_value_DIRTY(v_reg);
        regs_info_.unattach_reg_s_all_data(return_reg);
    }
}

void Global_register_manager::handle_ATTACH_CONST_TO_REG(OAA const_data,reg_index reg)
{
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        Register_manager::handle_ATTACH_CONST_TO_REG(const_data,reg);
    }
    else
    {
        virtual_regs_info_.attach_const_value_to_reg(const_data,reg);
    }
}

void Global_register_manager::handle_ATTACH_VAR_VALUE_TO_REG(struct ic_data * var_data,reg_index reg)
{
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        Register_manager::handle_ATTACH_VAR_VALUE_TO_REG(var_data,reg);
    }
    else
    {
        virtual_regs_info_.attach_var_value_to_reg(var_data,reg);
    }
}
void Global_register_manager::handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(struct ic_data * var,reg_index reg)
{
    pair<reg_index,reg_index> * event_data;
    reg_index target_reg;
    enum reg_processor processor;
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        Register_manager::handle_ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY(var,reg);
    }
    else
    {
        before_writing_var_value(var);
        if(!virtual_regs_info_.check_var_value_own_the_reg(var,reg))
        {
            processor=virtual_regs_info_.get_reg_s_processor(reg);
            if(virtual_regs_info_.check_var_value_own_a_reg(var,processor))
            {
                target_reg=virtual_regs_info_.get_var_value_s_reg(var,processor);
            }
            else
            {
                target_reg=allocate_idle_reg(processor);
                virtual_regs_info_.attach_var_value_to_reg(var,target_reg);
            }
            event_data=new pair<reg_index,reg_index>(reg,target_reg);
            notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
            delete event_data;
        }
        virtual_regs_info_.set_reg_s_value_DIRTY(target_reg);
    }
}

void Global_register_manager::handle_UNATTACH_REG_S_ALL_DATA(reg_index reg)
{
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        Register_manager::handle_UNATTACH_REG_S_ALL_DATA(reg);
    }
    else
    {
        virtual_regs_info_.set_reg_ineffective(reg);
    }
}

// void Global_register_manager::handle_DISABLE_ALL_ADDR_REGS_RELATED_WITH_SP()
// {
//     Register_manager::handle_DISABLE_ALL_ADDR_REGS_RELATED_WITH_SP();
//     for(auto reg:virtual_regs_info_.reg_indexs)
//     {
//         if(reg.second.data_type==virtual_related_data_type::VAR_ADDR)
//         {
//             virtual_regs_info_.set_reg_ineffective(reg.first);
//         }
//     }
// }

void Global_register_manager::handle_DEAL_WITH_PARMAS_IN_ARGUMENT_REGS()
{
    reg_index target_reg;
    pair<reg_index,reg_index> * event_data;
    map<struct ic_data *,enum reg_var_state> temp;
    //把函数开始的时候处于参数寄存器中的参数转移到虚拟寄存器中，并将这些参数寄存器设置为无效，把虚拟寄存器设置为脏值
    for(auto & reg_info:regs_info_.reg_indexs)
    {
        if(reg_info.second.attr==reg_attr::ARGUMENT && reg_info.second.state==reg_state::USED)
        {
            temp=reg_info.second.var_datas;
            for(auto var_data:temp)
            {
                if(var_data.second==reg_var_state::DIRTY)
                {
                    if(!virtual_regs_info_.check_var_value_own_a_reg(var_data.first,reg_info.second.processor))
                    {
                        target_reg=allocate_idle_reg(reg_info.second.processor);
                        virtual_regs_info_.attach_var_value_to_reg(var_data.first,target_reg);
                    }
                    else
                    {
                        target_reg=virtual_regs_info_.get_var_value_s_reg(var_data.first,reg_info.second.processor);
                        if(virtual_regs_info_.is_reg_effective(target_reg))
                        {
                            virtual_regs_info_.set_reg_effective(target_reg);
                        }
                    }
                    event_data=new pair<reg_index,reg_index>(reg_info.first,target_reg);
                    notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data));
                    delete event_data;
                    reg_info.second.remove_value(var_data.first);
                    virtual_regs_info_.set_reg_s_value_DIRTY(target_reg);
                }
            }
        }
    }
}

struct event Global_register_manager::handle_IS_CPU_REG(reg_index reg)
{
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        return Register_manager::handle_IS_CPU_REG(reg);
    }
    return event(event_type::RESPONSE_BOOL,virtual_regs_info_.get_reg_s_processor(reg)==reg_processor::CPU);
}

struct event Global_register_manager::handle_IS_VFP_REG(reg_index reg)
{
    if(virtual_regs_info_.is_physical_reg(reg))
    {
        return Register_manager::handle_IS_VFP_REG(reg);
    }
    return event(event_type::RESPONSE_BOOL,virtual_regs_info_.get_reg_s_processor(reg)==reg_processor::VFP);
}

struct event Global_register_manager::handle_CHECK_CONST_VALUE_OWN_CPU_REG(OAA const_value)
{
    //先对虚拟寄存器进行查看
    if(virtual_regs_info_.check_const_value_own_a_reg(const_value,reg_processor::CPU))
    {
        return event(event_type::RESPONSE_BOOL,true);
    }
    //再对物理寄存器进行查看
    return Register_manager::handle_CHECK_CONST_VALUE_OWN_CPU_REG(const_value);
}

struct event Global_register_manager::handle_GET_CONST_VALUE_S_CPU_REG(OAA const_value)
{
    reg_index res;
    //先对虚拟寄存器堆进行查看
    if(virtual_regs_info_.check_const_value_own_a_reg(const_value,reg_processor::CPU))
    {
        return event(event_type::RESPONSE_INT,(int)virtual_regs_info_.get_const_value_s_reg(const_value,reg_processor::CPU));
    }
    //再对物理寄存器堆进行查看
    return Register_manager::handle_GET_CONST_VALUE_S_CPU_REG(const_value);
}

struct event Global_register_manager::handle_ALLOCATE_IDLE_CPU_REG()
{
    return event(event_type::RESPONSE_INT,(int)allocate_idle_reg(reg_processor::CPU));
}

struct event Global_register_manager::handle_GET_CPU_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::CPU));
}

struct event Global_register_manager::handle_GET_AN_EMPTY_CPU_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::CPU,true));
}

struct event Global_register_manager::handle_GET_VFP_REG_FOR_CONST(OAA const_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_const(const_data,reg_processor::VFP));
}

struct event Global_register_manager::handle_GET_CPU_REG_FOR_READING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_reading_var(var_data,reg_processor::CPU));
}

struct event Global_register_manager::handle_GET_CPU_REG_FOR_WRITING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_writing_var(var_data,reg_processor::CPU));
}

struct event Global_register_manager::handle_GET_VFP_REG_FOR_READING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_reading_var(var_data,reg_processor::VFP));
}

struct event Global_register_manager::handle_GET_VFP_REG_FOR_WRITING_VAR(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_writing_var(var_data,reg_processor::VFP));
}

struct event Global_register_manager::handle_GET_ADDR_REG(struct ic_data * var_data)
{
    return event(event_type::RESPONSE_INT,(int)get_reg_for_var_addr(var_data,reg_processor::CPU));
}

struct event Global_register_manager::handle_IS_REG_EFFECTIVE(reg_index reg)
{
    return event(event_type::RESPONSE_BOOL,virtual_regs_info_.is_reg_effective(reg));
}

//==========================================================================//