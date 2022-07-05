/*
 *
 *  arm_instruction_generator.cpp
 *  arm汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#include "arm_instruction_generator.h"

Arm_instruction_generator::Arm_instruction_generator(map<reg_index,string> regs_info):Instruction_generator(regs_info),is_current_basic_block_starting_(false)
{

}

Arm_instruction_generator::~Arm_instruction_generator()
{
    for(auto i : arm_asm_codes_)
    {
        delete i;
    }
}

/*
生成最终的汇编文件序列,并将其赋值给父类Instruction_generator的asm_codes_成员
*/
void Arm_instruction_generator::generate_asm_codes()
{
    //生成汇编代码流图
    generate_arm_flow_graph();
    //arm汇编代码优化
    notify(event(event_type::OPTIMIZE,(void *)(& arm_flow_graph_)));
    //转换成字符串
    asm_codes_=arm_flow_graph_.to_string();
}

/*
根据中间代码生成arm汇编文件的流图形式，并将其赋值给arm_flow_graph_成员
*/
void Arm_instruction_generator::generate_arm_flow_graph()
{
    pair<bool,struct quaternion> next=get_next_intermediate_code();
    while(next.first)
    {
        ic_to_arm_asm(next.second);
        next=get_next_intermediate_code();
    }
}

/*
根据一条中间代码生成最终的arm汇编代码

Parameters
----------
intermediate_code:要转换的一条中间代码
*/
// #include<iostream>
// using namespace std;
void Arm_instruction_generator::ic_to_arm_asm(struct quaternion intermediate_code)
{
    // cout<<((int)intermediate_code.op)<<endl;
    if(is_current_basic_block_starting_ && intermediate_code.op!=ic_op::FUNC_DEFINE && intermediate_code.op!=ic_op::LABEL_DEFINE)
    {
        start_basic_block_to_arm_asm();
        is_current_basic_block_starting_=false;
    }
    switch(intermediate_code.op)
    {
        case ic_op::NOP:
            break;
        case ic_op::ASSIGN:
            assign_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::ADD:
            add_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::SUB:
            sub_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::MUL:
            mul_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::DIV:
            div_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::MOD:
            mod_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        //case ic_op::L_SHIFT:
            //l_shift_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            //break;
        //case ic_op::R_SHIFT:
            //r_shift_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            //break;
        case ic_op::NOT:
            not_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.result.second);
            break;
        // case ic_op::AND:
        //     and_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
        //     break;
        // case ic_op::OR:
        //     or_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
        //     break;
        // case ic_op::XOR:
        //     xor_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
        //     break;
        case ic_op::EQ:
            eq_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::UEQ:
            ueq_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::GT:
            gt_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::LT:
            lt_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::GE:
            ge_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::LE:
            le_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::JMP:
            jmp_ic_to_arm_asm((struct ic_label *)intermediate_code.result.second);
            break;
        case ic_op::IF_JMP:
            if_jmp_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_label *)intermediate_code.result.second);
            break;
        case ic_op::IF_NOT_JMP:
            if_not_jmp_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_label *)intermediate_code.result.second);
            break;
        case ic_op::VAR_DEFINE:
            var_define_ic_to_arm_asm((struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::LABEL_DEFINE:
            label_define_ic_to_arm_asm((struct ic_label *)intermediate_code.result.second);
            break;
        case ic_op::FUNC_DEFINE:
            func_define_ic_to_arm_asm((struct ic_func *)intermediate_code.result.second);
            break;
        case ic_op::END_FUNC_DEFINE:
            end_func_define_ic_to_arm_asm();
            break;
        case ic_op::CALL:
            call_ic_to_arm_asm((struct ic_func *)intermediate_code.arg1.second,(list<struct ic_data * > *)intermediate_code.arg2.second,intermediate_code.result.first==ic_operand::NONE?nullptr:(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::RET:
            ret_ic_to_arm_asm(intermediate_code.result.first==ic_operand::NONE?nullptr:(struct ic_data *)intermediate_code.result.second);
            break;
        default:
            break;
    }
}

inline void ASR_N(int & value,unsigned int bits,unsigned int times)
{
    int mask=0x1 << (bits-1);
    
    while(times-->0)
    {
        value=(value >> 1) | (mask & value);
    }
}

inline void LSL_N(int & value,unsigned int bits,unsigned int times)
{
    while(times-->0)
    {
        value=(value << 1);
    }
}

inline void LSR_N(int & value,unsigned int bits,unsigned int times)
{
    int mask=!(0x1 << (bits-1));
    
    while(times-->0)
    {
        value=(value >> 1) & mask;
    }
}

inline void ROR_N(int & value,unsigned int bits,unsigned int times)
{
    int mask=(0x1 << (bits-1));
    
    while(times-->0)
    {
        if((0x1 & value))
        {
            value=(value >> 1) & mask;
        }
        else
        {
            value=(value >> 1) | !mask;
        }
    }
}

/*
根据中间代码的数据获得一个operand2

Parameters
----------
ic_data:要处理的中间代码数据

Return
------
返回得到的operand2
*/
struct operand2 Arm_instruction_generator::get_operand2(struct ic_data * ic_data,enum operand2_shift_op shift_op,unsigned int n)
{
    struct operand2 res;
    int const_int_value;
    reg_index Rm;
    OAA const_value;
    if(ic_data->is_const() && shift_op!=operand2_shift_op::RRX)
    {
        const_int_value=ic_data->get_value().int_data;
        switch(shift_op)
        {
            case operand2_shift_op::ASR_N:
                ASR_N(const_int_value,8,n);
                break;
            case operand2_shift_op::LSL_N:
                LSL_N(const_int_value,8,n);
                break;
            case operand2_shift_op::LSR_N:
                LSR_N(const_int_value,8,n);
                break;
            case operand2_shift_op::ROR_N:
                ROR_N(const_int_value,8,n);
                break;
            default:
                break;
        }
        res=get_operand2(const_int_value);
    }
    else
    {
        if(ic_data->is_const())
        {
            const_value=OAA((int)ic_data->get_value().int_data);
            res=operand2((reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)&const_value)).int_data,shift_op,n);
        }
        else
        {
            res=operand2((reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)ic_data)).int_data,shift_op,n);
        }
    }
    return res;
}

/*
根据int常数获得一个operand2

Parameters
----------
const_int_value:要处理的int常数
get_an_empty_reg:如果指定为true，表示要获取为这个opernad2获取一个空的寄存器

Return
------
返回得到的operand2
*/
struct operand2 Arm_instruction_generator::get_operand2(int const_int_value,bool get_an_empty_reg)
{
    struct operand2 res;
    reg_index Rm;
    OAA const_value;
    if(operand2::is_legal_immed_8r(const_int_value))
    {
        res=operand2(const_int_value);
    }
    else
    {
        const_value=OAA((int)const_int_value);
        if(get_an_empty_reg)
        {
            Rm=(reg_index)notify(event(event_type::GET_AN_EMPTY_CPU_REG_FOR_CONST,(void *)(&const_value))).int_data;
        }
        else
        {
            Rm=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)(&const_value))).int_data;
        }
        res=operand2(Rm);
    }
    return res;
}

/*
根据常数偏移量获得一个flexoffset

Parameters
----------
offset:要获取的偏移量

Return
------
返回得到的flexoffset
*/
struct flexoffset Arm_instruction_generator::get_flexoffset(int offset)
{
    struct flexoffset res;
    OAA const_value;
    if(flexoffset::is_legal_expr(offset))
    {
        res=flexoffset(offset);
    }
    else
    {
        const_value=OAA((int)offset);
        res=flexoffset(false,(reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)&const_value)).int_data,flexoffset_shift_op::NONE,0);
    }
    return res;
}

