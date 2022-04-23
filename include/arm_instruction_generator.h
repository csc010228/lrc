/*
 *
 *  arm_instruction_generator.h
 *  arm汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#ifndef __ARM_INSTRUCTION_GENERATOR_H
#define __ARM_INSTRUCTION_GENERATOR_H

#include<stdarg.h>
#include "instruction_generator.h"

//arm的CPU汇编指令操作码
enum class arm_op
{
    /*================Arm的cpu指令================*/
    //Branch instruction
    B,
    BL,
    BLX,
    BX,
    //Data process instruction
    ADD,
    SUB,
    RSB,
    ADC,
    SBC,
    RSC,
    AND,
    ORR,
    EOR,
    BIC,
    CLZ,
    CMP,
    CMN,
    MOV,
    MVN,
    MOVW,
    MOVT,
    MUL,
    MLA,
    TST,
    TEQ,
    //Multiple register load and store instruction
    LDM,
    STM,
    PUSH,
    POP,
    //Single resgiter load and store instruction
    LDR,
    STR,
    /*============================================*/
    /*================Arm的vfp指令================*/
    //Register transfer
    VMOV,
    //Data process instruction
    VABS,
    VCPY,
    VNEG,
    VSQRT,
    VADD,
    VSUB,
    VDIV,
    VMLA,
    VNMLA,
    VMLS,
    VNMLS,
    VMUL,
    VNMUL,
    VCMP,
    //Multiple register load and store instruction
    VLDM,
    VSTM,
    VPOP,
    VPUSH,
    //Single resgiter load and store instruction
    VLDR,
    VSTR,
    /*============================================*/
};

//arm汇编的数据类型
enum class arm_data_type
{
    B,
    SB,
    H,
    SH,
    W,
    D
};

//arm条件执行的条件
enum class arm_condition
{
    NONE,
    EQ,
    NE,
    CS,
    HS,
    CC,
    LO,
    MI,
    PL,
    VS,
    VC,
    HI,
    LS,
    GE,
    LT,
    GT,
    LE,
    AL
};

//Operand2的类型
enum class operand2_type
{
    IMMED_8R,
    RM_SHIFT
};

//Operand2中的移位操作符
enum class operand2_shift_op
{
    NONE,
    ASR_N,
    LSL_N,
    LSR_N,
    ROR_N,
    RRX,
    ASR_RS,
    LSL_RS,
    LSR_RS,
    ROR_RS
};

//寄存器shitf类型的Operand2
struct opernad2_Rm_shift
{
    opernad2_Rm_shift(reg_index Rm):Rm(Rm),shift_op(operand2_shift_op::NONE)
    {

    };

    opernad2_Rm_shift(reg_index Rm,enum operand2_shift_op shift_op,int n):Rm(Rm),shift_op(shift_op),n(n)
    {

    };

    opernad2_Rm_shift(reg_index Rm,enum operand2_shift_op shift_op,reg_index Rs):Rm(Rm),shift_op(shift_op),Rs(Rs)
    {

    };

    reg_index Rm;
    enum operand2_shift_op shift_op;
    union
    {
        int n;
        reg_index Rs;
    };
};

//Data process指令特有的操作数Operand2
struct operand2
{
    operand2():type(operand2_type::IMMED_8R),immed_8r(0)
    {

    };

    operand2(int immed_8r):type(operand2_type::IMMED_8R),immed_8r(immed_8r)
    {

    };

    operand2(reg_index Rm):type(operand2_type::RM_SHIFT),Rm_shift(opernad2_Rm_shift(Rm))
    {

    };

    operand2(reg_index Rm,enum operand2_shift_op shift_op,int n):type(operand2_type::RM_SHIFT),Rm_shift(opernad2_Rm_shift(Rm,shift_op,n))
    {

    };

    operand2(reg_index Rm,enum operand2_shift_op shift_op,reg_index Rs):type(operand2_type::RM_SHIFT),Rm_shift(opernad2_Rm_shift(Rm,shift_op,Rs))
    {

    };

    //查看传入的immed_8r是否合法
    static bool is_legal_immed_8r(int immed_8r)
    {
        unsigned int mask=255;
        for(size_t i=0;i<24;i++)
        {
            if((immed_8r & mask)==immed_8r)
            {
                return true;
            }
            mask=mask<<1;
        }
        for(size_t i=0;i<8;i++)
        {
            if((immed_8r & mask)==immed_8r)
            {
                return true;
            }
            mask=((mask<<1) | 0x00000000000000000000000000000001);
        }
        return false;
    };

    //转换成字符串
    string to_string() const;

    enum operand2_type type;
    union
    {
        int immed_8r;
        struct opernad2_Rm_shift Rm_shift;
    };
};

