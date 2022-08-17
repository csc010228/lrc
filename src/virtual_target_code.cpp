/*
 *
 *  virtual_target_code.cpp
 *  虚拟目标代码
 *  Creator:Sichao Chen
 *  Create time:2022/8/17
 *
*/
#include"virtual_target_code.h"

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



//===================================== struct live_analysis =====================================//

void live_analysis::clear()
{
    basic_block_s_live_analysis_infos.clear();
}

//==========================================================================//



//===================================== struct virutal_reg_s_live_interval =====================================//

virutal_reg_s_live_interval::virutal_reg_s_live_interval():is_extending(false),score(0)
{

}

void virutal_reg_s_live_interval::start(virtual_target_code_pos start_pos,virtual_target_code_pos end_pos)
{
    is_extending=true;
    live_interval.push_front(make_pair(start_pos,end_pos));
}

void virutal_reg_s_live_interval::cut_off(virtual_target_code_pos pos)
{
    if(is_extending)
    {
        live_interval.front().first=pos;
    }
    else
    {
        start(pos,pos+1);
    }
    is_extending=false;
}

void virutal_reg_s_live_interval::extend(virtual_target_code_pos pos)
{
    live_interval.front().first=pos;
}

void virutal_reg_s_live_interval::add_use_pos(struct arm_basic_block * bb,virtual_target_code_pos pos)
{
    map_set_insert(use_poses,bb,pos);
}

void virutal_reg_s_live_interval::add_def_pos(struct arm_basic_block * bb,virtual_target_code_pos pos)
{
    map_set_insert(def_poses,bb,pos);
}

bool virutal_reg_s_live_interval::is_def_pos(virtual_target_code_pos pos) const
{
    for(auto def_bb_and_poses:def_poses)
    {
        if(def_bb_and_poses.second.find(pos)!=def_bb_and_poses.second.end())
        {
            return true;
        }
    }
    return false;
}

bool virutal_reg_s_live_interval::is_use_pos(virtual_target_code_pos pos) const
{
    for(auto use_bb_and_poses:use_poses)
    {
        if(use_bb_and_poses.second.find(pos)!=use_bb_and_poses.second.end())
        {
            return true;
        }
    }
    return false;
}

size_t virutal_reg_s_live_interval::get_score()
{
    if(score==0)
    {
        for(auto i:use_poses)
        {
            // if(score<i.first->loop_count)
            // {
            //     score=i.first->loop_count;
            // }
            if(score<i.first->loop_count*100)
            {
                score=i.first->loop_count*100+i.second.size();
                if(def_poses.find(i.first)!=def_poses.end())
                {
                    score+=def_poses.at(i.first).size()*4;
                }
            }
        }
        for(auto i:def_poses)
        {
            // if(score<i.first->loop_count)
            // {
            //     score=i.first->loop_count;
            // }
            if(score<i.first->loop_count*100)
            {
                score=i.first->loop_count*100+i.second.size();
                if(use_poses.find(i.first)!=use_poses.end())
                {
                    score+=use_poses.at(i.first).size();
                }
            }
        }
    }
    return score;
}

//==========================================================================//



//===================================== struct live_intervals =====================================//

void live_intervals::clear()
{
    virtual_regs_s_live_intervals.clear();
}

void live_intervals::new_empty_virtual_code_segment(virtual_target_code_pos pos,size_t add_size)
{
    for(auto reg_s_live_interval:virtual_regs_s_live_intervals)
    {
        for(auto & live_interval_segment:reg_s_live_interval.second.live_interval)
        {
            if(live_interval_segment.first>=pos)
            {
                live_interval_segment.first+=add_size;
            }
            if(live_interval_segment.second>pos)
            {
                live_interval_segment.second+=add_size;
            }
        }
        // for(auto & use_bb_and_pos:reg_s_live_interval.second.use_poses)
        // {
        //     for(auto & use_pos:use_bb_and_pos.second)
        //     {
        //         if(use_pos>=pos)
        //         {
        //             use_pos+=pos;
        //         }
        //     }
        // }
        // for(auto & def_bb_and_pos:reg_s_live_interval.second.def_poses)
        // {
        //     for(auto & def_pos:def_bb_and_pos.second)
        //     {
        //         if(def_pos>=pos)
        //         {
        //             def_pos+=pos;
        //         }
        //     }
        // }
    }
}

//==========================================================================//



//===================================== struct coherent_diagram_node =====================================//

coherent_diagram_node::coherent_diagram_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type,struct virutal_reg_s_live_interval live_interval):reg(reg),degree(0),live_interval(live_interval),data_type(data_type)
{

}

void coherent_diagram_node::add_a_move_related_neighbour(struct coherent_diagram_node * node)
{
    
}

void coherent_diagram_node::add_a_collision_neighbour(struct coherent_diagram_node * node)
{
    if(collision_nodes.find(node)==collision_nodes.end())
    {
        collision_nodes.insert(node);
        degree++;
    }
}

size_t coherent_diagram_node::get_score()
{
    return collision_nodes.size();
}

//==========================================================================//



//===================================== struct coherent_diagram =====================================//

coherent_diagram::~coherent_diagram()
{
    clear();
}

void coherent_diagram::clear()
{
    for(auto node:nodes)
    {
        delete node.second;
    }
    nodes.clear();
}

struct coherent_diagram_node * coherent_diagram::new_node(reg_index reg,enum coherent_diagram_node_s_data_type data_type)
{
    struct coherent_diagram_node * res=nullptr;
    if(nodes.find(reg)==nodes.end())
    {
        res=new coherent_diagram_node(reg,data_type,current_func_s_live_intervals.virtual_regs_s_live_intervals.at(reg));
        nodes.insert(make_pair(reg,res));
    }
    return res;
}

struct coherent_diagram_node * coherent_diagram::get_coherent_diagram_node(reg_index reg)
{
    return nodes.at(reg);
}

void coherent_diagram::add_move_related(reg_index reg_1,reg_index reg_2)
{
    
}

void coherent_diagram::add_collision(reg_index reg_1,reg_index reg_2)
{
    struct coherent_diagram_node * node_1,* node_2;
    node_1=get_coherent_diagram_node(reg_1);
    node_2=get_coherent_diagram_node(reg_2);
    node_1->add_a_collision_neighbour(node_2);
    node_2->add_a_collision_neighbour(node_1);
}

//==========================================================================//