/*
根据变量偏移量获得一个flexoffset

Parameters
----------
ic_data:要获取的变量

Return
------
返回得到的flexoffset
*/
struct flexoffset Arm_instruction_generator::get_flexoffset(struct ic_data * ic_data,enum flexoffset_shift_op shift_op,unsigned int n,bool is_negative)
{
    struct flexoffset res;
    int offset;
    OAA const_value;
    if(ic_data->is_const() && shift_op!=flexoffset_shift_op::RRX)
    {
        offset=ic_data->get_value().int_data;
        switch(shift_op)
        {
            case flexoffset_shift_op::ASR_N:
                ASR_N(offset,13,n);
                break;
            case flexoffset_shift_op::LSL_N:
                LSL_N(offset,13,n);
                break;
            case flexoffset_shift_op::LSR_N:
                LSR_N(offset,13,n);
                break;
            case flexoffset_shift_op::ROR_N:
                ROR_N(offset,13,n);
                break;
            default:
                break;
        }
        res=get_flexoffset(offset);
    }
    else
    {
        if(ic_data->is_const())
        {
            const_value=OAA((int)ic_data->get_value().int_data);
            res=flexoffset(is_negative,(reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)&const_value)).int_data,shift_op,n);
        }
        else
        {
            res=flexoffset(is_negative,(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,ic_data)).int_data,shift_op,n);
        }
    }
    return res;
}

/*
根据一条ASSIGN中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::assign_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result)
{
    pair<struct ic_data *,struct ic_data * > * event_data=new pair<struct ic_data *,struct ic_data * >(arg1,result);
    notify(event(event_type::ASSIGN_VAR,event_data));
    delete event_data;
}

/*
根据一条ADD中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::add_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rd,Rn,Rm;
    struct operand2 operand2;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                //如果ADD的第一个操作数是常数
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
            }
            //如果ADD的第二个操作数是常数
            //或者两个操作数都是变量
            //对于这种既要写变量，又要读变量的指令，必须先获取所有的读变量所需要的寄存器，才能获取所有的写变量所需的寄存器
            //考虑下面的例子：
            //假设此时有一条语句b=a[b]+c
            //我们知道：当这条指令执行完毕之后，由于b的值被改写了，因此如果原本有脏值寄存器里面存在着a[b]的值，那么这个值需要在b被正式改写之前写回寄存器，并将该值a[b]和该寄存器之间解除关联
            //那么如果此时我们先获取b所需的寄存器，然后再去获取a[b]所需的寄存器
            //那么在获取b所需的寄存器的时候会先把a[b]与它所在的寄存器解除关联（如果原本是脏值的话还会进行写回）
            //接着在获取a[b]所需的寄存器的时候就又会把a[b]的值从内存中读入某一个寄存器，并且这个寄存器会和a[b]建立联系
            //然后接下来这条ADD指令翻译完毕之后，就会有一个寄存器中存放的是该指令执行之前的a[b]的值，这样是错误的
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            Rd=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,Rd,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(arm_op::VADD,arm_condition::NONE,precision::S,Rd,Rn,Rm));
            break;
        default:
            break;
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条SUB中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::sub_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    reg_index Rd,Rn,Rm;
    struct operand2 operand2;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                //如果SUB的第一个操作数是常数
                Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                operand2=get_operand2(arg1);
                Rd=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_WRITING_VAR,(void *)result)).int_data;
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::RSB,arm_condition::NONE,false,Rd,Rn,operand2));
            }
            else
            {
                //如果SUB的第二个操作数是常数
                //或者两个操作数都是变量
                Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                operand2=get_operand2(arg2);
                Rd=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_WRITING_VAR,(void *)result)).int_data;
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,Rd,Rn,operand2));
            }
            break;
        case language_data_type::FLOAT:
            Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(arm_op::VSUB,arm_condition::NONE,precision::S,Rd,Rn,Rm));
            break;
        default:
            break;
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条MUL中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::mul_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    reg_index Rd,Rm,Rs;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            Rm=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MUL,arm_condition::NONE,false,Rd,Rm,Rs));
            break;
        case language_data_type::FLOAT:
            Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(arm_op::VMUL,arm_condition::NONE,precision::S,Rd,Rm,Rs));
            break;
        default:
            break;
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条DIV中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::div_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    list<struct ic_data * > * r_params;
    pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * event_data;
    reg_index Rd,Rm,Rs;
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            r_params=new list<struct ic_data * >;
            event_data=new pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * >;
            event_data->first.first.first="__aeabi_idiv";
            r_params->push_back(arg1);
            r_params->push_back(arg2);
            event_data->first.first.second=r_params;
            event_data->first.second.first=result;
            event_data->first.second.second=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
            event_data->second=nullptr;
            notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
            delete r_params;
            delete event_data;
            break;
        case language_data_type::FLOAT:
            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(arm_op::VDIV,arm_condition::NONE,precision::S,Rd,Rm,Rs));
            notify(event(event_type::END_INSTRUCTION,nullptr));
            break;
        default:
            break;
    }
}

/*
根据一条MOD中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::mod_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    list<struct ic_data * > * r_params;
    pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * event_data;
    //不能对浮点数进行mod
    r_params=new list<struct ic_data * >;
    event_data=new pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * >;
    event_data->first.first.first="__aeabi_idivmod";
    r_params->push_back(arg1);
    r_params->push_back(arg2);
    event_data->first.first.second=r_params;
    event_data->first.second.first=result;
    event_data->first.second.second=(reg_index)notify(event(event_type::GET_R1_REG,nullptr)).int_data;
    event_data->second=nullptr;
    notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
    delete r_params;
    delete event_data;
}

// /*
// 根据一条L_SHIFT中间代码生成最终的arm汇编代码
// */
// void Arm_instruction_generator::l_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
// {

// }

// /*
// 根据一条R_SHIFT中间代码生成最终的arm汇编代码
// */
// void Arm_instruction_generator::r_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
// {

// }

/*
根据一条NOT中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::not_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result)
{
    reg_index Rn,Rd;
    struct operand2 op2;
    pair<struct ic_data *,int> * event_data;
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            op2=get_operand2((int)0);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,op2));
            event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::EQ));
            notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
            delete event_data;
            notify(event(event_type::END_INSTRUCTION,nullptr));

            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rd=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction("eq","e"));
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::EQ,false,Rd,operand2((int)1)));
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NE,false,Rd,operand2((int)0)));
            notify(event(event_type::END_INSTRUCTION,nullptr));
            break;
        case language_data_type::FLOAT:
            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::EQ));
            notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
            delete event_data;
            notify(event(event_type::END_INSTRUCTION,nullptr));

            // notify(event(event_type::START_INSTRUCTION,nullptr));
            // Rd=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            // push_pseudo_instruction(new Arm_pseudo_instruction("eq","e"));
            // push_instruction(new Arm_vfp_register_transfer_instruction(arm_condition::EQ,Rd,1.f));
            // push_instruction(new Arm_vfp_data_process_instruction(arm_op::VSUB,arm_condition::NE,precision::S,Rd,Rd,Rd));
            // notify(event(event_type::END_INSTRUCTION,nullptr));
            break;
        default:
            break;
    }
}

// /*
// 根据一条AND中间代码生成最终的arm汇编代码
// */
// void Arm_instruction_generator::and_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
// {

// }

// /*
// 根据一条OR中间代码生成最终的arm汇编代码
// */
// void Arm_instruction_generator::or_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
// {

// }

// /*
// 根据一条XOR中间代码生成最终的arm汇编代码
// */
// void Arm_instruction_generator::xor_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
// {

