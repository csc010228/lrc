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