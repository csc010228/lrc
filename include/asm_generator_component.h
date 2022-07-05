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
    GET_CPU_REG_FOR_CONST,                  //为某一个常数获取一个寄存器
    GET_AN_EMPTY_CPU_REG_FOR_CONST,
    GET_VFP_REG_FOR_CONST,
    GET_CPU_REG_FOR_READING_VAR,                //为读某一个变量获取一个寄存器
    GET_VFP_REG_FOR_READING_VAR,
    GET_CPU_REG_FOR_WRITING_VAR,                //为写某一个变量获取一个寄存器
    GET_VFP_REG_FOR_WRITING_VAR,
    STORE_VAR_TO_MEM,                       //把一个存储在寄存器中的变量写入内存
    LOAD_VAR_TO_REG,                        //把一个存储在内存中的变量写入寄存器中
    GET_CURRENT_FUNC_STACK_SIZE,            //获取当前函数的栈空间大小（sp-fp）
    READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,   //获取保存上下文的时候需要保存的CPU寄存器
    READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS,
    READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS,   
    READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS,    //获取恢复上下文的时候需要恢复的寄存器
    READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS,
    GET_SP_REG,                             //获取sp寄存器的编号
    GET_FP_REG,
    GET_LR_REG,
    GET_PC_REG,
    GET_R0_REG,
    GET_R1_REG,
    GET_S0_REG,
    GET_APSR_REG,
    GET_FPSCR_REG,
    IS_CPU_REG,
    IS_VFP_REG,
    IS_ARGUMENT_REG,
    IS_TEMP_REG,
    IS_F_PARAM_PASSED_BY_STACK,
    CHANGE_FLAGS_FOR_VAR,
    GET_ALL_ARGUMENT_REGS,
    READY_TO_PUSH_F_PARAM_CPU_REGS,
    READY_TO_PUSH_F_PARAM_VFP_REGS,
    READY_TO_POP_WHEN_RET,
    READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK,               //准备把函数的局部变量压入栈中
    READY_TO_PUSH_TEMP_VARS,
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
    GET_VAR_STACK_POS_FROM_FP,
    RETURN_VAR,
    WRITE_ADDR_TO_REG,
    FUNC_DEFINE,
    FUNC_RET,
    ASSIGN_VAR,
    CALL_FUNC,
    CALL_ABI_FUNC,
    GET_REG_BYTE_SIZE,
    WRITE_CONST_TO_REG,
    PUSH_VAR_TO_STACK,
    DISABLE_ALL_ADDR_REG,
    PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,
    PADDING_WHEN_CALL_FUNC,
    CHECK_TEMP_VAR_IN_STACK,
    CHECK_CONST_INT_VALUE_OWN_CPU_REG,
    GET_CONST_VALUE_S_CPU_REG,
    SAVE_REGS_WHEN_CALLING_FUNC,
    SAVE_REGS_WHEN_CALLING_ABI_FUNC,
    PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC,
    PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC,
    MOVE_DATA_BETWEEN_REGS,
    BEFORE_CALL_FUNC,
    RET_FROM_CALLED_FUNC,
    RET_FROM_CALLED_ABI_FUNC,
    CHECK_VAR_ATTACHED_TO_FLAG,
    GET_VAR_CARED_FLAG,
    POP_STACK,
    ALLOCATE_IDLE_CPU_REG,
    ATTACH_CONST_TO_REG,
    ATTACH_VAR_VALUE_TO_REG,
    ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY,
    UNATTACH_REG_S_ALL_DATA,
    OPTIMIZE,
    IS_VAR_STILL_ALIVE,
    IS_FUNC_NEED_PASS_PARAMS_BY_STACK,
    GET_CPU_ARGUMENT_REG_NUM,
    GET_VFP_ARGUMENT_REG_NUM,
    GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC,
    GET_TEMP_VARS_IN_CURRENT_BASIC_BLOCK,
    IS_TEMP_VAR_OVER_BASIC_BLOCKS_IN_CURRENT_FUNC,
    CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC,
    GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC
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