// }

/*
根据一条EQ中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::eq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                //如果EQ的第一个操作数是常数
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
            }
            //如果EQ的第二个操作数是常数
            //或者两个操作数都是变量
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::EQ));
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条UEQ中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::ueq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
            }
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::NE));
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条GT中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::gt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    enum arm_condition cond=arm_condition::GT;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
                cond=arm_condition::LE;
            }
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                cond=arm_condition::LE;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)cond);
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条LT中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::lt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    enum arm_condition cond=arm_condition::LT;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
                cond=arm_condition::GE;
            }
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                cond=arm_condition::GE;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)cond);
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条GE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::ge_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    enum arm_condition cond=arm_condition::GE;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
                cond=arm_condition::LT;
            }
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                cond=arm_condition::LT;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)cond);
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条LE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::le_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{
    struct ic_data * tmp;
    reg_index Rn,Rm;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    enum arm_condition cond=arm_condition::LE;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                tmp=arg1;
                arg1=arg2;
                arg2=tmp;
                cond=arm_condition::GT;
            }
            Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                cond=arm_condition::GT;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
            }
            break;
        default:
            break;
    }
    event_data=new pair<struct ic_data *,int>(result,(int)cond);
    notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
    delete event_data;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

/*
根据一条JMP中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::jmp_ic_to_arm_asm(struct ic_label * result)
{
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为EMPTY
    notify(event(event_type::END_BASIC_BLOCK_WITHOUT_FLAG,nullptr));
    push_instruction(new Arm_cpu_branch_instruction(arm_op::B,arm_condition::NONE,(result->name)));
    //并在跳转结束之后通知内存管理器：此时要把标志寄存器也给清空
    notify(event(event_type::CLEAR_FLAG,nullptr));
}

/*
根据一条IF_JMP中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::if_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result)
{
    reg_index Rn;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    switch(arg1->get_data_type())
    {
        case language_data_type::INT:
            //首先查看此时要进行跳转的转移条件是否还存在，如果不存在，就需要一次测试，将标志寄存器中的标志位置位
            if(!notify(event(event_type::CHECK_VAR_ATTACHED_TO_FLAG,(void *)arg1)).bool_data)
            {
                notify(event(event_type::START_INSTRUCTION,nullptr));
                Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                operand2=get_operand2((int)0);
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        case language_data_type::FLOAT:
            //首先查看此时要进行跳转的转移条件是否还存在，如果不存在，就需要一次测试，将标志寄存器中的标志位置位
            if(!notify(event(event_type::CHECK_VAR_ATTACHED_TO_FLAG,(void *)arg1)).bool_data)
            {
                notify(event(event_type::START_INSTRUCTION,nullptr));
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        default:
            break;
    }
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为EMPTY
    notify(event(event_type::END_BASIC_BLOCK_WITHOUT_FLAG,nullptr));
    push_instruction(new Arm_cpu_branch_instruction(arm_op::B,arm_condition(notify(event(event_type::GET_VAR_CARED_FLAG,nullptr)).int_data),(result->name)));
    //并在跳转结束之后通知内存管理器：此时要把标志寄存器也给清空
    notify(event(event_type::CLEAR_FLAG,nullptr));
}

enum arm_condition reverse_arm_condition(enum arm_condition cond)
{
    switch(cond)
    {
        case arm_condition::EQ:
            return arm_condition::NE;
        case arm_condition::NE:
            return arm_condition::EQ;
        case arm_condition::GE:
            return arm_condition::LT;
        case arm_condition::LE:
            return arm_condition::GT;
        case arm_condition::GT:
            return arm_condition::LE;
        case arm_condition::LT:
            return arm_condition::GE;
        default:
            break;
    }
    return arm_condition::NONE;
}

/*
根据一条IF_NOT_JMP中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::if_not_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result)
{
    reg_index Rn;
    struct operand2 operand2;
    pair<struct ic_data *,int> * event_data;
    switch(arg1->get_data_type())
    {
        case language_data_type::INT:
            //首先查看此时要进行跳转的转移条件是否还存在，如果不存在，就需要再进行一次测试，将标志寄存器中的标志位置位
            if(!notify(event(event_type::CHECK_VAR_ATTACHED_TO_FLAG,(void *)arg1)).bool_data)
            {
                notify(event(event_type::START_INSTRUCTION,nullptr));
                Rn=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                operand2=get_operand2((int)0);
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        case language_data_type::FLOAT:
            //首先查看此时要进行跳转的转移条件是否还存在，如果不存在，就需要再进行一次测试，将标志寄存器中的标志位置位
            if(!notify(event(event_type::CHECK_VAR_ATTACHED_TO_FLAG,(void *)arg1)).bool_data)
            {
                notify(event(event_type::START_INSTRUCTION,nullptr));
                Rn=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMRS,arm_condition::NONE,(reg_index)notify(event(event_type::GET_APSR_REG,nullptr)).int_data,(reg_index)notify(event(event_type::GET_FPSCR_REG,nullptr)).int_data));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        default:
            break;
    }
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为EMPTY
    notify(event(event_type::END_BASIC_BLOCK_WITHOUT_FLAG,nullptr));
    push_instruction(new Arm_cpu_branch_instruction(arm_op::B,reverse_arm_condition(arm_condition(notify(event(event_type::GET_VAR_CARED_FLAG,nullptr)).int_data)),(result->name)));
    //并在跳转结束之后通知内存管理器：此时要把标志寄存器也给清空
    notify(event(event_type::CLEAR_FLAG,nullptr));
}

/*
根据一条VAR_DEFINE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::var_define_ic_to_arm_asm(struct ic_data * result)
{
    static Symbol_table * symbol_table=Symbol_table::get_instance();
    size_t zero_array_member_num=0,array_member_offset=0;
    vector<OAA> * array_init_values;
    reg_index Rd;
    struct operand2 operand2;
    pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * event_data_1;
    pair<struct ic_data *,struct ic_data * > * event_data_2;
    list<struct ic_data * > * r_params;
    if(result->is_pure_const())
    {
        return;
    }
    if(result->is_global())
    {
        if(result->is_array_var())
        {
            //如果变量是全局数组变量
            if((result->get_value().pointer_data)==nullptr)
            {
                push_directive(new Arm_directive(arm_directive_type::COMM,{result->var_name,to_string(result->get_byte_size()),"4"}));
            }
            else
            {
                push_directive(new Arm_directive(arm_directive_type::GLOBAL,{result->var_name}));
                array_init_values=(vector<OAA> * )result->get_value().pointer_data;
                if(result->is_const_var())
                {
                    push_directive(new Arm_directive(arm_directive_type::SECTION,{".rodata"}));
                }
                else
                {
                    push_directive(new Arm_directive(arm_directive_type::DATA,{}));
                }
                push_directive(new Arm_directive(arm_directive_type::ALIGN,{"2"}));
                push_directive(new Arm_directive(arm_directive_type::TYPE,{result->var_name,"%object"}));
                push_directive(new Arm_directive(arm_directive_type::SIZE,{result->var_name,to_string(result->get_byte_size())}));
                push_label_to_global(new Arm_label(result->var_name));
                for(auto i:*array_init_values)
                {
                    if(i.int_data!=0)
                    {
                        //默认变量都是4bytes的
                        if(zero_array_member_num!=0)
                        {
                            push_directive(new Arm_directive(arm_directive_type::SPACE,{to_string(zero_array_member_num*bits_of_data_types.at(result->data_type)/8)}));
                            zero_array_member_num=0;
                        }
                        push_directive(new Arm_directive(arm_directive_type::WORD,{to_string(i.int_data)}));
                    }
                    else
                    {
                        zero_array_member_num++;
                    }
                }
                if(zero_array_member_num!=0)
                {
                    push_directive(new Arm_directive(arm_directive_type::SPACE,{to_string(zero_array_member_num*bits_of_data_types.at(result->data_type)/8)}));
                }
            }
        }
        else
        {
            //如果变量是全局普通变量
            if(result->is_const_var())
            {
                return;
            }
            if((result->get_value().pointer_data)!=nullptr)
            {
                push_directive(new Arm_directive(arm_directive_type::GLOBAL,{result->var_name}));
                push_directive(new Arm_directive(arm_directive_type::DATA,{}));
                push_directive(new Arm_directive(arm_directive_type::ALIGN,{"2"}));
                push_directive(new Arm_directive(arm_directive_type::TYPE,{result->var_name,"%object"}));
                push_directive(new Arm_directive(arm_directive_type::SIZE,{result->var_name,to_string(result->get_byte_size())}));
                push_label_to_global(new Arm_label(result->var_name));
                push_directive(new Arm_directive(arm_directive_type::WORD,{to_string(((struct ic_data *)result->get_value().pointer_data)->get_value().int_data)}));
            }
            else
            {
                push_directive(new Arm_directive(arm_directive_type::COMM,{result->var_name,to_string(result->get_byte_size()),"4"}));
            }
        }
    }
    else
    {
        //只需要对有初始化的局部变量进行赋初值即可
        //如果函数是局部数组变量,并且进行了初始化
        if(result->is_array_var())
        {
            if((result->is_const_var() && result->get_value().pointer_data) || (!result->is_const_var() && result->get_value().bool_data))
            {
                //数组局部变量初始化的时候需要使用gcc的内置汇编函数memset先把数组的所有元素全都置为0
                event_data_1=new pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * >;
                event_data_1->first.first.first=string("memset");
                r_params=new list<struct ic_data *>;
                r_params->push_back(result);
                r_params->push_back(symbol_table->const_entry(language_data_type::INT,OAA((int)0)));
                r_params->push_back(symbol_table->const_entry(language_data_type::INT,OAA((int)result->get_byte_size())));
                event_data_1->first.first.second=r_params;
                event_data_1->first.second.first=nullptr;
                event_data_1->second=nullptr;
                notify(event(event_type::CALL_ABI_FUNC,event_data_1));
                delete r_params;
                delete event_data_1;
            }
            //再把那些常量数组中不是0的数组元素进行赋值
            if(result->is_const_var() && result->get_value().pointer_data)
            {
                event_data_2=new pair<struct ic_data *,struct ic_data *>;
                for(auto i:(*((vector<OAA> *)result->get_value().pointer_data)))
                {
                    event_data_2->first=symbol_table->const_entry(result->data_type,i);
                    event_data_2->second=symbol_table->array_member_not_array_var_entry(result,symbol_table->const_entry(language_data_type::INT,OAA((int)array_member_offset)));
                    notify(event(event_type::ASSIGN_VAR,(void *)event_data_2));
                    array_member_offset++;
                }
            }
        }
    }
}

/*
根据一条LABEL_DEFINE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::label_define_ic_to_arm_asm(struct ic_label * result)
{
    push_label_to_func(new Arm_label(result->name));
}

/*
根据一条FUNC_DEFINE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::func_define_ic_to_arm_asm(struct ic_func * result)
{
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data,fp=(reg_index)notify(event(event_type::GET_FP_REG,nullptr)).int_data;
    size_t local_vars_total_byte_size=0,padding_bytes;
    list<reg_index> * context_saved_regs;//,* f_param_regs;
    //首先通知内存管理器和寄存器管理器，新的函数定义开始了
    notify(event(event_type::FUNC_DEFINE,(void *)result));
    //然后保护现场，把那些在函数的调用者看来不会改变的寄存器入栈保存
    //先保存CPU中的寄存器,包括CPU中的所有临时寄存器，fp，lr
    context_saved_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,(void *)result)).pointer_data;
    if(!context_saved_regs->empty())
    {
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(*context_saved_regs)));
    }
    delete context_saved_regs;
    //再保存VFP中的所有临时寄存器
    context_saved_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_TEMP_VFP_REGS,(void *)result)).pointer_data;
    if(!context_saved_regs->empty())
    {
        push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(*context_saved_regs)));
    }
    delete context_saved_regs;
    //再计算fp寄存器的值
    push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,fp,sp,get_operand2(notify(event(event_type::GET_CURRENT_FUNC_STACK_SIZE,nullptr)).int_data)));
    padding_bytes=notify(event(event_type::CACULATE_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC,nullptr)).int_data;
    push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2((int)padding_bytes)));
    //最后在栈中为函数的函数形参，局部变量以及那些会跨越基本块的临时变量开辟空间
    local_vars_total_byte_size=notify(event(event_type::READY_TO_PUSH_F_PARAM_PASSED_BY_REGS_AND_LOCAL_VARS_AND_TEMP_VARS_OVER_BASIC_BLOCK,(void *)result)).int_data;
    if(local_vars_total_byte_size>0)
    {
        //这里如果需要为local_vars_total_byte_size分配寄存器，那么这个寄存器必须是还没有被使用过的
        //因为此时被使用过的寄存器都是存放了参数，如果使用这些寄存器，会把函数参数进行写回，但是此时的sp还没有正确的移动，因此写回就回出错
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2((int)local_vars_total_byte_size,true)));
    }
}

/*
根据一条END_FUNC_DEFINE中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::end_func_define_ic_to_arm_asm()
{
    ;
}

/*
根据一条CALL中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::call_ic_to_arm_asm(struct ic_func * arg1,list<struct ic_data * > * arg2,struct ic_data * result)
{
    pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * event_data;
    event_data=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
    event_data->first.first=arg1->name;
    event_data->first.second=arg2;
    event_data->second.first=result;
    if(result)
    {
        switch(result->get_data_type())
        {
            case language_data_type::INT:
                event_data->second.second=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
                break;
            case language_data_type::FLOAT:
                event_data->second.second=(reg_index)notify(event(event_type::GET_S0_REG,nullptr)).int_data;
                break;
            default:
                break;
        }
    }
    notify(event(event_type::CALL_FUNC,(void *)event_data));
    delete event_data;
}

/*
根据一条RET中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::ret_ic_to_arm_asm(struct ic_data * result)
{
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    size_t stack_offset=0;
    list<reg_index> * regs;
    set<reg_index> * regs_unaccessible;
    reg_index const_reg;
    struct operand2 op2;
    pair<OAA,reg_index> * event_data;
    //先把此时留在寄存器中的脏值全部写回内存
    notify(event(event_type::FUNC_RET,nullptr));
    //再把返回值放入寄存器
    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(result)
    {
        notify(event(event_type::RETURN_VAR,(void *)result));
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
    //把使用到的临时变量，局部变量，前16*32bits的浮点型参数，前4*4bytes的整型参数以及padding退栈
    stack_offset=notify(event(event_type::READY_TO_POP_WHEN_RET,nullptr)).int_data;
    if(stack_offset>0)
    {
        //此时需要将存放返回值的寄存器设置为不可获取的，这里默认是r0
        regs_unaccessible=new set<reg_index>;
        regs_unaccessible->insert((reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data);
        notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
        delete regs_unaccessible;
        //这里不能使用如下的方法：
        //op2=get_operand2((int)stack_offset);
        //push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,op2));
        //因为在第1行的get_operand2()的时候可能也会进行变量的出栈入栈，导致stack_offset无效
        //因此我们只能采用如下的比较麻烦的方法
        if(operand2::is_legal_immed_8r((int)stack_offset))
        {
            //如果此时stack_offset可以作为指令的一部分，不需要放入寄存器中
            op2=get_operand2((int)stack_offset);
        }
        else
        {
            //如果此时stack_offset需要放入寄存器之后再使用
            if(notify(event(event_type::CHECK_CONST_INT_VALUE_OWN_CPU_REG,(int)stack_offset)).bool_data)
            {
                //如果此时该stack_offset的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                op2=operand2((reg_index)notify(event(event_type::GET_CONST_VALUE_S_CPU_REG,(int)stack_offset)).int_data);
            }
            else
            {
                //如果此时没有寄存器中存放着stack_offset的值，那么就需要给该值新分配一个寄存器
                const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                //分配完之后还要再得到新的stack_offset，因为上面的ALLOCATE_IDLE_REG可能会使得新的临时变量入栈，从而栈顶的位置发送变化
                stack_offset=notify(event(event_type::READY_TO_POP_WHEN_RET,nullptr)).int_data;
                //然后把新的stack_offset值写入寄存器中
                event_data=new pair<OAA,reg_index>(OAA((int)stack_offset),const_reg);
                notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data));
                //再通知内存管理模块：寄存器被新的stack_offset占用了
                notify(event(event_type::ATTACH_CONST_TO_REG,(void *)event_data));
                delete event_data;
                //最后把得到的寄存器作为operand2的一部分即可
                op2=operand2(const_reg);
            }
        }
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,op2));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
    //最后计算padding
    stack_offset=notify(event(event_type::GET_PADDING_BYTES_BEFORE_LOCAL_VARS_IN_CURRENT_FUNC,nullptr)).int_data;
    regs_unaccessible=new set<reg_index>;
    regs_unaccessible->insert((reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data);
    notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
    delete regs_unaccessible;
    push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,get_operand2((int)stack_offset)));
    notify(event(event_type::END_INSTRUCTION,nullptr));
    //最后恢复现场
    //先把保护起来的浮点寄存器恢复
    regs=(list<reg_index> *)notify(event(event_type::READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS,nullptr)).pointer_data;
    if(regs->size()>0)
    {
        push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPOP,arm_condition::NONE,arm_registers(*regs)));
    }
    delete regs;
    //再把整型寄存器恢复，把栈中的lr的值赋值给pc即可完成函数返回
    regs=(list<reg_index> *)notify(event(event_type::READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS,nullptr)).pointer_data;
    if(regs->size()>0)
    {
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::POP,arm_condition::NONE,arm_registers(*regs)));
    }
    delete regs;
}

/*
把一个基本块开始的部分转换成arm汇编代码
*/
void Arm_instruction_generator::start_basic_block_to_arm_asm()
{
    size_t temp_vars_total_byte_size=0;
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    set<struct ic_data * > temp_vars_not_over_basic_block;
    set<struct ic_data * > * temp_vars_in_basic_block=(set<struct ic_data * > * )notify(event(event_type::GET_TEMP_VARS_IN_CURRENT_BASIC_BLOCK,nullptr)).pointer_data;
    set<struct ic_data * > * temp_vars_over_basic_blocks=(set<struct ic_data * > * )notify(event(event_type::GET_TEMP_VARS_OVER_BASIC_BLOCK_IN_CURRENT_FUNC,nullptr)).pointer_data;
    set_difference(temp_vars_in_basic_block->begin(),temp_vars_in_basic_block->end(),temp_vars_over_basic_blocks->begin(),temp_vars_over_basic_blocks->end(),inserter(temp_vars_not_over_basic_block,temp_vars_not_over_basic_block.begin()));
    delete temp_vars_in_basic_block;
    temp_vars_total_byte_size=notify(event(event_type::READY_TO_PUSH_TEMP_VARS,(void *)(&temp_vars_not_over_basic_block))).int_data;
    if(temp_vars_total_byte_size>0)
    {
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2((int)temp_vars_total_byte_size)));
    }
}