//16位立即数的类型
enum class immed_16r_type
{
    IMMED,
    VAR_ADDR
};

//变量取地址是高16位还是低16位
enum class immed_16r_var_addr_type
{
    LOWER16,
    UPPER16
};

//16位立即数
struct immed_16r
{
    immed_16r(int immed_data):type(immed_16r_type::IMMED),immed_data(immed_data)
    {

    };

    immed_16r(struct ic_data * var,enum immed_16r_var_addr_type addr_type):type(immed_16r_type::VAR_ADDR)
    {
        var_addr.var=var;
        var_addr.addr_type=addr_type;
    };

    //转换成字符串
    string to_string() const;

    enum immed_16r_type type;
    union
    {
        int immed_data;                         //立即数
        struct                                  //变量的地址
        {
            struct ic_data * var;               //取地址的变量
            enum immed_16r_var_addr_type addr_type;     //取地址的信息（是取变量的高16位地址还是低16位地址）
        } var_addr;
    };
};

//Multiple register load and store指令的address mode
enum class address_mode
{
    NONE,
    IA,
    IB,
    DA,
    DB
};

//FlexOffset的类型
enum class flexoffset_type
{
    EXPR,
    RM_SHIFT
};

//flexoffset中的移位操作
enum class flexoffset_shift_op
{
    NONE,
    ASR_N,
    LSL_N,
    LSR_N,
    ROR_N,
    RRX
};

//FlexOffset中的寄存器移位
struct flexoffset_shift
{
    flexoffset_shift(bool reverse,enum flexoffset_shift_op shift_op,int n):reverse(reverse),shift_op(shift_op),n(n)
    {
        
    };

    flexoffset_shift(bool reverse,enum flexoffset_shift_op shift_op,reg_index Rm):reverse(reverse),shift_op(shift_op),Rm(Rm)
    {
        
    };

    bool reverse;
    enum flexoffset_shift_op shift_op;
    int n;
    reg_index Rm;
};

//flexoffset中Rm寄存器的移位
struct Rm_shift 
{
    Rm_shift(bool reverse,reg_index Rm,enum flexoffset_shift_op shift_op,int n):reverse(reverse),Rm(Rm),shift_op(shift_op),n(n)
    {
                
    };

    bool reverse;
    enum flexoffset_shift_op shift_op;
    int n;
    reg_index Rm;
};

//Single resgiter load and store指令特有的操作数FlexOffset
struct flexoffset
{
    flexoffset():type(flexoffset_type::EXPR),expr(0)
    {

    }

    flexoffset(int expr):type(flexoffset_type::EXPR),expr(expr)
    {
        
    };

    flexoffset(bool reverse,reg_index Rm,enum flexoffset_shift_op shift_op,int n):type(flexoffset_type::RM_SHIFT),Rm_shift(reverse,Rm,shift_op,n)
    {

    };

    //转换成字符串
    string to_string() const;

    enum flexoffset_type type;
    union
    {
        int expr;
        struct Rm_shift Rm_shift;
    };

    //查看是否是合法的expr
    static bool is_legal_expr(int expr)
    {
        return (expr>=-4095 && expr <=4095);
        //return (expr>=-1020 && expr <=1020);
    };
    
};

//single register load and store的类型
enum class arm_single_register_load_and_store_type
{
    ZERO_OFFSET,
    PRE_INDEXED_OFFSET,
    PROGRAM_RELATIVE,
    POST_INDEXED_OFFSET
};

//arm寄存器组
struct arm_registers
{
    arm_registers()
    {

    };

    arm_registers(list<reg_index> registers):registers_(registers)
    {
        ;
    }

    arm_registers(size_t num,...)
    {
        va_list argptr;
		va_start(argptr,num);
		for(size_t i=0;i<num;i++)
		{
			registers_.push_back(va_arg(argptr,reg_index));
		}
		va_end(argptr);
    };

    //转换成字符串
    string to_string() const;

    //寄存器组
    list<reg_index> registers_;
};

//arm的directive的种类
enum class arm_directive_type
{
    ALIGN,
    ARCH,
    ARM,
    ASCII,
    ASCIZ,
    BSS,
    CODE,
    COMM,
    DATA,
    FORCE_THUMB,
    FPU,
    GLOBAL,
    IDENT,
    SIZE,
    SYNTAX,
    SECTION,
    SPACE,
    TEXT,
    THUMB,
    THUMB_FUNC,
    TYPE,
    WORD,
    EABI_ATTRIBUTE,
    FILE
};

//ARM的pseudo instruction的操作符类型
enum class arm_pseudo_op
{
    ADR,
    ADRL,
    LDR,
    VLDR,
    NOP,
    IT
};

