/*
 *
 *  ic_global_optimize.cpp
 *  中间代码全局优化
 *  Creator:Sichao Chen
 *  Create time:2022/8/15
 *
*/
#include"ic_global_optimize.h"
#include"data_flow_analyse.h"
#include"ic_local_optimize.h"
#include"pool.h"

/*
全局复制传播

Parameters
----------
func:要优化的函数流图
*/
void global_copy_progagation(struct ic_func_flow_graph * func)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    struct quaternion intermediate_code;
    struct ic_data * arg1,* res;
    ic_pos pos;
    OAA tmp;
    bool tag;
    for(auto basic_block:func->basic_blocks)
    {
        for(auto & ic_with_info:basic_block->ic_sequence)
        {
            tag=false;
            for(auto use:ic_with_info.uses)
            {
                if(ic_with_info.ud_chain.find(use)!=ic_with_info.ud_chain.end() && ic_with_info.ud_chain.at(use).size()==1)
                {
                    pos=*(ic_with_info.ud_chain.at(use).begin());
                    intermediate_code=func->get_ic_with_info(pos).intermediate_code;
                    arg1=((struct ic_data *)intermediate_code.arg1.second);
                    res=((struct ic_data *)intermediate_code.result.second);
                    if(intermediate_code.op==ic_op::ASSIGN && arg1->is_const() && res==use)
                    {
                        if(use->get_data_type()!=arg1->get_data_type())
                        {
                            tmp=arg1->get_value();
                            tmp.type_conversion(arg1->get_data_type(),use->get_data_type());
                            arg1=symbol_table->const_entry(use->get_data_type(),tmp);
                        }
                        if((use->is_global() || use->is_array_member() || use->is_f_param()) && (!func->is_dominated(basic_block,pos.basic_block) || basic_block==pos.basic_block))
                        {
                            continue;
                        }
                        ic_with_info.replace_datas(use,arg1,true,false);
                        tag=true;
                    }
                }
            }
            if(tag)
            {
                ic_with_info.build_info(false);
            }
        }
    }
}

/*
全局公共子表达式删除

Parameters
----------
func:要优化的函数流图
*/
void global_elimination_of_common_subexpression(struct ic_func_flow_graph * func)
{
    // printf("func < %s >\n", func->func->name.c_str());

    map< ic_basic_block*, pool* > blk_to_pool;   // 基本块的池, first 是入口池, second 是出口池
    ic_basic_block* blk;
    pool *pn_in, *pn_out, *sp, *new_sp, *jp, *new_jp;

    for( auto blk:func->basic_blocks ){
        blk_to_pool[blk] = new pool(blk);
    }

    blk_to_pool.at(func->basic_blocks.front())->set_pool_old();
    list< ic_basic_block* > proc_list = {func->basic_blocks.front()};   // 第一个基本块入队列

    while( !proc_list.empty() ){

        blk = proc_list.front();
        pn_in = blk_to_pool.at(blk);
        pn_out = pn_in->interation();         // 块内迭代, 得到 pn_out

        // printf("\tblk:%lx, in_size = %d, out_size = %d\n", blk, blk_to_pool[blk]->get_pi_count(), pn_out->get_pi_count());
        
        // 更新顺序后一块的入口池
        // printf("\n\tsequential_next  %lx\n", blk->sequential_next);
        if( blk->sequential_next != nullptr ){
            sp = blk_to_pool.at(blk->sequential_next);
            new_sp = sp->pool_intersection( pn_out );
            if ( !sp->pool_same_judge(new_sp) ){
                sp->~pool();        // 销毁旧池
                blk_to_pool.at(blk->sequential_next) = new_sp;  // 更新入口池
                proc_list.push_back(blk->sequential_next);      // 将后一个块入队列
            }else{
                new_sp->~pool();    // 销毁新池
            }
        }

        // 更新跳转后一块的入口池
        // printf("\n\tjump_next  %lx\n", blk->jump_next);
        if( blk->jump_next != nullptr ){
            jp = blk_to_pool.at(blk->jump_next);
            new_jp = jp->pool_intersection( pn_out );
            if ( !jp->pool_same_judge(new_jp) ){
                jp->~pool();        // 销毁旧池
                blk_to_pool.at(blk->jump_next) = new_jp;        // 更新入口池
                proc_list.push_back(blk->jump_next);            // 将后一个块入队列
            }else{
                new_jp->~pool();    // 销毁新池
            }
        }
        pn_out->~pool();    // 释放 pn_out

        proc_list.pop_front();  // 将 blk 从队列移除
    }

    int sum = 0;
    for( auto blk:func->basic_blocks ){
        // printf("pool for %lx is new %d\n", blk, blk_to_pool.at(blk)->is_new());
        sum += eliminate_common_exp_in_basic_block(blk, blk_to_pool.at(blk));  // 进行公共子表达式消除
    }

    // 释放 本 函数相关的池
    for( auto blk:func->basic_blocks ){
        blk_to_pool.at(blk)->~pool();
    }

    // printf("out func < %s > sum = %d\n", func->func->name.c_str(), sum);
}

