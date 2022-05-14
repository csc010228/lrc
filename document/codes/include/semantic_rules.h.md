# senamtic_rulws.h

## struct quaternion

*   该文件中定义的重要的结构体是四元式中间代码`struct quaternion`：

    ```c
    //中间代码的四元式表示
    struct quaternion
    {
        quaternion():op(ic_op::NOP),arg1(make_pair(ic_operand::NONE,nullptr)),arg2(make_pair(ic_operand::NONE,nullptr)),result(make_pair(ic_operand::NONE,nullptr))
        {
    
        };
    
        quaternion(enum ic_op op,enum ic_operand arg1_type,void * arg1_data,enum ic_operand arg2_type,void * arg2_data,enum ic_operand result_type,void * result_data):op(op),arg1(make_pair(arg1_type,arg1_data)),arg2(make_pair(arg2_type,arg2_data)),result(make_pair(result_type,result_data))
        {
            
        };
    
        enum ic_op op;                     //操作符
        pair<enum ic_operand,void *> arg1;                   //操作数1
        pair<enum ic_operand,void *> arg2;                   //操作数2
        pair<enum ic_operand,void *> result;                 //结果
    };
    ```

    *   op是操作符，也定义在该文件中：

        ```c
        //中间代码的操作符
        enum class ic_op
        {
            NOP,                        //空
            ASSIGN,                     //赋值
            ADD,                        //加
            SUB,                        //减
            MUL,                        //乘
            DIV,                        //除
            MOD,                        //取模
            //L_SHIFT,                    //左移位
            //R_SHIFT,                    //右移位
            NOT,                        //非
            //AND,                        //与
            //OR,                         //或
            //XOR,                        //异或
            EQ,                         //是否相等
            UEQ,                        //是否不相等
            GT,                         //比较是否大于
            LT,                         //比较是否小于
            GE,                         //比较是否大于等于
            LE,                         //比较是否小于等于
            JMP,                        //无条件跳转（没有操作数，只有结果，结果就是要跳转到的中间代码）
            IF_JMP,                     //有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码，当条件表达式的结果不是0的时候会进行跳转）
            IF_NOT_JMP,                 //有条件跳转（只有一个操作数，就是条件表达式，结果是要跳转到的中间代码，当条件表达式的结果是0的时候会进行跳转）
            VAR_DEFINE,                 //变量定义（可以赋初始值，也可以不赋初始值）
            LABEL_DEFINE,               //定义一个LABEL
            FUNC_DEFINE,                //函数定义（第一个操作数是形参列表，结果是要定义的函数在符号表中的表项指针）
            END_FUNC_DEFINE,            //结束函数定义（只有结果操作数，是结束定义的函数在符号表中的表项指针）
            CALL,                       //函数调用（第一个操作数是要调用的函数，第二个操作数是实参列表，结果是返回值）
            RET,                        //函数返回（只有结果，没有操作数，结果就是返回值）
        };
        ```

    *   arg1，arg2和result都是一个`pair<enum ic_operand,void *>`，其中，first表明了该操作数是什么类型的，second是该操作数的值。操作数类型也定义在该文件中：

        ```c
        //中间代码的操作数类型
        enum class ic_operand
        {
            NONE,                               //空
            DATA,                               //数据
            DATAS,                              //多个数据（目前只用在函数调用CALL上）
            LABEL,                              //标签
            FUNC,                               //函数
        };
        ```

        *   当操作数类型是NONE的时候，表示该操作数是空的，此时的操作数的值无意义
        *   当操作数类型是DATA的时候，表示操作数是一个数据，此时的操作数的值void *应该是一个`struct ic_data *`（定义在`symbol_table.h`中）
        *   当操作数类型是DATAS的时候，表示操作数是若干个数据，此时的操作数的值void *应该是一个`list<struct ic_data *> *`
        *   当操作数类型是LABEL的时候，表示操作数是一个标签，此时的操作数的值void *应该是一个`struct ic_label *`（定义在`symbol_table.h`中）
        *   当操作数类型是FUNC的时候，表示操作数是一个函数，此时的操作数的值void *应该是一个`struct ic_func *`（定义在`symbol_table.h`中）