//arm汇编文件的每一行的类型
enum class arm_asm_file_line_type
{
    DIRECTIVE,
    INSTRUCTION,
    PSEUDO_INSTRUCTION,
    LABEL
};

//arm汇编文件的一行
class Arm_asm_file_line
{
protected:
    //该行的类型
    enum arm_asm_file_line_type type_;

public:
    Arm_asm_file_line()
    {

    };
    
    Arm_asm_file_line(enum arm_asm_file_line_type type):type_(type)
    {

    };

    virtual ~Arm_asm_file_line()
    {

    };

    //转换成字符串
    virtual string to_string() const =0;
};

//arm的directive
class Arm_directive:public Arm_asm_file_line
{
protected:
    //directive的类型
    enum arm_directive_type directive_type_;

    //directive的数据
    list<string> data_;

public:
    Arm_directive(enum arm_directive_type directive_type,list<string> data):Arm_asm_file_line(arm_asm_file_line_type::DIRECTIVE),directive_type_(directive_type),data_(data)
    {

    };

    ~Arm_directive()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm的instruction
class Arm_instruction:public Arm_asm_file_line
{
protected:
    //操作码
    enum arm_op op_;

    //条件执行的条件
    enum arm_condition cond_;

    //目的寄存器
    struct arm_registers destination_registers_;

    //源寄存器
    struct arm_registers source_registers_;

public:
    Arm_instruction(enum arm_op op,enum arm_condition cond,struct arm_registers destination_registers,struct arm_registers source_registers):Arm_asm_file_line(arm_asm_file_line_type::INSTRUCTION),op_(op),cond_(cond),destination_registers_(destination_registers),source_registers_(source_registers)
    {

    };

    virtual ~Arm_instruction()
    {

    };
};

//arm浮点数的精度
enum class precision
{
    S,
    D
};

//arm的pseduo instruction
class Arm_pseudo_instruction:public Arm_asm_file_line
{
protected:
    //操作码
    enum arm_pseudo_op op_;

    //条件执行的条件
    enum arm_condition cond_;

    //浮点精度
    enum precision precision_;

    //目的寄存器
    reg_index reg_;

    //表达式
    string expr_;

    //条件
    list<string> conds_;

public:
    //NOP的构造函数
    Arm_pseudo_instruction():Arm_asm_file_line(arm_asm_file_line_type::PSEUDO_INSTRUCTION),op_(arm_pseudo_op::NOP),cond_(arm_condition::NONE),reg_(0),expr_("")
    {

    };

    //ADR和ADRL的构造函数
    Arm_pseudo_instruction(enum arm_pseudo_op op,enum arm_condition cond,reg_index reg,string expr):Arm_asm_file_line(arm_asm_file_line_type::PSEUDO_INSTRUCTION),op_(op),cond_(cond),reg_(reg),expr_(expr)
    {

    };

    //LDR的构造函数
    Arm_pseudo_instruction(enum arm_condition cond,reg_index reg,string expr):Arm_asm_file_line(arm_asm_file_line_type::PSEUDO_INSTRUCTION),op_(arm_pseudo_op::LDR),cond_(cond),reg_(reg),expr_(expr)
    {

    };

    //VLDR的构造函数
    Arm_pseudo_instruction(enum arm_condition cond,enum precision precision,reg_index reg,string expr):Arm_asm_file_line(arm_asm_file_line_type::PSEUDO_INSTRUCTION),op_(arm_pseudo_op::VLDR),cond_(cond),precision_(precision),reg_(reg),expr_(expr)
    {

    };

    //IT的构造函数
    Arm_pseudo_instruction(string cond1,string cond2="",string cond3="",string cond4=""):Arm_asm_file_line(arm_asm_file_line_type::PSEUDO_INSTRUCTION),op_(arm_pseudo_op::IT),cond_(arm_condition::NONE)
    {
        conds_.push_back(cond1);
        conds_.push_back(cond2);
        conds_.push_back(cond3);
        conds_.push_back(cond4);
    };

    ~Arm_pseudo_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};


//arm的label
class Arm_label:public Arm_asm_file_line
{
public:
    //label的名字
    string name_;

    Arm_label()
    {

    };

    Arm_label(string name):Arm_asm_file_line(arm_asm_file_line_type::LABEL),name_(name)
    {

    };

    ~Arm_label()
    {

    };

    //转换成字符串
    string to_string(bool is_define) const;

    //转换成字符串
    string to_string() const;
};

//arm的cpu指令
class Arm_cpu_instruction:public Arm_instruction
{
public:
    Arm_cpu_instruction(enum arm_op op,enum arm_condition cond,struct arm_registers destination_registers,struct arm_registers source_registers):Arm_instruction(op,cond,destination_registers,source_registers)
    {

    };