/*
把一行instruction插入到正在生成的函数的最后

Parameters
----------
instruction:要插入的instruction
*/
void Arm_instruction_generator::push_instruction(Arm_instruction * instruction)
{
    arm_flow_graph_.add_arm_asm_to_func(instruction,false);
}

/*
把一行pseudo instruction插入到正在生成的函数的最后

Parameters
----------
pseudo instruction:要插入的pseudo instruction
*/
void Arm_instruction_generator::push_pseudo_instruction(Arm_pseudo_instruction * pseudo_instruction)
{
    arm_flow_graph_.add_arm_asm_to_func(pseudo_instruction,false);
}

/*
把一行label插入到当前的汇编函数中

Parameters
----------
label:要插入的label
*/
void Arm_instruction_generator::push_label_to_func(Arm_label * label)
{
    arm_flow_graph_.add_arm_asm_to_func(label,false);
}

/*
把一行label插入到当前的汇编的全局区域中

Parameters
----------
label:要插入的label
*/
void Arm_instruction_generator::push_label_to_global(Arm_label * label)
{
    arm_flow_graph_.add_arm_asm_to_global(label);
}

/*
把一行directive插入到汇编的全局区域当中

Parameters
----------
directive:要插入的directive
*/
void Arm_instruction_generator::push_directive(Arm_directive * directive)
{
    arm_flow_graph_.add_arm_asm_to_global(directive);
}