/*
全局死代码消除

Parameters
----------
func:要优化的函数流图
*/
void global_dead_code_elimination(struct ic_func_flow_graph * func)
{
    struct quaternion qua;
    for(auto basic_block:func->basic_blocks)
    {
        for(vector<struct quaternion_with_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            switch((*ic_with_info).intermediate_code.op)
            {
                case ic_op::ASSIGN:
                case ic_op::NOT:
                case ic_op::ADD:
                case ic_op::SUB:
                case ic_op::MUL:
                case ic_op::DIV:
                case ic_op::MOD:
                case ic_op::EQ:
                case ic_op::UEQ:
                case ic_op::GT:
                case ic_op::LT:
                case ic_op::GE:
                case ic_op::LE:
                    if(!(*ic_with_info).check_if_def_global_or_f_param_array() && 
                    (*ic_with_info).du_chain.empty())
                    {
                        (*ic_with_info)=quaternion_with_info();
                    }
                    break;
                case ic_op::CALL:
                    // if((*ic_with_info).explicit_def && 
                    // (*ic_with_info).explicit_def->is_tmp_var() && 
                    // (*ic_with_info).du_chain.find((*ic_with_info).explicit_def)==(*ic_with_info).du_chain.end())
                    // {
                    //     qua=(*ic_with_info).intermediate_code;
                    //     (*ic_with_info)=quaternion_with_info(quaternion(qua.op,qua.arg1.first,qua.arg1.second,qua.arg2.first,qua.arg2.second,ic_operand::NONE,nullptr));
                    // }
                    // break;
                case ic_op::NOP:
                case ic_op::JMP:
                case ic_op::IF_JMP:
                case ic_op::IF_NOT_JMP:
                case ic_op::VAR_DEFINE:
                case ic_op::LABEL_DEFINE:
                case ic_op::FUNC_DEFINE:
                case ic_op::END_FUNC_DEFINE:
                case ic_op::RET:
                    break;
                default:
                    break;
            }
        }
    }
}