    ~Arm_cpu_instruction()
    {

    };
};

//arm的vfp指令
class Arm_vfp_instruction:public Arm_instruction
{
public:
    Arm_vfp_instruction(enum arm_op op,enum arm_condition cond,struct arm_registers destination_registers,struct arm_registers source_registers):Arm_instruction(op,cond,destination_registers,source_registers)
    {

    };

    ~Arm_vfp_instruction()
    {

    };
};

//arm的branch类型的instruction
class Arm_cpu_branch_instruction:public Arm_cpu_instruction
{
protected:
    //要跳转的label
    Arm_label label_;

public:
    Arm_cpu_branch_instruction(enum arm_op op,enum arm_condition cond,string label_name):Arm_cpu_instruction(op,cond,arm_registers(),arm_registers())
    {
        label_=Arm_label(label_name);
    };

    Arm_cpu_branch_instruction(enum arm_op op,enum arm_condition cond,reg_index Rm):Arm_cpu_instruction(op,arm_condition::NONE,arm_registers(),arm_registers(1,Rm))
    {

    };

    ~Arm_cpu_branch_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

enum class arm_data_process_instruction_data_type
{
    NONE,
    OPERNAD2,
    IMMED_16R,
};

//arm的data process类型的instruction
class Arm_cpu_data_process_instruction:public Arm_cpu_instruction
{
protected:
    //是否允许执行结果影响标志位
    bool update_flags_;

    //操作数是opernad2还是16位立即数
    enum arm_data_process_instruction_data_type data_type_;

    union
    {
        //Data process指令特有的操作数Operand2
        struct operand2 operand2_;

        //Data process指令特有的操作数16位立即数
        struct immed_16r immed_16r_;
    };

public:
    //ADD,SUB,RSB,ADC,SBC,RSC,AND,ORR,EOR,BIC的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,bool update_flags,reg_index Rd,reg_index Rn,struct operand2 operand2):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(1,Rn)),update_flags_(update_flags),operand2_(operand2),data_type_(arm_data_process_instruction_data_type::OPERNAD2)
    {

    };

    //CLZ的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,reg_index Rd,reg_index Rm):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(1,Rm)),update_flags_(false),data_type_(arm_data_process_instruction_data_type::NONE)
    {

    };

    //CMP,CMN,TST,TEQ的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,reg_index Rn,struct operand2 operand2):Arm_cpu_instruction(op,cond,arm_registers(),arm_registers(1,Rn)),update_flags_(false),operand2_(operand2),data_type_(arm_data_process_instruction_data_type::OPERNAD2)
    {

    };

    //MOV,MVN的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,bool update_flags,reg_index Rd,struct operand2 operand2):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers()),update_flags_(update_flags),operand2_(operand2),data_type_(arm_data_process_instruction_data_type::OPERNAD2)
    {

    };

    //MOVW,MOVT的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,reg_index Rd,struct immed_16r immed_16r):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers()),immed_16r_(immed_16r),data_type_(arm_data_process_instruction_data_type::IMMED_16R)
    {

    };

    //MUL的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,bool update_flags,reg_index Rd,reg_index Rm,reg_index Rs):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(2,Rm,Rs)),update_flags_(update_flags),data_type_(arm_data_process_instruction_data_type::NONE)
    {

    };

    //MLA的构造函数
    Arm_cpu_data_process_instruction(enum arm_op op,enum arm_condition cond,bool update_flags,reg_index Rd,reg_index Rm,reg_index Rs,reg_index Rn):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(3,Rm,Rs,Rn)),update_flags_(update_flags),data_type_(arm_data_process_instruction_data_type::NONE)
    {

    };

    ~Arm_cpu_data_process_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm的multiple registers load and store类型的instruction
class Arm_cpu_multiple_registers_load_and_store_instruction:public Arm_cpu_instruction
{
protected:
    //内存模式
    enum address_mode address_mode_;

    //是否将结果写回源寄存器Rn
    bool write_back_Rn_;

public:
    //LDM和STM的构造函数
    Arm_cpu_multiple_registers_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum address_mode address_mode,reg_index Rn,bool write_back_Rn,struct arm_registers reg_list):Arm_cpu_instruction(op,cond,reg_list,arm_registers(1,Rn)),address_mode_(address_mode),write_back_Rn_(write_back_Rn)
    {

    };

    //PSUH和POP的构造函数
    Arm_cpu_multiple_registers_load_and_store_instruction(enum arm_op op,enum arm_condition cond,struct arm_registers reg_list):Arm_cpu_instruction(op,cond,reg_list,arm_registers()),address_mode_(address_mode::NONE),write_back_Rn_(false)
    {

    };