void Arm_instruction_generator::handle_WRITE_CONST_TO_REG(OAA const_data,reg_index reg)
{
    reg_index cpu_reg;
    set<reg_index> regs_unaccessible;
    if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
    {
        union {
            int int_data;
            unsigned int unsigned_int_data;
        } signed_int_to_unsigned_int;
        signed_int_to_unsigned_int.int_data=const_data.int_data;
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVW,arm_condition::NONE,reg,immed_16r(signed_int_to_unsigned_int.unsigned_int_data & 0x0000ffff)));
        signed_int_to_unsigned_int.unsigned_int_data=signed_int_to_unsigned_int.unsigned_int_data >> 16;
        if(signed_int_to_unsigned_int.unsigned_int_data>0)
        {
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVT,arm_condition::NONE,reg,immed_16r(signed_int_to_unsigned_int.unsigned_int_data)));
        }
    }
    else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
    {
        union {
            float float_data;
            unsigned int unsigned_int_data;
        } signed_float_to_unsigned_int;
        signed_float_to_unsigned_int.float_data=const_data.float_data;
        regs_unaccessible.insert(reg);
        notify(event(event_type::START_INSTRUCTION,(void *)&regs_unaccessible));
        cpu_reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)&const_data)).int_data;
        notify(event(event_type::END_INSTRUCTION,nullptr));
        push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMOV,arm_condition::NONE,reg,cpu_reg));
        //push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,precision::S,reg,::to_string(const_data.int_data)));
    }
}

