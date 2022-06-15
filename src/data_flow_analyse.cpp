/*
 *
 *  data_flow_analyse.cpp
 *  数据流分析
 *  Creator:Sichao Chen
 *  Create time:2022/6/9
 *
*/
#include"data_flow_analyse.h"

/*
数据流分析的准备

Parameters
----------
func:要分析的函数流图
*/
void prepare_before_data_flow_analyse(struct ic_func_flow_graph * func)
{
    //获取要分析的函数流图的数组元素的所属数组和偏移量信息
    func->build_array_and_offset_to_array_member_map();
    //获取要分析的函数流图的变量定义点和使用点信息
    func->build_vars_def_and_use_pos_info();
}

/*
到达-定义分析

Parameters
----------
func:要分析的函数流图
*/
void use_define_analysis(struct ic_func_flow_graph * func)
{
    map<struct ic_basic_block *,map<struct ic_data *,set<ic_pos> > > gens,kills;
    set<struct ic_basic_block * > precursors;
    map<struct ic_data *,set<ic_pos> > oldout;
    struct ic_data * belong_array,* offset;
    size_t pos;
    ic_pos current_pos;
    bool change=true;
    //构造每一个基本块的gen和kill
    for(auto basic_block:func->basic_blocks)
    {
        gens.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        kills.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        pos=0;
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            current_pos=ic_pos(basic_block,pos);
            if(ic_with_info.explicit_def)
            {
                //一个变量的明确定义会生成（gen）该变量的一次定义，同时会杀死（kill）该变量的其他所有的定义
                if(gens.at(basic_block).find(ic_with_info.explicit_def)==gens.at(basic_block).end())
                {
                    gens.at(basic_block).insert(make_pair(ic_with_info.explicit_def,set<ic_pos>()));
                }
                else
                {
                    gens.at(basic_block).at(ic_with_info.explicit_def).clear();
                }
                gens.at(basic_block).at(ic_with_info.explicit_def).insert(current_pos);
                if(func->vars_def_positions.find(ic_with_info.explicit_def)!=func->vars_def_positions.end())
                {
                    if(kills.at(basic_block).find(ic_with_info.explicit_def)==kills.at(basic_block).end())
                    {
                        kills.at(basic_block).insert(make_pair(ic_with_info.explicit_def,set<ic_pos>()));
                    }
                    set_union(func->vars_def_positions.at(ic_with_info.explicit_def).begin(),func->vars_def_positions.at(ic_with_info.explicit_def).end(),kills.at(basic_block).at(ic_with_info.explicit_def).begin(),kills.at(basic_block).at(ic_with_info.explicit_def).end(),inserter(kills.at(basic_block).at(ic_with_info.explicit_def),kills.at(basic_block).at(ic_with_info.explicit_def).begin()));
                    kills.at(basic_block).at(ic_with_info.explicit_def).erase(current_pos);
                }
                //一个变量的明确定义会杀死（kill）所有以该变量为偏移量的数组取元素变量的定义
                if(func->offset_to_array_member_map.find(ic_with_info.explicit_def)!=func->offset_to_array_member_map.end())
                {
                    for(auto array_member:func->offset_to_array_member_map.at(ic_with_info.explicit_def))
                    {
                        if(func->vars_def_positions.find(array_member)!=func->vars_def_positions.end())
                        {
                            if(kills.at(basic_block).find(array_member)==kills.at(basic_block).end())
                            {
                                kills.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                            }
                            set_union(func->vars_def_positions.at(array_member).begin(),func->vars_def_positions.at(array_member).end(),kills.at(basic_block).at(array_member).begin(),kills.at(basic_block).at(array_member).end(),inserter(kills.at(basic_block).at(array_member),kills.at(basic_block).at(array_member).begin()));
                        }
                        if(gens.at(basic_block).find(array_member)!=gens.at(basic_block).end())
                        {
                            gens.at(basic_block).erase(array_member);
                        }
                    }
                }
                //当明确定义的变量是数组或者数组元素的时候，需要进行额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if(ic_with_info.explicit_def->is_array_var())
                {
                    if(func->array_to_array_member_map.find(ic_with_info.explicit_def)!=func->array_to_array_member_map.end())
                    {
                        for(auto array_member:func->array_to_array_member_map.at(ic_with_info.explicit_def))
                        {
                            //明确定义的数组会杀死（kill）所有的该数组的数组取元素的定义
                            if(func->vars_def_positions.find(array_member)!=func->vars_def_positions.end())
                            {
                                if(kills.at(basic_block).find(array_member)==kills.at(basic_block).end())
                                {
                                    kills.at(basic_block).insert(make_pair(array_member,set<ic_pos>()));
                                }
                                set_union(func->vars_def_positions.at(array_member).begin(),func->vars_def_positions.at(array_member).end(),kills.at(basic_block).at(array_member).begin(),kills.at(basic_block).at(array_member).end(),inserter(kills.at(basic_block).at(array_member),kills.at(basic_block).at(array_member).begin()));
                            }
                            if(gens.at(basic_block).find(array_member)!=gens.at(basic_block).end())
                            {
                                gens.at(basic_block).erase(array_member);
                            }
                            //同时也会在该点生成（gen）所有的该数组的数组取元素的定义
                            map_set_insert(gens.at(basic_block),array_member,current_pos);
                        }
                    }
                }
                else if(ic_with_info.explicit_def->is_array_member())
                {
                    belong_array=ic_with_info.explicit_def->get_belong_array();
                    offset=ic_with_info.explicit_def->get_offset();
                    if(func->array_to_array_member_map.find(belong_array)!=func->array_to_array_member_map.end())
                    {
                        //如果明确定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的属于该数组的偏移量不是常量的数组取元素变量，以及所有的属于该数组的偏移量是常数而且和该数组取元素偏移量相等的数组取元素变量的定义
                        //如果明确定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:func->array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==ic_with_info.explicit_def || (offset->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=offset->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(gens.at(basic_block),array_member,current_pos);
                        }
                    }
                }
            }
            for(auto vague_def:ic_with_info.vague_defs)
            {
                if(gens.at(basic_block).find(vague_def)==gens.at(basic_block).end())
                {
                    gens.at(basic_block).insert(make_pair(vague_def,set<ic_pos>()));
                }
                gens.at(basic_block).at(vague_def).insert(current_pos);
                //当模糊定义的变量是数组或者数组取元素的时候，需要进行额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if(vague_def->is_array_var())
                {
                    //模糊定义的数组会生成（gen）所有的该数组的数组取元素的定义
                    if(func->array_to_array_member_map.find(vague_def)!=func->array_to_array_member_map.end())
                    {
                        for(auto array_member:func->array_to_array_member_map.at(vague_def))
                        {
                            map_set_insert(gens.at(basic_block),array_member,current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    belong_array=vague_def->get_belong_array();
                    offset=vague_def->get_offset();
                    if(func->array_to_array_member_map.find(belong_array)!=func->array_to_array_member_map.end())
                    {
                        //如果模糊定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义，以及所有的属于该数组的偏移量是常数而且和该数组取元素偏移量相等的数组取元素变量的定义
                        //如果模糊定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:func->array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (offset->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=offset->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(gens.at(basic_block),array_member,current_pos);
                        }
                    }
                }
            }
            pos++;
        }
    }
    //最后开始迭代计算每一个基本块的到达-定义信息
    for(auto basic_block:func->basic_blocks)
    {
        basic_block->use_def_analysis_info.in.clear();
        basic_block->use_def_analysis_info.out=gens.at(basic_block);
    }
    while(change)
    {
        change=false;
        for(auto basic_block:func->basic_blocks)
        {
            precursors=basic_block->get_precursors();
            basic_block->use_def_analysis_info.in.clear();
            for(auto precursor:precursors)
            {
                basic_block->use_def_analysis_info.in=map_set_union(basic_block->use_def_analysis_info.in,precursor->use_def_analysis_info.out);
            }
            oldout=basic_block->use_def_analysis_info.out;
            basic_block->use_def_analysis_info.out=map_set_union(gens.at(basic_block),map_set_difference(basic_block->use_def_analysis_info.in,kills.at(basic_block)));
            if(oldout!=basic_block->use_def_analysis_info.out)
            {
                change=true;
            }
        }
    }
}