    ~Arm_cpu_multiple_registers_load_and_store_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm的single register load and store类型的instruction
class Arm_cpu_single_register_load_and_store_instruction:public Arm_cpu_instruction
{
protected:
    //single register load and store的类型
    enum arm_single_register_load_and_store_type single_register_load_and_store_type_;

    //要load和store的数据类型
    enum arm_data_type data_type_;

    union
    {
        //是否将结果写回Rn
        bool write_back_Rn_;

        //要load和store的label
        string label_;
    };

    //Single resgiter load and store指令特有的操作数FlexOffset
    struct flexoffset flexoffset_;

public:
    //zero offset类型的single register load and store指令
    Arm_cpu_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum arm_data_type data_type,reg_index Rd,reg_index Rn):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(1,Rn)),data_type_(data_type),single_register_load_and_store_type_(arm_single_register_load_and_store_type::ZERO_OFFSET)
    {

    };

    //pre-indexed offset类型的single register load and store指令
    Arm_cpu_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum arm_data_type data_type,reg_index Rd,reg_index Rn,struct flexoffset flexoffset,bool write_back_Rn):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(1,Rn)),data_type_(data_type),single_register_load_and_store_type_(arm_single_register_load_and_store_type::PRE_INDEXED_OFFSET),flexoffset_(flexoffset),write_back_Rn_(write_back_Rn)
    {

    };

    //program-relative类型的single register load and store指令
    Arm_cpu_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum arm_data_type data_type,reg_index Rd,string label):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers()),data_type_(data_type),single_register_load_and_store_type_(arm_single_register_load_and_store_type::PROGRAM_RELATIVE),label_(label)
    {

    };

    //post-indexed offset类型的single register load and store指令
    Arm_cpu_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum arm_data_type data_type,reg_index Rd,reg_index Rn,struct flexoffset flexoffset):Arm_cpu_instruction(op,cond,arm_registers(1,Rd),arm_registers(1,Rn)),data_type_(data_type),single_register_load_and_store_type_(arm_single_register_load_and_store_type::POST_INDEXED_OFFSET),flexoffset_(flexoffset)
    {

    };

    ~Arm_cpu_single_register_load_and_store_instruction()
    {

    };

    //转换成字符串
    string to_string() const;

};

//arm的寄存器数据转移指令
class Arm_vfp_register_transfer_instruction:public Arm_vfp_instruction
{
private:
    //立即数
    float imm_;

public:
    //VMOV
    Arm_vfp_register_transfer_instruction(enum arm_condition cond,reg_index Rd,reg_index Rn):Arm_vfp_instruction(arm_op::VMOV,cond,arm_registers(1,Rd),arm_registers(1,Rn))
    {

    };

    //VMOV
    Arm_vfp_register_transfer_instruction(enum arm_condition cond,reg_index Rd,float imm):Arm_vfp_instruction(arm_op::VMOV,cond,arm_registers(1,Rd),arm_registers(0)),imm_(imm)
    {

    };

    //VMOV
    Arm_vfp_register_transfer_instruction(enum arm_condition cond,reg_index Rd1,reg_index Rd2,reg_index Rn,reg_index Rm):Arm_vfp_instruction(arm_op::VMOV,cond,arm_registers(2,Rd1,Rd2),arm_registers(2,Rn,Rm))
    {

    };

    ~Arm_vfp_register_transfer_instruction()
    {

    };
    
    //转换成字符串
    string to_string() const;

};

//arm的vfp数据处理指令
class Arm_vfp_data_process_instruction:public Arm_vfp_instruction
{
private:
    //浮点数精度
    enum precision precision_;

    //是否有E
    bool E_;

    //是否和0比较
    bool compare_with_zero_;

public:
    //VABS,VCPY,VNEG,VSQRT
    Arm_vfp_data_process_instruction(enum arm_op op,enum arm_condition cond,enum precision precision,reg_index Fd,reg_index Fm):Arm_vfp_instruction(op,cond,arm_registers(1,Fd),arm_registers(1,Fm)),precision_(precision),E_(false),compare_with_zero_(false)
    {

    };

    //VADD,VSUB,VDIV,VMLA,VNMLA,VMLS,VNMLS,VMUL,VNMUL
    Arm_vfp_data_process_instruction(enum arm_op op,enum arm_condition cond,enum precision precision,reg_index Fd,reg_index Fn,reg_index Fm):Arm_vfp_instruction(op,cond,arm_registers(1,Fd),arm_registers(2,Fn,Fm)),precision_(precision),E_(false),compare_with_zero_(false)
    {

    };

    //VCMP
    Arm_vfp_data_process_instruction(bool E,enum arm_condition cond,enum precision precision,reg_index Fd,reg_index Fm):Arm_vfp_instruction(arm_op::VCMP,cond,arm_registers(1,Fd),arm_registers(1,Fm)),precision_(precision),E_(E),compare_with_zero_(false)
    {

    };

