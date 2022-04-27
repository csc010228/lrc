/*
 *
 *  asm_generator_component.h
 *  汇编代码生成器组件
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#ifndef __ASM_GENERATOR_COMPONENT_H
#define __ASM_GENERATOR_COMPONENT_H



//寄存器编号
typedef unsigned int reg_index;

//汇编代码生成器组件之间互相传递的事件类型
enum class event_type
{
    NONE,                                   //空
    NEXT_IC,                                //获取下一条中间代码
    RESPONSE_BOOL,                          //返回bool类型数据
    RESPONSE_INT,                           //返回int类型数据
    RESPONSE_POINTER,                       //返回指针数据
    GET_REG_FOR_INT_CONST,                  //为某一个常数获取一个寄存器
    GET_REG_FOR_READING_VAR,                //为读某一个变量获取一个寄存器
    GET_REG_FOR_WRITING_VAR,                //为写某一个变量获取一个寄存器
    STORE_VAR_TO_MEM,                       //把一个存储在寄存器中的变量写入内存
    LOAD_VAR_TO_REG,                        //把一个存储在内存中的变量写入寄存器中
    READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,   //获取保存上下文的时候需要保存的CPU寄存器
    READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS,   
    READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS,    //获取恢复上下文的时候需要恢复的寄存器
    READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS,
    GET_SP_REG,                             //获取sp寄存器的编号
    GET_LR_REG,
    GET_PC_REG,
    GET_R0_REG,
    GET_R1_REG,
    GET_S0_REG,
    IS_CPU_REG,
    IS_VFP_REG,
    IS_ARGUMENT_REG,
    IS_TEMP_REG,
    CHANGE_FLAGS_FOR_VAR,
    READY_TO_PUSH_F_PARAM_CPU_REGS,
    READY_TO_PUSH_F_PARAM_VFP_REGS,
    READY_TO_POP_F_PARAM_CPU_REGS,
    READY_TO_POP_F_PARAM_VFP_REGS,
    READY_TO_PUSH_LOCAL_VARS,               //准备把函数的局部变量压入栈中
    READY_TO_POP_LOCAL_VARS,
    READY_TO_POP_TEMP_VARS,
    START_BASIC_BLOCK,
    END_BASIC_BLOCK,
    END_BASIC_BLOCK_WITHOUT_FLAG,
    CLEAR_FLAG,
    START_FUNC,
    END_FUNC,
    START_INSTRUCTION,
    END_INSTRUCTION,
    GET_ADDR_REG,
    GET_VAR_STACK_POS_FROM_SP,
    RETURN_VAR,
    WRITE_ADDR_TO_REG,
    FUNC_DEFINE,
    FUNC_RET,
    ASSIGN_VAR,
    CALL_FUNC,
    CALL_ABI_FUNC,
    GET_REG_BYTE_SIZE,
    WRITE_CONST_INT_TO_REG,
    WRITE_CONST_FLOAT_TO_REG,
    PUSH_VAR_TO_STACK,
    PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,
    PUSH_TEMP_VAR_FROM_REG_TO_STACK,
    CHECK_TEMP_VAR_IN_STACK,
    CHECK_CONST_INT_OWN_VALUE_REG,
    CHECK_VAR_OWN_VALUE_REG,
    GET_VAR_S_VALUE_REG,
    GET_CONST_INT_S_VALUE_REG,
    SAVE_REGS_WHEN_CALLING_FUNC,
    SAVE_REGS_WHEN_CALLING_ABI_FUNC,
    PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC,
    MOVE_DATA_BETWEEN_CPU_REGS,
    PUSH_ARGUMENT_FROM_REG_TO_STACK_WHEN_CALLING_FUNC,
    RET_FROM_CALLED_FUNC,
    RET_FROM_CALLED_ABI_FUNC,
    CHECK_VAR_ATTACHED_TO_FLAG,
    GET_VAR_CARED_FLAG,
    POP_STACK,
    ALLOCATE_IDLE_CPU_REG,
    ATTACH_CONST_INT_TO_REG,
    OPTIMIZE
};

//汇编代码生成器组件之间互相传递的事件
struct event
{
    event():type(event_type::NONE),pointer_data(nullptr)
    {

    };

    event(enum event_type type,bool bool_data):type(type),bool_data(bool_data)
    {

    };

    event(enum event_type type,int int_data):type(type),int_data(int_data)
    {

    };

    event(enum event_type type,void * pointer_data):type(type),pointer_data(pointer_data)
    {

    };

    enum event_type type;                //事件类型
    //汇编代码生成器组件之间互相传递的事件数据
    union
    {
        bool bool_data;
        int int_data;
        void * pointer_data;
    };
};

class Asm_generator;

//汇编代码生成器组件
class Asm_generator_component
{

protected:
    //中介者
    Asm_generator *mediator_;

    //通知中介者有事件发生
    struct event notify(struct event event);

public:
    //构造函数
    Asm_generator_component(Asm_generator *mediator = nullptr) : mediator_(mediator) {
    }

    //设置中介者
    void set_mediator(Asm_generator *mediator);

    //事件处理函数(由中介者进行调用)
    virtual struct event handler(struct event event)=0;
};

#endif //__ASM_GENERATOR_COMPONENT_H