/*
循环不变量外提

Parameters
----------
func:要优化的函数流图
*/
void loop_invariant_computation_motion(struct ic_func_flow_graph * func)
{
    set<struct ic_basic_block * > bbs_in_loop,precursors,temp;
    struct ic_basic_block * target_basic_block;
    ic_pos current_pos;
    size_t pos;
    set<ic_pos> unchange_poses;
    list<ic_pos> ordered_unchange_poses;
    list<struct quaternion_with_info> target_ic_with_infos;
    bool tag,tag_1,tag_2;
    //对每一个循环分别进行操作
    for(auto loop:func->loops_info)
    {
        //首先进行循环不变量语句的检测
        unchange_poses.clear();
        ordered_unchange_poses.clear();
        target_basic_block=nullptr;
        //如果语句s的运算对象或者是常数，或者它们的所有到达-定义都在循环L的外面，那么将语句s标记为“不变”
        for(auto bb:loop.second->all_basic_blocks)
        {
            pos=0;
            for(auto ic_with_info:bb->ic_sequence)
            {
                if(ic_with_info.intermediate_code.get_ic_op_type()!=ic_op_type::DATA_PROCESS)
                {
                    goto next_1;
                }
                current_pos=ic_pos(bb,pos);
                for(auto use:ic_with_info.uses)
                {
                    if(!use->is_const() && ic_with_info.ud_chain.find(use)!=ic_with_info.ud_chain.end())
                    {
                        for(auto define_pos:ic_with_info.ud_chain.at(use))
                        {
                            if(loop.second->all_basic_blocks.find(define_pos.basic_block)!=loop.second->all_basic_blocks.end())
                            {
                                goto next_1;
                            }
                        }
                    }
                }
                unchange_poses.insert(current_pos);
                ordered_unchange_poses.push_back(current_pos);
next_1:
                pos++;
            }
        }
        tag=true;
        //如果语句s先前没有被标记
        //并且s的所有运算对象或者是常数，或者所有到达-定义都在循环L的外面，或者只有一个到达定义，这个定义是循环L中已标记为“不变”的语句
        while(tag)
        {
            tag=false;
            for(auto bb:loop.second->all_basic_blocks)
            {
                pos=0;
                for(auto ic_with_info:bb->ic_sequence)
                {
                    if(unchange_poses.find(current_pos)!=unchange_poses.end() || ic_with_info.intermediate_code.get_ic_op_type()!=ic_op_type::DATA_PROCESS)
                    {
                        goto next_2;
                    }
                    current_pos=ic_pos(bb,pos);
                    for(auto use:ic_with_info.uses)
                    {
                        if(!use->is_const() && ic_with_info.ud_chain.find(use)!=ic_with_info.ud_chain.end())
                        {
                            if(ic_with_info.ud_chain.at(use).size()!=1)
                            {
                                goto next_2;
                            }
                            if(unchange_poses.find((*ic_with_info.ud_chain.at(use).begin()))==unchange_poses.end())
                            {
                                goto next_2;
                            }
                        }
                    }
                    if(unchange_poses.find(current_pos)==unchange_poses.end())
                    {
                        tag=true;
                        unchange_poses.insert(current_pos);
                        ordered_unchange_poses.push_back(current_pos);
                    }
next_2:
                    pos++;
                }
            }
        }
        //检测完毕之后开始进行外提
        for(auto unchange_pos:ordered_unchange_poses)
        {
            struct quaternion_with_info & ic_with_info=func->get_ic_with_info(unchange_pos);
            //目前暂时不处理对全局变量，数组和数组元素的明确定义，也不处理函数调用
            if(ic_with_info.intermediate_code.op==ic_op::CALL || ic_with_info.explicit_def->is_array_member() || ic_with_info.explicit_def->is_array_var() || ic_with_info.explicit_def->is_global())
            {
                goto next_3;
            }
            // if(!ic_with_info.explicit_def->is_tmp_var())
            // {
            //     goto next_3;
            // }
            // //将要外提的语句s所在的基本块必须能够达到
            // if(unchange_pos.basic_block->get_precursors().empty())
            // {
            //     goto next_3;
            // }
            //要么将要外提的语句s所在的块支配循环L的所有出口
            tag_1=true;
            tag_2=true;
            for(auto loop_exit:loop.second->exit_basic_blocks)
            {
                if(!func->is_dominated(loop_exit,unchange_pos.basic_block))
                {
                    tag_1=false;
                    break;
                }
            }
            //要么将要外提的语句s定义的变量x在循环的外面没有被使用，并且在循环内部的所有x的使用，只有可能来自s的定义
            if(ic_with_info.du_chain.find(ic_with_info.explicit_def)!=ic_with_info.du_chain.end())
            {
                for(auto use_pos:ic_with_info.du_chain.at(ic_with_info.explicit_def))
                {
                    if(loop.second->all_basic_blocks.find(use_pos.basic_block)==loop.second->all_basic_blocks.end() || 
                    *(func->get_ic_with_info(use_pos).ud_chain.at(ic_with_info.explicit_def).begin())!=unchange_pos || 
                    func->get_ic_with_info(use_pos).ud_chain.at(ic_with_info.explicit_def).size()!=1)
                    {
                        tag_2=false;
                        break;
                    }
                }
            }
            if(!tag_1 && !tag_2)
            {
                goto next_3;
            }
            for(auto i:loop.second->all_basic_blocks)
            {
                pos=0;
                for(auto j:i->ic_sequence)
                {
                    current_pos=ic_pos(i,pos);
                    //s定义的变量x在L种没有别处的定义
                    if(current_pos!=unchange_pos && j.explicit_def==ic_with_info.explicit_def || j.vague_defs.find(ic_with_info.explicit_def)!=j.vague_defs.end())
                    {
                        goto next_3;
                    }
                    //L中所有x的引用只能由s中x的定义到达
                    if(j.ud_chain.find(ic_with_info.explicit_def)!=j.ud_chain.end() && (j.ud_chain.at(ic_with_info.explicit_def).size()!=1 || (*j.ud_chain.at(ic_with_info.explicit_def).begin())!=unchange_pos))
                    {
                        goto next_3;
                    }
                    pos++;
                }
            }
            if(target_basic_block==nullptr)
            {
                precursors=loop.first->get_precursors();
                temp.clear();
                set_difference(precursors.begin(),precursors.end(),loop.second->all_basic_blocks.begin(),loop.second->all_basic_blocks.end(),inserter(temp,temp.begin()));
                if(temp.size()==1 && (*temp.begin())->get_successors().size()==1)
                {
                    target_basic_block=(*temp.begin());
                }
                else
                {
                    target_basic_block=new struct ic_basic_block(func);
                    for(auto pre_bb:temp)
                    {
                        if(pre_bb->jump_next==loop.first)
                        {
                            pre_bb->set_jump_next(target_basic_block);
                        }
                        if(pre_bb->sequential_next==loop.first)
                        {
                            pre_bb->set_sequential_next(target_basic_block);
                        }
                    }
                    target_basic_block->set_jump_next(nullptr);
                    target_basic_block->set_sequential_next(loop.first);
                    for(list<struct ic_basic_block * >::iterator basic_block=func->basic_blocks.begin();basic_block!=func->basic_blocks.end();basic_block++)
                    {
                        if(loop.first==(*basic_block))
                        {
                            func->basic_blocks.insert(basic_block,target_basic_block);
                        }
                    }
                    for(auto & loop_info:func->loops_info)
                    {
                        if(loop_info.first!=loop.first && loop_info.second->all_basic_blocks.find(loop.first)!=loop_info.second->all_basic_blocks.end())
                        {
                            loop_info.second->all_basic_blocks.insert(target_basic_block);
                        }
                    }
                }
            }
            target_basic_block->add_ic(ic_with_info.intermediate_code,true);
            ic_with_info=quaternion_with_info();
            //外提的时候更改ud链和du链的信息
            current_pos=ic_pos(target_basic_block,target_basic_block->ic_sequence.size()-1);
            // Data_flow_analyzer::change_ud_chain(func,unchange_pos,current_pos);
            // Data_flow_analyzer::change_du_chain(func,unchange_pos,current_pos);
            Data_flow_analyzer::data_flow_analysis_for_a_func(func,false);
            //同时需要修改变量的定义点和使用点的信息
            //TO-DO
next_3:
            ;
        }
    }
}