    //VCMP
    Arm_vfp_data_process_instruction(bool E,enum arm_condition cond,enum precision precision,reg_index Fd):Arm_vfp_instruction(arm_op::VCMP,cond,arm_registers(1,Fd),arm_registers(0)),precision_(precision),E_(E),compare_with_zero_(true)
    {

    };

    ~Arm_vfp_data_process_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm的vfp多寄存器load和store指令
class Arm_vfp_multiple_registers_load_and_store_instruction:public Arm_vfp_instruction
{
protected:
    //内存模式
    enum address_mode address_mode_;

    //是否将结果写回源寄存器Rn
    bool write_back_Rn_;

public:
    //VLDM,VSTM
    Arm_vfp_multiple_registers_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum address_mode address_mode,reg_index Rn,bool write_back_Rn,struct arm_registers reg_list):Arm_vfp_instruction(op,cond,reg_list,arm_registers(1,Rn)),address_mode_(address_mode),write_back_Rn_(write_back_Rn)
    {

    };

    //VPSUH,VPOP
    Arm_vfp_multiple_registers_load_and_store_instruction(enum arm_op op,enum arm_condition cond,struct arm_registers reg_list):Arm_vfp_instruction(op,cond,reg_list,arm_registers()),address_mode_(address_mode::NONE),write_back_Rn_(false)
    {

    };

    ~Arm_vfp_multiple_registers_load_and_store_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm的vfp单寄存器load和store指令
class Arm_vfp_single_register_load_and_store_instruction:public Arm_vfp_instruction
{
private:
    //浮点数精度
    enum precision precision_;

    //single register load and store的类型
    enum arm_single_register_load_and_store_type single_register_load_and_store_type_;

    union
    {
        //是否将结果写回Rn
        struct flexoffset flexoffset_;

        //要load和store的label
        string label_;
    };

public:
    //zero offset
    Arm_vfp_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum precision precision,reg_index Fd,reg_index Rd):Arm_vfp_instruction(op,cond,arm_registers(1,Fd),arm_registers(1,Rd)),single_register_load_and_store_type_(arm_single_register_load_and_store_type::ZERO_OFFSET),precision_(precision)
    {

    };

    //pre-indexed pffset
    Arm_vfp_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum precision precision,reg_index Fd,reg_index Rd,struct flexoffset flexoffset):Arm_vfp_instruction(op,cond,arm_registers(1,Fd),arm_registers(1,Rd)),single_register_load_and_store_type_(arm_single_register_load_and_store_type::PRE_INDEXED_OFFSET),flexoffset_(flexoffset),precision_(precision)
    {

    };

    //program-relative
    Arm_vfp_single_register_load_and_store_instruction(enum arm_op op,enum arm_condition cond,enum precision precision,reg_index Fd,string label):Arm_vfp_instruction(op,cond,arm_registers(1,Fd),arm_registers(0)),single_register_load_and_store_type_(arm_single_register_load_and_store_type::PROGRAM_RELATIVE),label_(label),precision_(precision)
    {

    };

    ~Arm_vfp_single_register_load_and_store_instruction()
    {

    };

    //转换成字符串
    string to_string() const;
};

//arm流图中的基本块
struct arm_basic_block
{
    arm_basic_block()
    {
        sequential_next=nullptr;
        jump_next=nullptr;
    };

    ~arm_basic_block()
    {
        for(auto i:arm_sequence)
        {
            delete i;
        }
    };

    //往汇编函数流图中添加一条arm汇编
    void add_arm_asm(Arm_asm_file_line * arm_asm)
    {
        arm_sequence.push_back(arm_asm);
    };

    //转换成字符串
    list<string> to_string()
    {
        list<string> res;
        for(auto i:arm_sequence)
        {
            res.push_back(i->to_string());
        }
        return res;
    };

    list<Arm_asm_file_line * > arm_sequence;
    struct arm_basic_block * sequential_next,* jump_next;
};

//arm汇编函数的流图
struct arm_func_flow_graph
{
    arm_func_flow_graph(struct ic_func * function):function(function)
    {
    };

    ~arm_func_flow_graph()
    {
        for(auto i:basic_blocks)
        {
            delete i;
        }
    };

    //往汇编函数流图中添加一条arm汇编
    void add_arm_asm(Arm_asm_file_line * arm_asm,bool new_basic_block)
    {
        static struct arm_basic_block * current_arm_basic_block=nullptr;
        if(new_basic_block)
        {
            current_arm_basic_block=new struct arm_basic_block;
            basic_blocks.push_back(current_arm_basic_block);
        }
        if(arm_asm)
        {
            current_arm_basic_block->add_arm_asm(arm_asm);
        }
    };

