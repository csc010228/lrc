/*
 *
 *  util.h
 *  通用函数
 *  Creator:Sichao Chen
 *  Create time:2022/6/9
 *
*/
#ifndef __UTIL_H
#define __UTIL_H

template<typename MAP_SET_KEY,typename MAP_SET_VALUE>
void map_set_insert(map<MAP_SET_KEY,set<MAP_SET_VALUE> > & target,const MAP_SET_KEY & key,const MAP_SET_VALUE & value)
{
    if(target.find(key)==target.end())
    {
        target.insert(make_pair(key,set<MAP_SET_VALUE>()));
    }
    target.at(key).insert(value);
}

template<typename MAP_SET_KEY,typename MAP_SET_VALUE>
void map_set_erase(map<MAP_SET_KEY,set<MAP_SET_VALUE> > & target,const MAP_SET_KEY & key,const MAP_SET_VALUE & value)
{
    if(target.find(key)!=target.end())
    {
        if(target.at(key).find(value)!=target.at(key).end())
        {
            target.at(key).erase(value);
        }
        if(target.at(key).empty())
        {
            target.erase(key);
        }
    }
}

template<typename MAP_SET_KEY,typename MAP_SET_VALUE>
map<MAP_SET_KEY,set<MAP_SET_VALUE> > map_set_union(const map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg1,const map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg2)
{
    map<MAP_SET_KEY,set<MAP_SET_VALUE> > res=arg1;
    for(auto map_member:arg2)
    {
        if(res.find(map_member.first)==res.end())
        {
            res.insert(make_pair(map_member.first,set<MAP_SET_VALUE>()));
        }
        set_union(map_member.second.begin(),map_member.second.end(),res.at(map_member.first).begin(),res.at(map_member.first).end(),inserter(res.at(map_member.first),res.at(map_member.first).begin()));
    }
    return res;
}

template<typename MAP_SET_KEY,typename MAP_SET_VALUE>
map<MAP_SET_KEY,set<MAP_SET_VALUE> > map_set_difference(const map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg1,const map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg2)
{
    map<MAP_SET_KEY,set<MAP_SET_VALUE> > res=arg1;
    for(auto map_member:arg2)
    {
        if(res.find(map_member.first)!=res.end())
        {
            set_difference(map_member.second.begin(),map_member.second.end(),res.at(map_member.first).begin(),res.at(map_member.first).end(),inserter(res.at(map_member.first),res.at(map_member.first).begin()));
        }
    }
    return res;
}

template<typename MAP_SET_KEY,typename MAP_SET_VALUE>
void map_set_union_and_assign_to_arg1(map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg1,const map<MAP_SET_KEY,set<MAP_SET_VALUE> > & arg2)
{
    for(auto map_member:arg2)
    {
        if(arg1.find(map_member.first)==arg1.end())
        {
            arg1.insert(make_pair(map_member.first,set<MAP_SET_VALUE>()));
        }
        set_union(map_member.second.begin(),map_member.second.end(),arg1.at(map_member.first).begin(),arg1.at(map_member.first).end(),inserter(arg1.at(map_member.first),arg1.at(map_member.first).begin()));
    }
}

#endif //__UTIL_H