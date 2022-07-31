/*
 *
 *  local_register_manager.cpp
 *  局部寄存器分配管理器
 *  Creator:Sichao Chen
 *  Create time:2022/7/6
 *
*/
#include"local_register_manager.h"

Local_register_manager::Local_register_manager(set<struct reg> regs,struct flag_reg flag_reg):Register_manager(regs,flag_reg)
{

}

Local_register_manager::~Local_register_manager()
{

}

void Local_register_manager::handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    ;//什么也不用做
}

void Local_register_manager::handle_END_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    handle_END_BASIC_BLOCK_WITHOUT_FLAG(basic_block);
    handle_CLEAR_FLAG();
}

void Local_register_manager::handle_END_BASIC_BLOCK_WITHOUT_FLAG(struct ic_basic_block * basic_block)
{
    static struct ic_basic_block * last_end_basic_block=nullptr;
    set<struct ic_data * > written_back_vars;
    pair<struct ic_data *,reg_index> * event_data;
    map<struct ic_data * ,enum reg_var_state> temp;
    if(basic_block==last_end_basic_block)
    {
        return;
    }
    last_end_basic_block=basic_block;
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
                if(var_data.second==reg_var_state::DIRTY && (!var_data.first->is_tmp_var() || notify(event(event_type::IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,(void *)var_data.first)).bool_data))
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