void Arm_instruction_generator::handle_STORE_VAR_TO_MEM(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp;
    reg_index addr_reg,offset_reg,new_reg;
    set<reg_index> * regs_unaccessible;
    pair<reg_index,reg_index> * event_data_1;
    pair<struct ic_data * ,reg_index> * event_data_2;

    if(var->is_const() || (var->is_array_var() && !var->is_f_param()) || (var->is_array_member() && var->is_array_var()))
    {
        //常量数据不需要写回
        //对于数组变量来说，只有那些函数形参才能写回
        //如果一个变量即是数组，又是数组取元素，那么就意味着这个变量其实是一个数组取元素，只不过没有全部维度都用于取元素，这种变量也是不能写回的
        return;
    }
    
    regs_unaccessible=new set<reg_index>;
    regs_unaccessible->insert(reg);
    notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
    delete regs_unaccessible;
    if(var->is_array_member())
    {
        //对于数组取元素的变量，需要先获取其所属数组的地址，然后取偏移
        addr_reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
        //取偏移的时候需要乘上数组元素类型的大小（这里默认是4）
        if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
        {
            push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
        }
        else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
        {
            if(var->get_offset()->is_const() && var->get_offset()->get_value().int_data<=(1020/4) && var->get_offset()->get_value().int_data>=(-1020/4))
            {
                push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VSTR,arm_condition::NONE,precision::S,reg,addr_reg,var->get_offset()->get_value().int_data*4,vfp_single_register_load_and_store_type::RN_OFFSET));
            }
            else
            {
                new_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                event_data_1=new pair<reg_index,reg_index>(reg,new_reg);
                notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_1));
                delete event_data_1;
                event_data_2=new pair<struct ic_data *,reg_index>(var,new_reg);
                notify(event(event_type::ATTACH_VAR_VALUE_TO_REG,(void *)event_data_2));
                delete event_data_2;
                regs_unaccessible=new set<reg_index>;
                regs_unaccessible->insert(new_reg);
                notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
                delete regs_unaccessible;
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,new_reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
        }
    }
    else
    {
        //对于全局变量,临时变量和那些非数组取元素的局部变量和函数形参，先获取他们的地址寄存器，然后再将其保存到内存中
        addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
        if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
        {
            push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
        }
        else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
        {
            push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VSTR,arm_condition::NONE,precision::S,reg,addr_reg));
        }
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_LOAD_VAR_TO_REG(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp;
    reg_index addr_reg,const_reg,new_reg;
    struct operand2 op2;
    pair<OAA,reg_index> * event_data;
    pair<reg_index,reg_index> * event_data_1;
    pair<struct ic_data * ,reg_index> * event_data_2;
    set<reg_index> * regs_unaccessible;
    struct flexoffset offset;
    
    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(var->is_array_var())
    {
        //如果要load到寄存器中的变量是数组
        if(var->is_f_param())
        {
            //如果该数组是函数形参，那么此时函数形参传递的实际上是数组的地址
            addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
            push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
        }
        else if(var->is_global())
        {
            //如果该数组是全局变量，那么就把数组的首地址放入寄存器即可
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVW,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::LOWER16)));
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVT,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::UPPER16)));
        }
        else if(var->is_array_member())
        {
            //如果该数组同时也是数组取元素变量
            addr_reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
            //默认数组元素的大小都是4bytes
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,reg,addr_reg,get_operand2((var->get_offset()),operand2_shift_op::LSL_N,2)));
        }
        else
        {
            //如果该数组是局部变量，也只需要把数组首地址放入寄存器
            addr_reg=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
            //这里不能使用如下的方法：
            //var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
            //op2=get_operand2(var_stack_pos_from_sp);
            //因为在第2行的get_operand2()的时候可能也会进行变量的出栈入栈，导致第1行获得的var_stack_pos_from_sp无效
            //因此我们只能采用如下的比较麻烦的方法
            var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
            if(operand2::is_legal_immed_8r(var_stack_pos_from_sp))
            {
                //如果此时var_stack_pos_from_sp可以作为指令的一部分，不需要放入寄存器中
                op2=get_operand2(var_stack_pos_from_sp);
            }
            else
            {
                //如果此时var_stack_pos_from_sp需要放入寄存器之后再使用
                if(notify(event(event_type::CHECK_CONST_INT_VALUE_OWN_CPU_REG,(int)var_stack_pos_from_sp)).bool_data)
                {
                    //如果此时该var_stack_pos_from_sp的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                    op2=operand2((reg_index)notify(event(event_type::GET_CONST_VALUE_S_CPU_REG,(int)var_stack_pos_from_sp)).int_data);
                }
                else
                {
                    //如果此时没有寄存器中存放着var_stack_pos_from_sp的值，那么就需要给该值新分配一个寄存器
                    const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                    //分配完之后还要再进行一次GET_VAR_STACK_POS_FROM_SP，得到新的var_stack_pos_from_sp，因为上面的ALLOCATE_IDLE_REG可能会使得栈顶的位置发送变化
                    var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
                    //然后把新的var_stack_pos_from_sp值写入寄存器中
                    event_data=new pair<OAA,reg_index>(OAA((int)var_stack_pos_from_sp),const_reg);
                    notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data));
                    //再通知内存管理模块：寄存器被新的stack_offset占用了
                    notify(event(event_type::ATTACH_CONST_TO_REG,(void *)event_data));
                    delete event_data;
                    //最后把得到的寄存器作为operand2的一部分即可
                    op2=operand2(const_reg);
                }
            }
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,reg,addr_reg,op2));
        }
    }
    else
    {
        if(var->is_array_member())
        {
            //如果要load到寄存器中的变量是数组取元素,并且该数组取元素不是数组
            addr_reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
            //默认数组元素的大小都是4bytes
            if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
            {
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
            }
            else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
            {
                if(var->get_offset()->is_const() && var->get_offset()->get_value().int_data<=(1020/4) && var->get_offset()->get_value().int_data>=(-1020/4))
                {
                    push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,addr_reg,var->get_offset()->get_value().int_data*4,vfp_single_register_load_and_store_type::RN_OFFSET));
                }
                else
                {
                    regs_unaccessible=new set<reg_index>;
                    regs_unaccessible->insert(reg);
                    notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
                    delete regs_unaccessible;
                    offset=get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false);
                    new_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                    push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,new_reg,addr_reg,offset,false));
                    notify(event(event_type::END_INSTRUCTION,nullptr));
                    event_data_2=new pair<struct ic_data *,reg_index>(var,new_reg);
                    notify(event(event_type::ATTACH_VAR_VALUE_TO_REG,(void *)event_data_2));
                    delete event_data_2;
                    event_data_1=new pair<reg_index,reg_index>(new_reg,reg);
                    notify(event(event_type::MOVE_DATA_BETWEEN_REGS,(void *)event_data_1));
                    delete event_data_1;
                }
            }
        }
        else
        {
            //如果要load到寄存器中的变量是全局变量或者局部变量或者函数形参或者临时变量
            addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
            if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
            {
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
            }
            else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
            {
                push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,addr_reg));
            }
        }
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_WRITE_ADDR_TO_REG(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp,var_stack_pos_from_fp;
    reg_index sp,fp,const_reg;
    struct operand2 op2;
    pair<OAA,reg_index> * event_data;

    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(var->is_global())
    {
        //如果要获取地址的变量是全局变量
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVW,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::LOWER16)));
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVT,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::UPPER16)));
    }
    else if(notify(event(event_type::IS_F_PARAM_PASSED_BY_STACK,(void *)var)).bool_data)
    {
        //如果要获取地址的变量是通过栈传递的函数参数
        fp=(reg_index)notify(event(event_type::GET_FP_REG,nullptr)).int_data;
        var_stack_pos_from_fp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_FP,(void *)var)).int_data;
        op2=get_operand2(var_stack_pos_from_fp);
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,reg,fp,op2));
    }
    else
    {
        //如果要获取地址的变量是局部变量或者临时变量或者通过寄存器传递的函数参数
        sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
        //这里不能使用如下的方法：
        //var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
        //op2=get_operand2(var_stack_pos_from_sp);
        //因为在第2行的get_operand2()的时候可能也会进行变量的出栈入栈，导致第1行获得的var_stack_pos_from_sp无效
        //因此我们只能采用如下的比较麻烦的方法
        var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
        if(operand2::is_legal_immed_8r(var_stack_pos_from_sp))
        {
            //如果此时var_stack_pos_from_sp可以作为指令的一部分，不需要放入寄存器中
            op2=get_operand2(var_stack_pos_from_sp);
        }
        else
        {
            //如果此时var_stack_pos_from_sp需要放入寄存器之后再使用
            if(notify(event(event_type::CHECK_CONST_INT_VALUE_OWN_CPU_REG,(int)var_stack_pos_from_sp)).bool_data)
            {
                //如果此时该var_stack_pos_from_sp的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                op2=operand2((reg_index)notify(event(event_type::GET_CONST_VALUE_S_CPU_REG,(int)var_stack_pos_from_sp)).int_data);
            }
            else
            {
                //如果此时没有寄存器中存放着var_stack_pos_from_sp的值，那么就需要给该值新分配一个寄存器
                const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                //分配完之后还要再进行一次GET_VAR_STACK_POS_FROM_SP，得到新的var_stack_pos_from_sp，因为上面的ALLOCATE_IDLE_REG可能会使得栈顶的位置发送变化
                var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
                //然后把新的var_stack_pos_from_sp值写入寄存器中即可
                event_data=new pair<OAA,reg_index>(OAA((int)var_stack_pos_from_sp),const_reg);
                notify(event(event_type::WRITE_CONST_TO_REG,(void *)event_data));
                //再通知内存管理模块：寄存器被新的stack_offset占用了
                notify(event(event_type::ATTACH_CONST_TO_REG,(void *)event_data));
                delete event_data;
                //最后把得到的寄存器作为operand2的一部分即可
                op2=operand2(const_reg);
            }
        }
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,reg,sp,op2));
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_PUSH_TEMP_VAR_FROM_REG_TO_STACK(struct ic_data * var,reg_index reg)
{
    if(!notify(event(event_type::CHECK_TEMP_VAR_IN_STACK,(void *)var)).bool_data)
    {
        if(notify(event(event_type::IS_CPU_REG,(int)reg)).bool_data)
        {
            push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        else if(notify(event(event_type::IS_VFP_REG,(int)reg)).bool_data)
        {
            push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        notify(event(event_type::PUSH_VAR_TO_STACK,(void *)var));
        notify(event(event_type::DISABLE_ALL_ADDR_REG,nullptr));
    }
}

void Arm_instruction_generator::handle_CALL_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg)
{
    size_t int_r_params_num=0,float_r_params_num=0,r_params_passed_by_stack_bytes=0;
    int pop_stack_size=0;
    reg_index reg,sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    pair<struct ic_data *,reg_index> * event_data;
    set<reg_index> * regs_unaccessible;
    list<struct ic_data * > r_params_passed_by_stack;
    //先把r0-r3和s0-s15中需要保存的进行保存，因为在函数调用的时候不会保证这些寄存器的值不变
    //此时还需要保存如下变量所在的脏值寄存器（因为这些变量可能会在函数调用的过程中被改变）：
    //全局变量
    //数组取元素
    //和上述两类变量有关的变量
    notify(event(event_type::SAVE_REGS_WHEN_CALLING_FUNC,nullptr));
    //把需要入栈的参数从右到左入栈
    //默认每一参数都是4bytes的
    for(auto r_param:*r_params)
    {
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(int_r_params_num>=4)
            {
                r_params_passed_by_stack.push_front(r_param);
            }
            int_r_params_num++;
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num>=16)
            {
                r_params_passed_by_stack.push_front(r_param);
            }
            float_r_params_num++;
        }
    }
    //目前默认每一个变量都是32bits
    r_params_passed_by_stack_bytes=r_params_passed_by_stack.size()*4;
    if(r_params_passed_by_stack_bytes%8!=0)
    {
        notify(event(event_type::PADDING_WHEN_CALL_FUNC,(int)(8-(r_params_passed_by_stack_bytes%8))));
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2(8-(r_params_passed_by_stack_bytes%8))));
    }
    //把需要入栈的参数从右到左入栈
    for(auto r_param:r_params_passed_by_stack)
    {
        notify(event(event_type::START_INSTRUCTION,nullptr));
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)r_param)).int_data;
            push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            reg=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)r_param)).int_data;
            push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        notify(event(event_type::DISABLE_ALL_ADDR_REG,nullptr));
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)r_param));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
    //再把需要存放在寄存器中的参数放入寄存器
    notify(event(event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC,(void *)r_params));
    //在真正执行函数跳转之前还需要对返回值进行检查，检查此时是否有和返回值相关的脏值数据需要先写回内存的
    notify(event(event_type::BEFORE_CALL_FUNC,(void *)return_value));
    //执行函数跳转
    push_instruction(new Arm_cpu_branch_instruction(arm_op::BL,arm_condition::NONE,func_name));
    //如果函数有返回值，通知寄存器管理器：寄存器r0已经被相应的返回值占用了
    //同时也会通知内存管理器，下一步将要把入栈的参数清除，需要其返回要出栈的大小
    event_data=new pair<struct ic_data *,reg_index>(return_value,return_reg);
    pop_stack_size=notify(event(event_type::RET_FROM_CALLED_FUNC,(void *)event_data)).int_data;
    delete event_data;
    //调用者需要把入栈的参数清理掉即可
    if(pop_stack_size!=0)
    {
        //此时需要将存放返回值的寄存器设置为不可获取的
        regs_unaccessible=new set<reg_index>;
        regs_unaccessible->insert(return_reg);
        notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
        delete regs_unaccessible;
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,get_operand2(pop_stack_size)));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
}