    //转换成字符串
    list<string> to_string()
    {
        string func_name=function->name;
        list<string> res;
        res.push_back("\t.text");
        res.push_back("\t.align 1");
        res.push_back("\t.global	__aeabi_idiv");
        res.push_back("\t.global	__aeabi_idivmod");
        res.push_back("\t.global "+func_name);
        res.push_back("\t.syntax unified");
        res.push_back("\t.thumb");
        res.push_back("\t.thumb_func");
        res.push_back("\t.fpu vfp");
        res.push_back("\t.type	"+func_name+", %function");
        res.push_back(func_name+":");
        for(auto i:basic_blocks)
        {
            res.splice(res.end(),i->to_string());
        }
        res.push_back("\t.size	"+func_name+", .-"+func_name);
        return res;
    };

    //函数
    struct ic_func * function;
    //函数流图中的所有基本块序列，顺序就是中间代码的书写顺序
    list<struct arm_basic_block * > basic_blocks;
};

//arm汇编的流图
struct arm_flow_graph
{
    arm_flow_graph()
    {
        
    };

    //把一条arm汇编语句插入到汇编流图中
    //参数arm_asm时要插入的汇编语句
    //参数new_func如果不是nullptr，就表示要新建一个函数，将arm_asm插入到这个函数中
    //参数new_basic_block如果不是false，就表示要新建一个基本块，将arm_asm插入到这个基本块中
    void add_arm_asm_to_current_func(Arm_asm_file_line * arm_asm,struct ic_func * new_func,bool new_basic_block)
    {
        static struct arm_func_flow_graph * current_arm_func_flow_graph=nullptr;
        if(new_func!=nullptr)
        {
            current_arm_func_flow_graph=new struct arm_func_flow_graph(new_func);
            func_flow_graphs.insert(current_arm_func_flow_graph);
        }
        if(current_arm_func_flow_graph)
        {
            current_arm_func_flow_graph->add_arm_asm(arm_asm,new_basic_block);
        }
    };

    void add_arm_asm_to_global(Arm_asm_file_line * arm_asm)
    {
        global_basic_block.add_arm_asm(arm_asm);
    };

    ~arm_flow_graph()
    {
        for(auto i:func_flow_graphs)
        {
            delete i;
        }
    };

    //转换成字符串
    list<string> to_string()
    {
        list<string> res,tmp;
        res.push_back("\t.arch armv7");
        res.push_back("\t.eabi_attribute 28, 1");
	    res.push_back("\t.eabi_attribute 20, 1");
	    res.push_back("\t.eabi_attribute 21, 1");
	    res.push_back("\t.eabi_attribute 23, 3");
	    res.push_back("\t.eabi_attribute 24, 1");
	    res.push_back("\t.eabi_attribute 25, 1");
	    res.push_back("\t.eabi_attribute 26, 2");
	    res.push_back("\t.eabi_attribute 30, 6");
	    res.push_back("\t.eabi_attribute 34, 1");
	    res.push_back("\t.eabi_attribute 18, 4");
        //res.push_back("\t.file	\"lrc_test.sy\"");
        for(auto i:func_flow_graphs)
        {
            tmp=i->to_string();
            res.splice(res.end(),tmp);
        }
        tmp=global_basic_block.to_string();
        res.splice(res.end(),tmp);
        res.push_back("\t.ident	\"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0\"");
	    res.push_back("\t.section	.note.GNU-stack,\"\",%progbits");
        return res;
    };

    //所有函数的流图，这些流图之间相互独立
    set<struct arm_func_flow_graph * > func_flow_graphs;
    //全局定义域流图（里面的语句只能是变量定义语句）
    struct arm_basic_block global_basic_block;
};

//arm汇编代码生成器
class Arm_instruction_generator:public Instruction_generator
{

protected:
    //arm汇编
    list<Arm_asm_file_line * > arm_asm_codes_;

    //arm汇编的流图
    struct arm_flow_graph arm_flow_graph_;

    //生成汇编文件序列,并将其赋值给父类Instruction_generator的asm_codes_成员
    void generate_asm_codes();

    //生成arm汇编文件的流图形式，并将其赋值给arm_flow_graph_成员
    void generate_arm_flow_graph();