/*
归纳变量删除

Parameters
----------
func:要优化的函数流图
*/
void induction_variable_elimination(struct ic_func_flow_graph * func)
{

}

/*
循环展开

Parameters
----------
func:要优化的函数流图
*/
void loop_unroll(struct ic_func_flow_graph * func)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    size_t offset;
    ic_pos loop_var_self_op_pos,pos;
    struct ic_basic_block * first_bb,* second_bb,* pre_bb,* next_bb,* target_bb;
    struct ic_data * loop_var,* upper_limit_var,* lower_limit_var,* arg1,* arg2,* const_data;
    int upper_limit,lower_limit,i;
    struct ic_label * out_label;
    struct quaternion_with_info label_define,exit_loop_judge,conditional_jump,assign_const_to_loop_var,loop_var_self_op,new_ic;
    list<struct quaternion_with_info> unrolled_ics_before_self_op,unrolled_ics_after_self_op;
    map<struct ic_data *,struct ic_data * > old_and_new_vars_map;
    map<struct ic_basic_block * ,struct quaternion_with_info> first_bbs;
    set<struct ic_basic_block *> second_bbs,target_bbs;
    bool tag;
    for(auto loop_info:func->loops_info)
    {
        //暂时只对满足如下条件的循环进行优化：
        //只对类似如下格式的循环进行优化：
        //     ......
        //     ASSIGN   0 --> i
        //     ......
        // -----------------------------------------BASIC_BLOCK:0x6000016d81c0-----------------------------------------
        // .0 : 
        //     LT		i , 20 --> (int)@9
        //     IF_NOT_JMP		@9 --> .1
        // -------------------------------------NEXT:0x6000016d82a0,0x6000016d8380-------------------------------------
        // -----------------------------------------BASIC_BLOCK:0x6000016d82a0-----------------------------------------
        //     ......
        //     ADD		i , 1 --> i
        //     ......
        //     JMP		.0
        // --------------------------------------------NEXT:0x6000016d81c0---------------------------------------------
        // -----------------------------------------BASIC_BLOCK:0x6000016d8380-----------------------------------------
        // .1 : 
        if(!loop_info.second->children_loops.empty() || 
        loop_info.second->all_basic_blocks.size()>2)
        {
            continue;
        }
        first_bb=loop_info.first;
        for(auto ic_with_info:first_bb->ic_sequence)
        {
            switch(ic_with_info.intermediate_code.op)
            {
                case ic_op::NOP:
                    break;
                case ic_op::LABEL_DEFINE:
                    if(label_define.intermediate_code.op==ic_op::NOP && 
                    exit_loop_judge.intermediate_code.op==ic_op::NOP && 
                    conditional_jump.intermediate_code.op==ic_op::NOP)
                    {
                        label_define=ic_with_info;
                    }
                    else
                    {
                        goto next;
                    }
                    break;
                case ic_op::LT:
                    if(label_define.intermediate_code.op!=ic_op::NOP && 
                    exit_loop_judge.intermediate_code.op==ic_op::NOP && 
                    conditional_jump.intermediate_code.op==ic_op::NOP)
                    {
                        exit_loop_judge=ic_with_info;
                    }
                    else
                    {
                        goto next;
                    }
                    break;
                case ic_op::IF_NOT_JMP:
                    if(label_define.intermediate_code.op!=ic_op::NOP && 
                    exit_loop_judge.intermediate_code.op!=ic_op::NOP && 
                    conditional_jump.intermediate_code.op==ic_op::NOP)
                    {
                        conditional_jump=ic_with_info;
                    }
                    else
                    {
                        goto next;
                    }
                    break;
                default:
                    goto next;
                    break;
            }
        }
        arg1=(struct ic_data * )exit_loop_judge.intermediate_code.arg1.second;
        arg2=(struct ic_data * )exit_loop_judge.intermediate_code.arg2.second;
        if(exit_loop_judge.intermediate_code.result.second!=conditional_jump.intermediate_code.arg1.second || 
        arg1->is_const() || 
        arg1->is_array_member() || 
        arg1->is_global() || 
        arg1->get_data_type()!=language_data_type::INT || 
        !arg2->is_const() || 
        exit_loop_judge.ud_chain.find(arg1)==exit_loop_judge.ud_chain.end() || 
        exit_loop_judge.ud_chain.at(arg1).size()!=2 || 
        (*(exit_loop_judge.ud_chain.at(arg1).begin())).basic_block==(*(++exit_loop_judge.ud_chain.at(arg1).begin())).basic_block)
        {
            goto next;
        }
        loop_var=arg1;
        upper_limit_var=arg2;
        upper_limit=upper_limit_var->get_value().int_data;
        out_label=(struct ic_label *)conditional_jump.intermediate_code.result.second;
        for(auto bb_in_loop:loop_info.second->all_basic_blocks)
        {
            if(bb_in_loop!=first_bb)
            {
                second_bb=bb_in_loop;
            }
        }
        if(second_bb->sequential_next || 
        second_bb->jump_next!=first_bb)
        {
            goto next;
        }
        for(auto def_pos:exit_loop_judge.ud_chain.at(loop_var))
        {
            if(def_pos.basic_block==second_bb)
            {
                loop_var_self_op=func->get_ic_with_info(def_pos);
                loop_var_self_op_pos=def_pos;
            }
            else
            {
                assign_const_to_loop_var=func->get_ic_with_info(def_pos);
            }
        }
        if(assign_const_to_loop_var.intermediate_code.op!=ic_op::ASSIGN || 
        !((struct ic_data *)assign_const_to_loop_var.intermediate_code.arg1.second)->is_const() || 
        assign_const_to_loop_var.intermediate_code.result.second!=loop_var)
        {
            goto next;
        }
        lower_limit_var=(struct ic_data *)assign_const_to_loop_var.intermediate_code.arg1.second;
        lower_limit=lower_limit_var->get_value().int_data;
        if(loop_var_self_op.intermediate_code.op!=ic_op::ADD || 
        loop_var_self_op.intermediate_code.arg1.second!=loop_var ||
        loop_var_self_op.intermediate_code.result.second!=loop_var || 
        !((struct ic_data *)loop_var_self_op.intermediate_code.arg2.second)->is_const() || 
        ((struct ic_data *)loop_var_self_op.intermediate_code.arg2.second)->get_value().int_data!=1)
        {
            goto next;
        }
        pre_bb=nullptr;
        next_bb=nullptr;
        for(auto bb:func->basic_blocks)
        {
            if(bb==first_bb)
            {
                if(!pre_bb || 
                pre_bb->get_successors().size()!=1 || 
                pre_bb->sequential_next!=first_bb)
                {
                    goto next;
                }
                else
                {
                    target_bb=pre_bb;
                }
            }
            else if(pre_bb==second_bb && 
            (bb->get_precursors().size()!=1 || 
            (*bb->get_precursors().begin())!=first_bb))
            {
                goto next;
            }
            pre_bb=bb;
        }
        offset=0;
        tag=true;
        for(auto ic_with_info:second_bb->ic_sequence)
        {
            pos=ic_pos(second_bb,offset++);
            if(ic_with_info.explicit_def && ic_with_info.explicit_def==loop_var)
            {
                if(pos!=loop_var_self_op_pos)
                {
                    goto next;
                }
                else
                {
                    tag=false;
                }
            }
            else if(ic_with_info.intermediate_code.op!=ic_op::JMP)
            {
                if(tag)
                {
                    unrolled_ics_before_self_op.push_back(ic_with_info);
                }
                else
                {
                    unrolled_ics_after_self_op.push_back(ic_with_info);
                }
            }
        }
        if((unrolled_ics_before_self_op.size()+unrolled_ics_after_self_op.size())>20 || 
        (upper_limit-lower_limit)>30)
        {
            goto next;
        }
        old_and_new_vars_map.insert(make_pair(loop_var,nullptr));
        for(i=lower_limit;i<upper_limit;i++)
        {
            const_data=symbol_table->const_entry(language_data_type::INT,OAA((int)i));
            old_and_new_vars_map.at(loop_var)=const_data;
            for(auto ic_with_info:unrolled_ics_before_self_op)
            {
                new_ic=ic_with_info;
                new_ic.replace_all_vars(old_and_new_vars_map);
                target_bb->ic_sequence.push_back(new_ic);
            }
            const_data=symbol_table->const_entry(language_data_type::INT,OAA((int)(i+1)));
            old_and_new_vars_map.at(loop_var)=const_data;
            for(auto ic_with_info:unrolled_ics_after_self_op)
            {
                new_ic=ic_with_info;
                new_ic.replace_all_vars(old_and_new_vars_map);
                target_bb->ic_sequence.push_back(new_ic);
            }
        }
        const_data=symbol_table->const_entry(language_data_type::INT,OAA((int)i));
        target_bb->ic_sequence.push_back(quaternion_with_info(quaternion(ic_op::ASSIGN,ic_operand::DATA,(void *)const_data,ic_operand::NONE,nullptr,ic_operand::DATA,(void *)loop_var)));
        first_bbs.insert(make_pair(first_bb,label_define));
        second_bbs.insert(second_bb);
        target_bbs.insert(target_bb);
next:
        unrolled_ics_before_self_op.clear();
        unrolled_ics_after_self_op.clear();
        old_and_new_vars_map.clear();
        label_define=quaternion_with_info();
        exit_loop_judge=quaternion_with_info();
        conditional_jump=quaternion_with_info();
        assign_const_to_loop_var=quaternion_with_info();
        loop_var_self_op=quaternion_with_info();
    }
    if(!first_bbs.empty() && !second_bbs.empty())
    {
        for(auto bb_and_ic:first_bbs)
        {
            bb_and_ic.first->ic_sequence.clear();
            bb_and_ic.first->ic_sequence.push_back(bb_and_ic.second);
        }
        for(auto bb:second_bbs)
        {
            bb->ic_sequence.clear();
        }
        for(auto bb:target_bbs)
        {
            DAG_optimize(bb);
        }
        //需要重新进行数据流分析
        Data_flow_analyzer::data_flow_analysis_for_a_func(func,true);
    }
}