void Arm_instruction_generator::handle_CALL_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg,list<reg_index> * r_param_regs)
{
    size_t int_r_params_num=0,float_r_params_num=0,r_params_passed_by_stack_bytes=0;
    int pop_stack_size=0;
    reg_index reg,sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<list<struct ic_data * > *,list<reg_index> *> * event_data_2;
    set<reg_index> * regs_unaccessible;
    list<struct ic_data * > r_params_passed_by_stack;
    //先把参数寄存器中需要保存的进行保存，因为在函数调用的时候不会保证这些寄存器的值不变
    notify(event(event_type::SAVE_REGS_WHEN_CALLING_ABI_FUNC,nullptr));
    //把需要入栈的参数从右到左入栈
    //默认每一参数都是4bytes的
    for(auto r_param:*r_params)
    {
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            if(int_r_params_num>=4)
            {
                r_params_passed_by_stack.push_front(r_param);
            }
            int_r_params_num++;
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num>=16)
            {
                r_params_passed_by_stack.push_front(r_param);
            }
            float_r_params_num++;
        }
    }
    //目前默认每一个变量都是32bits
    r_params_passed_by_stack_bytes=r_params_passed_by_stack.size()*4;
    if(r_params_passed_by_stack_bytes%8!=0)
    {
        notify(event(event_type::PADDING_WHEN_CALL_FUNC,(int)(8-(r_params_passed_by_stack_bytes%8))));
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2(8-(r_params_passed_by_stack_bytes%8))));
    }
    //把需要入栈的参数从右到左入栈
    for(auto r_param:r_params_passed_by_stack)
    {
        notify(event(event_type::START_INSTRUCTION,nullptr));
        if(r_param->get_data_type()==language_data_type::INT || r_param->is_array_var())
        {
            reg=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)r_param)).int_data;
            push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        else if(r_param->get_data_type()==language_data_type::FLOAT)
        {
            reg=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)r_param)).int_data;
            push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
        }
        notify(event(event_type::DISABLE_ALL_ADDR_REG,nullptr));
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)r_params));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
    //再把需要存放在寄存器中的参数放入寄存器
    event_data_2=new pair<list<struct ic_data * > *,list<reg_index> *>(r_params,r_param_regs);
    notify(event(event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_ABI_FUNC,(void *)event_data_2));
    delete event_data_2;
    //在真正执行函数跳转之前还需要对返回值进行检查，检查此时是否有和返回值相关的脏值数据需要先写回内存的
    notify(event(event_type::BEFORE_CALL_FUNC,(void *)return_value));
    //执行函数跳转
    push_instruction(new Arm_cpu_branch_instruction(arm_op::BL,arm_condition::NONE,func_name));
    //如果函数有返回值，通知寄存器管理器：寄存器r0已经被相应的返回值占用了
    //同时也会通知内存管理器，下一步将要把入栈的参数清除，需要其返回要出栈的大小
    event_data_1=new pair<struct ic_data *,reg_index>(return_value,return_reg);
    pop_stack_size=notify(event(event_type::RET_FROM_CALLED_ABI_FUNC,(void *)event_data_1)).int_data;
    delete event_data_1;
    //调用者需要把入栈的参数清理掉即可
    if(pop_stack_size!=0)
    {
        //此时需要将存放返回值的寄存器设置为不可获取的
        regs_unaccessible=new set<reg_index>;
        if(return_value!=nullptr)
        {
            regs_unaccessible->insert(return_reg);
        }
        notify(event(event_type::START_INSTRUCTION,(void *)regs_unaccessible));
        delete regs_unaccessible;
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,get_operand2(pop_stack_size)));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
}