    //根据一条中间代码生成最终的arm汇编代码
    void ic_to_arm_asm(struct quaternion intermediate_code);
    //获得一个operand2
    struct operand2 get_operand2(struct ic_data * ic_data,enum operand2_shift_op shift_op=operand2_shift_op::NONE,unsigned int n=0);
    struct operand2 get_operand2(int const_int_value);
    //获取一个flexoffset
    struct flexoffset get_flexoffset(int offset);
    struct flexoffset get_flexoffset(struct ic_data * ic_data,enum flexoffset_shift_op shift_op=flexoffset_shift_op::NONE,unsigned int shift_num=0,bool is_negative=false);
    //根据一条ASSIGN中间代码生成最终的arm汇编代码
    void assign_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result);
    //根据一条ADD中间代码生成最终的arm汇编代码
    void add_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条SUB中间代码生成最终的arm汇编代码
    void sub_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条MUL中间代码生成最终的arm汇编代码
    void mul_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条DIV中间代码生成最终的arm汇编代码
    void div_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条MOD中间代码生成最终的arm汇编代码
    void mod_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条L_SHIFT中间代码生成最终的arm汇编代码
    void l_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条R_SHIFT中间代码生成最终的arm汇编代码
    void r_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条NOT中间代码生成最终的arm汇编代码
    void not_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * result);
    //根据一条AND中间代码生成最终的arm汇编代码
    void and_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条OR中间代码生成最终的arm汇编代码
    void or_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条XOR中间代码生成最终的arm汇编代码
    void xor_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条EQ中间代码生成最终的arm汇编代码
    void eq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条UEQ中间代码生成最终的arm汇编代码
    void ueq_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条GT中间代码生成最终的arm汇编代码
    void gt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条LT中间代码生成最终的arm汇编代码
    void lt_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条GE中间代码生成最终的arm汇编代码
    void ge_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条LE中间代码生成最终的arm汇编代码
    void le_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result);
    //根据一条JMP中间代码生成最终的arm汇编代码
    void jmp_ic_to_arm_asm(struct ic_label * result);
    //根据一条IF_JMP中间代码生成最终的arm汇编代码
    void if_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result);
    //根据一条IF_NOT_JMP中间代码生成最终的arm汇编代码
    void if_not_jmp_ic_to_arm_asm(struct ic_data * arg1,struct ic_label * result);
    //根据一条VAR_DEFINE中间代码生成最终的arm汇编代码
    void var_define_ic_to_arm_asm(struct ic_data * result);
    //根据一条LABEL_DEFINE中间代码生成最终的arm汇编代码
    void label_define_ic_to_arm_asm(struct ic_label * result);
    //根据一条FUNC_DEFINE中间代码生成最终的arm汇编代码
    void func_define_ic_to_arm_asm(struct ic_func * result);
    //根据一条END_FUNC_DEFINE中间代码生成最终的arm汇编代码
    void end_func_define_ic_to_arm_asm();
    //根据一条CALL中间代码生成最终的arm汇编代码
    void call_ic_to_arm_asm(struct ic_func * arg1,list<struct ic_data * > * arg2,struct ic_data * result);
    //根据一条RET中间代码生成最终的arm汇编代码
    void ret_ic_to_arm_asm(struct ic_data * result);

    //把一行instruction插入到当前汇编文件的最后
    inline void push_instruction(Arm_instruction * instruction);
    //把一行pseudo instruction插入到当前汇编文件的最后
    inline void push_pseudo_instruction(Arm_pseudo_instruction * pseudo_instruction);
    //把一行label插入到当前汇编函数的最后
    inline void push_label_to_func(Arm_label * label);
    //把一行label插入到当前汇编文件的全局区域的最后
    inline void push_label_to_global(Arm_label * label);
    //把一行directive插入到汇编的全局区域当中
    inline void push_directive(Arm_directive * directive);

    //事件处理函数
    void handle_WRITE_CONST_INT_TO_REG(int const_int_data,reg_index reg);
    void handle_WRITE_CONST_FLOAT_TO_REG(float const_float_data,reg_index reg);
    void handle_STORE_VAR_TO_MEM(struct ic_data * var,reg_index reg);
    void handle_LOAD_VAR_TO_REG(struct ic_data * var,reg_index reg);
    void handle_WRITE_ADDR_TO_REG(struct ic_data * var,reg_index reg);
    void handle_PUSH_TEMP_VAR_FROM_REG_TO_STACK(struct ic_data * var,reg_index reg);
    void handle_CALL_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    void handle_CALL_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg);
    void handle_MOVE_DATA_BETWEEN_CPU_REGS(reg_index from,reg_index to);
    void handle_ASSIGN_VAR(struct ic_data * from,struct ic_data * to);
    void handle_POP_STACK(size_t pop_size);
    void handle_START_FUNC(struct ic_func * func);
    void handle_START_BASIC_BLOCK();

public:
    //构造函数
    Arm_instruction_generator(map<reg_index,string> regs_info);

    //析构函数
    ~Arm_instruction_generator();

    //事件处理函数(由中介者进行调用)
    struct event handler(struct event event);

};

#endif //__ARM_INSTRUCTION_GENERATOR_H