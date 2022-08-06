/*
 *
 *  util.cpp
 *  通用函数
 *  Creator:Sichao Chen
 *  Create time:2022/6/9
 *
*/

#include<map>

using namespace std;

map<int,int> two_s_power_exponents=
{
    {1,0},
    {2,1},
    {4,2},
    {8,3},
    {16,4},
    {32,5},
    {64,6},
    {128,7},
    {256,8},
    {512,9},
    {1024,10},
    {2048,11},
    {4096,12},
    {8192,13},
    {16384,14},
    {32768,15},
    {65536,16},
    {131072,17},
    {262144,18},
    {524288,19},
    {1048576,20},
    {2097152,21},
    {4194304,22},
    {8388608,23},
    {16777216,24},
    {33554432,25},
    {67108864,26},
    {134217728,27},
    {268435456,28},
    {536870912,29},
    {1073741824,30},
    {2147483648,31},
    //{4294967296,32},
};

bool is_two_s_power(int num)
{
    return two_s_power_exponents.find(num)!=two_s_power_exponents.end();
}

int get_two_s_power_exponent(int num)
{
    return two_s_power_exponents.at(num);
}