/*
构建ud-链

Parameters
----------
func:已经完成了到达-定义分析的，要构建ud-链的函数流图
*/
void build_ud_chain(struct ic_func_flow_graph * func)
{
    map<struct ic_data *,set<ic_pos> > defs;
    size_t pos;
    ic_pos current_pos;
    struct ic_data * belong_array;
    for(auto basic_block:func->basic_blocks)
    {
        pos=0;
        defs=basic_block->use_def_analysis_info.in;
        for(vector<struct quaternion_with_info>::iterator ic_with_info=basic_block->ic_sequence.begin();ic_with_info!=basic_block->ic_sequence.end();ic_with_info++)
        {
            current_pos=ic_pos(basic_block,pos);
            for(auto use:(*ic_with_info).uses)
            {
                // if(use->is_array_var())
                // {
                //     continue;
                // }
                if(defs.find(use)!=defs.end())
                {
                    (*ic_with_info).add_to_ud_chain(use,defs.at(use));
                }
            }
            if((*ic_with_info).explicit_def)
            {
                //如果本条语句对某一个变量进行了明确定义，那么它会杀死（kill）该基本块中之前的所有对该变量的明确定义和模糊定义，同时在该点生成（gen）对该变量的定义
                if(defs.find((*ic_with_info).explicit_def)==defs.end())
                {
                    defs.insert(make_pair((*ic_with_info).explicit_def,set<ic_pos>()));
                }
                else
                {
                    defs.at((*ic_with_info).explicit_def).clear();
                }
                defs.at((*ic_with_info).explicit_def).insert(current_pos);
                //明确定义也会杀死（kill）所有以该变量作为偏移的数组取元素的定义
                if(func->offset_to_array_member_map.find((*ic_with_info).explicit_def)!=func->offset_to_array_member_map.end())
                {
                    for(auto array_member:func->offset_to_array_member_map.at((*ic_with_info).explicit_def))
                    {
                        if(defs.find(array_member)!=defs.end())
                        {
                            defs.erase(array_member);
                        }
                    }
                }
                //如果明确定义是数组或者数组取元素，那么需要做额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if((*ic_with_info).explicit_def->is_array_var())
                {
                    //如果明确定义是数组，那么就会杀死之前所有的该数组取元素的定义，然后再在该点生成（gen）所有的该数组取元素的定义
                    if(func->array_to_array_member_map.find((*ic_with_info).explicit_def)!=func->array_to_array_member_map.end())
                    {
                        for(auto array_member:func->array_to_array_member_map.at((*ic_with_info).explicit_def))
                        {
                            if(defs.find(array_member)==defs.end())
                            {
                                defs.insert(make_pair(array_member,set<ic_pos>()));
                            }
                            else
                            {
                                defs.at(array_member).clear();
                            }
                            defs.at(array_member).insert(current_pos);
                        }
                    }
                }
                else if((*ic_with_info).explicit_def->is_array_member())
                {
                    //如果明确定义是数组取元素，那么就判断其偏移量是否是常量
                    belong_array=(*ic_with_info).explicit_def->get_belong_array();
                    if(func->array_to_array_member_map.find(belong_array)!=func->array_to_array_member_map.end())
                    {
                        //如果明确定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义，以及所有的属于该数组的偏移量是常数而且和该数组取元素偏移量相等的数组取元素变量的定义
                        //如果明确定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:func->array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==(*ic_with_info).explicit_def || ((*ic_with_info).explicit_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=(*ic_with_info).explicit_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(defs,array_member,current_pos);
                        }
                    }
                }
            }
            for(auto vague_def:(*ic_with_info).vague_defs)
            {
                //如果本条语句对某一个变量进行了模糊定义，那么它会在该点生成（gen）对该变量的定义
                if(defs.find(vague_def)==defs.end())
                {
                    defs.insert(make_pair(vague_def,set<ic_pos>()));
                }
                defs.at(vague_def).insert(current_pos);
                //如果模糊定义是数组或者数组取元素，那么需要做额外的处理
                //这里不会出现即是数组，又是数组取元素的情况
                if(vague_def->is_array_var())
                {
                    //如果模糊定义是数组，那么就会在该点生成（gen）所有的该数组取元素的定义
                    if(func->array_to_array_member_map.find(vague_def)!=func->array_to_array_member_map.end())
                    {
                        for(auto array_member:func->array_to_array_member_map.at(vague_def))
                        {
                            map_set_insert(defs,array_member,current_pos);
                        }
                    }
                }
                else if(vague_def->is_array_member())
                {
                    //如果模糊定义的是数组取元素，那么就判断其偏移量是否是常量
                    belong_array=vague_def->get_belong_array();
                    if(func->array_to_array_member_map.find(belong_array)!=func->array_to_array_member_map.end())
                    {
                        //如果模糊定义数组取元素变量的偏移量是常量的话，那么该定义会生成（gen）所有的该数组的偏移量不是常量的数组取元素变量的定义，以及所有的属于该数组的偏移量是常数而且和该数组取元素偏移量相等的数组取元素变量的定义
                        //如果模糊定义数组取元素变量的偏移量不是常量的话，那么该定义会生成（gen）所有的该数组的数组取元素变量的定义
                        for(auto array_member:func->array_to_array_member_map.at(belong_array))
                        {
                            if(array_member==vague_def || (vague_def->get_offset()->is_const() && array_member->get_offset()->is_const() && array_member->get_offset()->get_value()!=vague_def->get_offset()->get_value()))
                            {
                                continue;
                            }
                            map_set_insert(defs,array_member,current_pos);
                        }
                    }
                }
            }
            pos++;
        }
    }
}

/*
活跃变量分析

Parameters
----------
func:要分析的函数流图
*/
void live_variable_analysis(struct ic_func_flow_graph * func)
{
    map<struct ic_basic_block *,map<struct ic_data *,set<ic_pos> > > uses,defs;
    set<struct ic_basic_block * > precursors;
    map<struct ic_data *,set<ic_pos> > oldout;
    struct ic_data * belong_array,* offset;
    size_t pos;
    ic_pos current_pos;
    bool change=true;
    //先构造每一个基本块的uses和defs集合
    for(auto basic_block:func->basic_blocks)
    {
        uses.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        defs.insert(make_pair(basic_block,map<struct ic_data *,set<ic_pos> >()));
        pos=0;
        for(auto ic_with_info:basic_block->ic_sequence)
        {
            current_pos=ic_pos(basic_block,pos);
            
        }
    }
}

/*
构建du-链

Parameters
----------
func:已经完成了活跃变量分析的，要构建du-链的函数流图
*/
void build_du_chain(struct ic_func_flow_graph * func)
{

}

/*
可用表达式分析

Parameters
----------
func:要分析的函数流图
*/
void available_expression_analysis(struct ic_func_flow_graph * func)
{

}