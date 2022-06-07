# ic_optimizer.h

*   该文件用于定义一系列中间代码优化的结构体和方法。在优化过程中主要关注这个文件
*   在优化的过程中可能需要对这个文件中的某些结构体进行增加成员。如果这么做了，务必将其写入文档，并周知队友

## class Ic_optimizer

*   中间代码优化器类
*   intermediate_codes_flow_graph_是要优化的中间代码的流图格式

## struct ic_flow_graph

*   该结构体表示一个中间代码流图

*   定义如下：

    ```c
    //中间代码的流图表示
    struct ic_flow_graph
    {
        ic_flow_graph(list<struct quaternion> * intermediate_codes)
        {
            struct ic_func_flow_graph * current_ic_func_flow_graph=nullptr;
            for(auto i:*intermediate_codes)
            {
                if(i.op==ic_op::VAR_DEFINE && ((struct ic_data *)i.result.second)->is_global())
                {
                    global_defines.push_back(i);
                }
                else
                {
                    if(i.op==ic_op::FUNC_DEFINE)
                    {
                        current_ic_func_flow_graph=new struct ic_func_flow_graph((struct ic_func *)i.result.second);
                        func_flow_graphs.push_back(current_ic_func_flow_graph);
                    }
                    current_ic_func_flow_graph->add_ic(i);
                }
            }
        };
    
        ~ic_flow_graph()
        {
            for(auto i:func_flow_graphs)
            {
                delete i;
            }
        };
    
        //所有函数的流图，这些流图之间相互独立
        list<struct ic_func_flow_graph * > func_flow_graphs;
        //全局定义域流图（里面的语句只能是变量定义语句）
        //struct ic_basic_block * global_basic_block;
        list<struct quaternion> global_defines;
    };
    ```

    *   func_flow_graphs是程序中的所有函数的流图，每一个函数的流图之间是独立的
    *   global_defines是全局定义语句

## struct ic_func_flow_graph

*   该结构体表示一个函数的流图

*   定义如下：

    ```c
    //一个中间代码的函数的流图
    struct ic_func_flow_graph
    {
        ic_func_flow_graph(struct ic_func * func)
        {
            this->func=func;
        };
    
        ~ic_func_flow_graph()
        {
            for(auto i:basic_blocks)
            {
                delete i;
            }
        };
    
        //往当前的函数流图中加入一条中间代码
        void add_ic(struct quaternion ic)
        {
            static map<struct ic_label *,struct ic_basic_block *> ic_label_basic_block_map;
            static struct ic_basic_block * current_basic_block=nullptr;
            static bool previous_ic_is_jump=false;
            struct quaternion_with_info current_ic_with_def_use_info;
            size_t current_ic_with_def_use_info_pos;
            unsigned short new_basic_block_tag=0;
            bool func_end=false;
            if(func_end)
            {
                return;
            }
            
            //划分基本块
            if(previous_ic_is_jump)
            {
                //跳转语句的下一条语句是基本块的入口语句
                new_basic_block_tag=2;
                previous_ic_is_jump=false;
            }
            switch(ic.op)
            {
                case ic_op::JMP:
                case ic_op::IF_JMP:
                case ic_op::IF_NOT_JMP:
                    previous_ic_is_jump=true;
                    break;
                case ic_op::FUNC_DEFINE:
                    //第一条语句是基本块的入口语句
                    new_basic_block_tag=1;
                    break;
                case ic_op::LABEL_DEFINE:
                    //跳转语句跳转到的语句是基本块的入口语句
                    new_basic_block_tag=3;
                    break;
                case ic_op::END_FUNC_DEFINE:
                    func_end=true;
                    new_basic_block_tag=4;
                    break;
                default:
                    break;
            }
            switch(new_basic_block_tag)
            {
                case 0:
                    break;
                case 1:
                    current_basic_block=new struct ic_basic_block;
                    new_basic_block_tag=0;
                    break;
                case 2:
                    basic_blocks.push_back(current_basic_block);
                    current_basic_block=new struct ic_basic_block;
                    basic_blocks.back()->set_sequential_next(current_basic_block);
                    new_basic_block_tag=0;
                    break;
                case 3:
                    basic_blocks.push_back(current_basic_block);
                    current_basic_block=new struct ic_basic_block;
                    basic_blocks.back()->set_sequential_next(current_basic_block);
                    ic_label_basic_block_map.insert(make_pair((struct ic_label *)ic.result.second,current_basic_block));
                    new_basic_block_tag=0;
                    break;
                case 4:
                    basic_blocks.push_back(current_basic_block);
                    new_basic_block_tag=0;
                    break;
                default:
                    new_basic_block_tag=0;
                    break;
            }
            
            //将中间代码加入当前基本块中
            current_basic_block->add_ic(ic);
            
            
            //如果该函数的流图构造完毕，那么就对各个基本块之间的跳转情况进行设置
            if(func_end)
            {
                for(auto i:basic_blocks)
                {
                    if(i->ic_sequence.back().intermediate_code.op==ic_op::JMP || i->ic_sequence.back().intermediate_code.op==ic_op::IF_JMP || i->ic_sequence.back().intermediate_code.op==ic_op::IF_NOT_JMP)
                    {
                        i->set_jump_next(ic_label_basic_block_map.at((struct ic_label *)(i->ic_sequence.back().intermediate_code.result.second)));
                    }
                    else
                    {
                        i->set_jump_next(nullptr);
                    }
                }
            }
        };
    
        //对应的函数在符号表中的指针
        struct ic_func * func;
        //函数流图中的所有基本块序列，顺序就是中间代码的书写顺序
        list<struct ic_basic_block * > basic_blocks;
    };
    ```

    *   func成员表示该函数流图对应的函数
    *   basic_blocks成员将函数流图中的基本块按顺序排列
    *   可以在这个结构体中加入更多的成员来存储一些数据流分析的信息

