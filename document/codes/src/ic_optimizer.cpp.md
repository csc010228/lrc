# ic_optimizer.cpp

*   该文件实现了中间代码优化器class Ic_optimizer
*   主要的函数有：
    *   local_optimizer：
        *   局部优化，将局部优化的代码写在里面
    *   data_flow_analysis：
        *   数据流优化，将数据流优化的代码写在里面
    *   global_optimize：
        *   全局优化，将全局优化的代码写在里面
*   所有的优化的对象都是class Ic_optimizer成员intermediate_codes_flow_graph_
*   在编写优化代码的时候，每一种优化方法都独立写一个函数，然后在上面的三个函数local_optimizer，data_flow_analysis或者global_optimize中将其调用，这样结构清晰