void Arm_instruction_generator::handle_MOVE_DATA_BETWEEN_REGS(reg_index from,reg_index to)
{
    if(from!=to)
    {
        if(notify(event(event_type::IS_CPU_REG,(int)from)).bool_data && notify(event(event_type::IS_CPU_REG,(int)to)).bool_data)
        {
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NONE,to,operand2(from)));
        }
        else
        {
            push_instruction(new Arm_vfp_register_transfer_instruction(arm_op::VMOV,arm_condition::NONE,to,from));
        }
    }
}

void Arm_instruction_generator::handle_ASSIGN_VAR(struct ic_data * from,struct ic_data * to)
{
    reg_index Rm,r0,s0;
    list<struct ic_data * > * r_params;
    pair<struct ic_data *,reg_index> * event_data_1;
    pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * event_data_2;
    OAA const_value;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(from->get_data_type()==to->get_data_type())
    {
        if(from->get_data_type()==language_data_type::INT)
        {
            Rm=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_READING_VAR,(void *)from)).int_data;
        }
        else if(from->get_data_type()==language_data_type::FLOAT)
        {
            Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_READING_VAR,(void *)from)).int_data;
        }
    }
    else
    {
        if(from->get_data_type()==language_data_type::INT && to->get_data_type()==language_data_type::FLOAT)
        {
            if(from->is_const() && !from->is_array_var())
            {
                const_value=OAA((float)(from->get_value().int_data));
                Rm=(reg_index)notify(event(event_type::GET_VFP_REG_FOR_CONST,(void *)(&const_value))).int_data;
            }
            else
            {
                r_params=new list<struct ic_data * >;
                event_data_2=new pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * >;
                event_data_2->first.first.first="__aeabi_i2f";
                r_params->push_back(from);
                event_data_2->first.first.second=r_params;
                event_data_2->first.second.first=nullptr;
                event_data_2->first.second.second=(reg_index)0;
                event_data_2->second=nullptr;
                notify(event(event_type::CALL_ABI_FUNC,(void *)event_data_2));
                delete r_params;
                delete event_data_2;
                r0=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
                notify(event(event_type::UNATTACH_REG_S_ALL_DATA,(int)r0));
                Rm=r0;
            }
        }
        else if(from->get_data_type()==language_data_type::FLOAT && to->get_data_type()==language_data_type::INT)
        {
            if(from->is_const() && !from->is_array_var())
            {
                const_value=OAA((int)(from->get_value().float_data));
                Rm=(reg_index)notify(event(event_type::GET_CPU_REG_FOR_CONST,(void *)(&const_value))).int_data;
                event_data_1=new pair<struct ic_data *,reg_index>(to,Rm);
                notify(event(event_type::ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY,(void *)event_data_1));
                delete event_data_1;
            }
            else
            {
                r_params=new list<struct ic_data * >;
                event_data_2=new pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * >;
                event_data_2->first.first.first="__aeabi_f2iz";
                r_params->push_back(from);
                event_data_2->first.first.second=r_params;
                event_data_2->first.second.first=nullptr;
                event_data_2->first.second.second=(reg_index)0;
                event_data_2->second=new list<reg_index>;
                event_data_2->second->push_back((reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data);
                notify(event(event_type::CALL_ABI_FUNC,(void *)event_data_2));
                delete r_params;
                delete event_data_2->second;
                delete event_data_2;
                r0=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
                notify(event(event_type::UNATTACH_REG_S_ALL_DATA,(int)r0));
                Rm=r0;
            }
        }
    }
    event_data_1=new pair<struct ic_data *,reg_index>(to,Rm);
    notify(event(event_type::ATTACH_VAR_VALUE_TO_REG_THEN_SET_DIRTY,(void *)event_data_1));
    delete event_data_1;
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_POP_STACK(size_t pop_size)
{
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    if(pop_size>0)
    {
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,get_operand2((int)pop_size)));
    }
}

void Arm_instruction_generator::handle_START_FUNC(struct ic_func * func)
{
    arm_flow_graph_.add_arm_asm_to_func(nullptr,false,func);
}

void Arm_instruction_generator::handle_START_BASIC_BLOCK(struct ic_basic_block * basic_block)
{
    arm_flow_graph_.add_arm_asm_to_func(nullptr,true);
    is_current_basic_block_starting_=true;
}

/*
事件处理函数(由中介者进行调用)

Parameters
----------
event:要处理的事件信息

Return
------
处理完事件之后返回响应信息
*/
struct event Arm_instruction_generator::handler(struct event event)
{
    struct event res;
    switch(event.type)
    {
        case event_type::WRITE_CONST_TO_REG:
            handle_WRITE_CONST_TO_REG(((pair<OAA,reg_index> *)event.pointer_data)->first,((pair<OAA,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::STORE_VAR_TO_MEM:
            handle_STORE_VAR_TO_MEM(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::LOAD_VAR_TO_REG:
            handle_LOAD_VAR_TO_REG(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::WRITE_ADDR_TO_REG:
            handle_WRITE_ADDR_TO_REG(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::CALL_FUNC:
            handle_CALL_FUNC(((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.second,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.second);
            break;
        case event_type::CALL_ABI_FUNC:
            handle_CALL_ABI_FUNC(((pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * )event.pointer_data)->first.first.first,((pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * )event.pointer_data)->first.first.second,((pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * )event.pointer_data)->first.second.first,((pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * )event.pointer_data)->first.second.second,((pair<pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >,list<reg_index> * > * )event.pointer_data)->second);
            break;
        case event_type::MOVE_DATA_BETWEEN_REGS:
            handle_MOVE_DATA_BETWEEN_REGS(((pair<reg_index,reg_index> *)event.pointer_data)->first,((pair<reg_index,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::ASSIGN_VAR:
            handle_ASSIGN_VAR(((pair<struct ic_data *,struct ic_data * > *)event.pointer_data)->first,((pair<struct ic_data *,struct ic_data * > *)event.pointer_data)->second);
            break;
        case event_type::POP_STACK:
            handle_POP_STACK((size_t)event.int_data);
            break;
        case event_type::START_FUNC:
            handle_START_FUNC((struct ic_func *)event.pointer_data);
            break;
        case event_type::START_BASIC_BLOCK:
            handle_START_BASIC_BLOCK((struct ic_basic_block *)event.pointer_data);
            break;
        default:
            break;
    }
    return res;
}