## struct ic_basic_block

*   该结构体表示一个基本块

*   定义如下：

    ```c
    //中间代码的基本块
    struct ic_basic_block
    {
        ic_basic_block()
        {
            sequential_next=nullptr;
            jump_next=nullptr;
        };
    
        //设置该基本块顺序执行的时候的下一个基本块
        void set_sequential_next(struct ic_basic_block * next)
        {
            sequential_next=next;
        };
    
        //设置该基本块跳转执行的时候的下一个基本块
        void set_jump_next(struct ic_basic_block * next)
        {
            jump_next=next;
        };
    
        //往当前基本块中加入一条中间代码
        void add_ic(struct quaternion ic)
        {
            struct quaternion_with_info ic_with_def_use_info(ic);
            //size_t current_pos;
            ic_sequence.push_back(ic_with_def_use_info);
            //current_pos=get_last_ic_pos();
            //记录该中间代码的明确定义，模糊定义和使用
            
        };
    
        //基本块中的中间代码序列
        vector<struct quaternion_with_info> ic_sequence;
        //该基本块的后续基本块
        struct ic_basic_block * sequential_next,* jump_next;
    };
    ```

    *   ic_sequence成员按序存储了该基本块中的中间代码序列
    *   sequential_next指向了该基本块的下一个基本块
    *   jump_next指向了该基本块经过跳转之后的可能会跳转到的基本块，可能为nullptr
    *   可以增加该结构体的成员来存储一些基本块的信息

## struct quaternion_with_info

*   该结构体定义了中间代码，及其信息

*   定义如下：

    ```c
    //带有变量定义和使用信息的中间代码
    struct quaternion_with_info
    {
        quaternion_with_info()
        {
    
        };
    
        quaternion_with_info(struct quaternion ic):intermediate_code(ic)
        {
            
        };
    
        //中间代码
        struct quaternion intermediate_code;
    };
    ```

    *   目前只有intermediate_code一个成员，表示本条中间代码
    *   可以增加该结构体的成员，用于表示本条中间代码的一些信息，比如本条中间代码使用到了哪些变量，定义了哪些变量等等