void globalize_local_arrays(struct ic_flow_graph * intermediate_codes_flow_graph,struct ic_func_flow_graph * func)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    map<struct ic_data *,set<ic_pos> > arrays_def_poses,arrays_use_poses;
    map<struct ic_data *,struct ic_data * > old_new_vars_map;
    map<size_t,OAA> array_members_values;
    vector<OAA> * new_initial_array_members_values;
    struct quaternion_with_info array_def_ic,target_ic;
    ic_pos pos;
    size_t offset,array_member_nums;
    struct ic_data * array_var,* new_array_var,* arg1,* result;
    struct ic_basic_block * bb;
    for(auto bb:func->basic_blocks)
    {
        offset=0;
        for(auto ic_with_info:bb->ic_sequence)
        {
            pos=ic_pos(bb,offset++);
            if(ic_with_info.intermediate_code.op==ic_op::VAR_DEFINE && ((struct ic_data *)ic_with_info.intermediate_code.result.second)->is_array_var())
            {
                map_set_insert(arrays_def_poses,((struct ic_data *)ic_with_info.intermediate_code.result.second),pos);
            }
            else if(ic_with_info.explicit_def && ic_with_info.explicit_def->is_array_member())
            {
                map_set_insert(arrays_def_poses,ic_with_info.explicit_def->get_belong_array(),pos);
            }
            for(auto vague_def:ic_with_info.vague_defs)
            {
                if(vague_def->is_array_member())
                {
                    map_set_insert(arrays_def_poses,vague_def->get_belong_array(),pos);
                }
                else if(vague_def->is_array_var())
                {
                    map_set_insert(arrays_def_poses,vague_def,pos);
                }
            }
            for(auto use:ic_with_info.uses)
            {
                if(use->is_array_member())
                {
                    map_set_insert(arrays_use_poses,use->get_belong_array(),pos);
                }
                else if(use->is_array_var())
                {
                    map_set_insert(arrays_use_poses,use,pos);
                }
            }
        }
    }
    for(auto array_def_poses:arrays_def_poses)
    {
        if(array_def_poses.first->is_global() || array_def_poses.first->is_f_param())
        {
            continue;
        }
        bb=nullptr;
        for(auto def_pos:array_def_poses.second)
        {
            if(bb && def_pos.basic_block!=bb)
            {
                break;
            }
            array_def_ic=func->get_ic_with_info(def_pos);
            switch(array_def_ic.intermediate_code.op)
            {
                case ic_op::VAR_DEFINE:
                    break;
                case ic_op::ASSIGN:
                    arg1=(struct ic_data * )array_def_ic.intermediate_code.arg1.second;
                    result=(struct ic_data * )array_def_ic.intermediate_code.result.second;
                    if(!result->get_offset()->is_const() || 
                    !arg1->is_const() || 
                    array_members_values.find((size_t)result->get_offset()->get_value().int_data)!=array_members_values.end())
                    {
                        goto next;
                    }
                    array_members_values.insert(make_pair((size_t)result->get_offset()->get_value().int_data,arg1->get_value()));
                    break;
                default:
                    goto next;
                    break;
            }
        }
        array_member_nums=1;
        for(auto dimension:*array_def_poses.first->dimensions_len)
        {
            if(dimension->is_const())
            {
                array_member_nums*=dimension->get_value().int_data;
            }
        }
        if(array_member_nums>100)
        {
            goto next;
        }
        new_initial_array_members_values=new vector<OAA>(array_member_nums);
        switch(array_def_poses.first->get_data_type())
        {
            case language_data_type::INT:
                for(auto & value:*new_initial_array_members_values)
                {
                    value=OAA((int)0);
                }
                break;
            case language_data_type::FLOAT:
                for(auto & value:*new_initial_array_members_values)
                {
                    value=OAA((float)0);
                }
                break;
            default:
                break;
        }
        for(auto array_member_value:array_members_values)
        {
            new_initial_array_members_values->at(array_member_value.first)=array_member_value.second;
        }
        for(auto def_pos:array_def_poses.second)
        {
            struct quaternion_with_info & def_ic=func->get_ic_with_info(def_pos);
            def_ic=quaternion_with_info();
        }
        if(arrays_use_poses.find(array_def_poses.first)!=arrays_use_poses.end())
        {
            new_array_var=symbol_table->new_var(symbol_table->new_l2g_array_name(),array_def_poses.first->get_data_type(),array_def_poses.first->dimensions_len,OAA((void *)new_initial_array_members_values),false,symbol_table->get_global_scope());
            old_new_vars_map.insert(make_pair(array_def_poses.first,new_array_var));
            for(auto use_pos:arrays_use_poses.at(array_def_poses.first))
            {
                struct quaternion_with_info & use_ic=func->get_ic_with_info(use_pos);
                use_ic.replace_all_vars(old_new_vars_map);
            }
            intermediate_codes_flow_graph->global_defines.push_back(quaternion(ic_op::VAR_DEFINE,ic_operand::NONE,nullptr,ic_operand::NONE,nullptr,ic_operand::DATA,(void *)new_array_var));
        }
next:
        old_new_vars_map.clear();
        array_members_values.clear();
    }
}