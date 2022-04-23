/*
 *
 *  arm_instruction_generator.cpp
 *  arm汇编指令生成器
 *  Creator:Sichao Chen
 *  Create time:2022/1/28
 *
*/
#include "arm_instruction_generator.h"

map<reg_index,string> Instruction_generator::regs_info_;

//用于directive的输出
map<enum arm_directive_type,string> directive_output_map=
{
    {arm_directive_type::ALIGN,".align"},
    {arm_directive_type::ARCH,".arch"},
    {arm_directive_type::ARM,".arm"},
    {arm_directive_type::ASCII,".ascii"},
    {arm_directive_type::ASCIZ,".asciz"},
    {arm_directive_type::BSS,".bss"},
    {arm_directive_type::CODE,".code"},
    {arm_directive_type::COMM,".comm"},
    {arm_directive_type::DATA,".data"},
    {arm_directive_type::EABI_ATTRIBUTE,".eabi_attribute"},
    {arm_directive_type::FILE,".file"},
    {arm_directive_type::FORCE_THUMB,".force_thumb"},
    {arm_directive_type::FPU,".fpu"},
    {arm_directive_type::GLOBAL,".global"},
    {arm_directive_type::IDENT,".ident"},
    {arm_directive_type::SIZE,".size"},
    {arm_directive_type::SYNTAX,".syntax"},
    {arm_directive_type::SECTION,".section"},
    {arm_directive_type::SPACE,".space"},
    {arm_directive_type::TEXT,".text"},
    {arm_directive_type::THUMB,".thumb"},
    {arm_directive_type::THUMB_FUNC,".thumb_func"},
    {arm_directive_type::TYPE,".type"},
    {arm_directive_type::WORD,".word"},
};

//用于instruction的输出
map<enum arm_op,string> op_output_map=
{
    {arm_op::B,"b"},
    {arm_op::BL,"bl"},
    {arm_op::BLX,"blx"},
    {arm_op::BX,"bx"},
    {arm_op::ADD,"add"},
    {arm_op::SUB,"sub"},
    {arm_op::RSB,"rsb"},
    {arm_op::ADC,"adc"},
    {arm_op::SBC,"sbc"},
    {arm_op::RSC,"rsc"},
    {arm_op::AND,"and"},
    {arm_op::ORR,"orr"},
    {arm_op::EOR,"eor"},
    {arm_op::BIC,"bic"},
    {arm_op::CLZ,"clz"},
    {arm_op::CMP,"cmp"},
    {arm_op::CMN,"cmn"},
    {arm_op::MOV,"mov"},
    {arm_op::MOVW,"movw"},
    {arm_op::MOVT,"movt"},
    {arm_op::MVN,"mvn"},
    {arm_op::MUL,"mul"},
    {arm_op::MLA,"mla"},
    {arm_op::TST,"tst"},
    {arm_op::TEQ,"teq"},
    {arm_op::LDM,"ldm"},
    {arm_op::STM,"stm"},
    {arm_op::PUSH,"push"},
    {arm_op::POP,"pop"},
    {arm_op::LDR,"ldr"},
    {arm_op::STR,"str"},
    {arm_op::VMOV,"vmov"},
    {arm_op::VABS,"vabs"},
    {arm_op::VCPY,"vcpy"},
    {arm_op::VNEG,"vneg"},
    {arm_op::VSQRT,"vsqrt"},
    {arm_op::VADD,"vadd"},
    {arm_op::VSUB,"vsub"},
    {arm_op::VDIV,"vdiv"},
    {arm_op::VMLA,"vmla"},
    {arm_op::VNMLA,"vnmla"},
    {arm_op::VMLS,"vmls"},
    {arm_op::VNMLS,"vnmls"},
    {arm_op::VMUL,"vmul"},
    {arm_op::VNMUL,"vnmul"},
    {arm_op::VCMP,"vcmp"},
    {arm_op::VLDM,"vldm"},
    {arm_op::VSTM,"vstm"},
    {arm_op::VPOP,"vpop"},
    {arm_op::VPUSH,"vpush"},
    {arm_op::VLDR,"vldr"},
    {arm_op::VSTR,"vstr"},
};

map<enum precision,string> precision_output_map=
{
    {precision::S,".32"},
    {precision::D,".64"},
};

map<enum arm_data_type,string> data_type_output_map=
{
    {arm_data_type::B,"b"},
    {arm_data_type::SB,"sb"},
    {arm_data_type::H,"h"},
    {arm_data_type::SH,"sh"},
    {arm_data_type::W,""},
    {arm_data_type::D,"d"},
};

map<enum arm_condition,string> condition_output_map=
{
    {arm_condition::NONE,""},
    {arm_condition::EQ,"eq"},
    {arm_condition::NE,"ne"},
    {arm_condition::CS,"cs"},
    {arm_condition::HS,"hs"},
    {arm_condition::CC,"cc"},
    {arm_condition::LO,"lo"},
    {arm_condition::MI,"mi"},
    {arm_condition::PL,"pl"},
    {arm_condition::VS,"vs"},
    {arm_condition::VC,"vc"},
    {arm_condition::HI,"hi"},
    {arm_condition::LS,"ls"},
    {arm_condition::GE,"ge"},
    {arm_condition::LT,"lt"},
    {arm_condition::GT,"gt"},
    {arm_condition::LE,"le"},
    {arm_condition::AL,"al"},
};

map<enum address_mode,string> address_mode_output_map=
{
    {address_mode::NONE,""},
    {address_mode::IA,"ia"},
    {address_mode::IB,"ib"},
    {address_mode::DA,"da"},
    {address_mode::DB,"db"},
};

map<enum operand2_shift_op,string> operand2_shift_n_op_output_map=
{
    {operand2_shift_op::ASR_N,"asr"},
    {operand2_shift_op::LSL_N,"lsl"},
    {operand2_shift_op::LSR_N,"lsr"},
    {operand2_shift_op::ROR_N,"ror"},
};

map<enum operand2_shift_op,string> operand2_shift_Rs_op_output_map=
{
    {operand2_shift_op::ASR_RS,"asr"},
    {operand2_shift_op::LSL_RS,"lsl"},
    {operand2_shift_op::LSR_RS,"lsr"},
    {operand2_shift_op::ROR_RS,"ror"},
};

map<enum operand2_shift_op,string> operand2_shift_neither_n_and_Rs_op_output_map=
{
    {operand2_shift_op::RRX,"rrx"},
};

map<enum immed_16r_var_addr_type,string> immed_16r_var_addr_type_output_map=
{
    {immed_16r_var_addr_type::LOWER16,"lower16"},
    {immed_16r_var_addr_type::UPPER16,"upper16"},
};

map<enum flexoffset_shift_op,string> flexoffset_shift_n_output_map=
{
    {flexoffset_shift_op::ASR_N,"asr"},
    {flexoffset_shift_op::LSL_N,"lsl"},
    {flexoffset_shift_op::LSR_N,"lsr"},
    {flexoffset_shift_op::ROR_N,"ror"},
};

map<enum flexoffset_shift_op,string> flexoffset_shift_not_n_output_map=
{
    {flexoffset_shift_op::RRX,"rrx"},
};

//用于pseudo instruction的输出
map<enum arm_pseudo_op,string> pseudo_instruction_output_map=
{
    {arm_pseudo_op::ADR,"adr"},
    {arm_pseudo_op::ADRL,"adrl"},
    {arm_pseudo_op::LDR,"ldr"},
    {arm_pseudo_op::NOP,"nop"},
    {arm_pseudo_op::IT,"it"}
};

string arm_registers::to_string() const
{
    string res;

    if(registers_.size()>0)
    {
        for(auto i : registers_)
        {
            res+=Instruction_generator::regs_info_[i]+",";
        }
        res.pop_back();
    }

    return res;
}

string operand2::to_string() const
{
    string res;
    if(type==operand2_type::IMMED_8R)
    {
        res="#"+std::to_string(immed_8r);
    }
    else if(type==operand2_type::RM_SHIFT)
    {
        res=Instruction_generator::regs_info_[Rm_shift.Rm]+",";
        if(Rm_shift.shift_op==operand2_shift_op::NONE)
        {
            res.pop_back();
        }
        else if(operand2_shift_n_op_output_map.find(Rm_shift.shift_op)!=operand2_shift_n_op_output_map.end())
        {
            res+=(operand2_shift_n_op_output_map[Rm_shift.shift_op]+" "+std::to_string(Rm_shift.n));
        }
        else if(operand2_shift_Rs_op_output_map.find(Rm_shift.shift_op)!=operand2_shift_Rs_op_output_map.end())
        {
            res+=(operand2_shift_Rs_op_output_map[Rm_shift.shift_op]+" "+Arm_instruction_generator::regs_info_[Rm_shift.Rs]);
        }
        else if(operand2_shift_neither_n_and_Rs_op_output_map.find(Rm_shift.shift_op)!=operand2_shift_neither_n_and_Rs_op_output_map.end())
        {
            res+=operand2_shift_neither_n_and_Rs_op_output_map[Rm_shift.shift_op];
        }
    }

    return res;
}

string immed_16r::to_string() const
{
    string res="#";

    if(type==immed_16r_type::IMMED)
    {
        res+=::to_string(immed_data);
    }
    else if(type==immed_16r_type::VAR_ADDR)
    {
        res+=(":"+immed_16r_var_addr_type_output_map.at(var_addr.addr_type)+":"+var_addr.var->get_var_name());
    }

    return res;
}

string flexoffset::to_string() const
{
    string res;

    if(this->type==flexoffset_type::EXPR)
    {
        res="#"+std::to_string(this->expr);
    }
    else if(this->type==flexoffset_type::RM_SHIFT)
    {
        if(this->Rm_shift.reverse)
        {
            res="-";
        }
        res+=(Instruction_generator::regs_info_[this->Rm_shift.Rm]+",");
        if(this->Rm_shift.shift_op==flexoffset_shift_op::NONE)
        {
            res.pop_back();
        }
        else if(flexoffset_shift_n_output_map.find(this->Rm_shift.shift_op)!=flexoffset_shift_n_output_map.end())
        {
            res+=(flexoffset_shift_n_output_map[this->Rm_shift.shift_op]+" "+std::to_string(this->Rm_shift.n));
        }
        else if(flexoffset_shift_not_n_output_map.find(this->Rm_shift.shift_op)!=flexoffset_shift_not_n_output_map.end())
        {
            res+=flexoffset_shift_not_n_output_map[this->Rm_shift.shift_op];
        }
    }

    return res;
}

string Arm_directive::to_string() const
{
    string res="\t"+directive_output_map[directive_type_]+" ";
    if(data_.size()>0)
    {
        for(auto i : data_)
        {
            res+=(i+",");
        }
        res.pop_back();
    }
    return res;
}

string Arm_pseudo_instruction::to_string() const
{
    string res="\t"+pseudo_instruction_output_map[op_],cond1;
    bool tag=false;
    switch(op_)
    {
        case arm_pseudo_op::NOP:
            break;
        case arm_pseudo_op::IT:
            for(auto i:conds_)
            {
                if(!tag)
                {
                    cond1=i;
                    tag=true;
                }
                else
                {
                    res+=i;
                }
            }
            res+=(" "+cond1);
            break;
        default:
            res+=(condition_output_map[cond_]+" "+Instruction_generator::regs_info_[reg_]+",");
            if(op_==arm_pseudo_op::LDR)
            {
                res+="=";
            }
            res+=expr_;
            break;
    }
    return res;
}

string Arm_cpu_branch_instruction::to_string() const
{
    return "\t"+op_output_map[op_]+condition_output_map[cond_]+"\t"+source_registers_.to_string()+label_.to_string(false);
}

string Arm_cpu_data_process_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+(update_flags_?"s":"")+"\t",destination_registers=destination_registers_.to_string(),source_registers=source_registers_.to_string();

    if(destination_registers!="")
    {
        res+=(destination_registers+",");
    }

    if(source_registers!="")
    {
        res+=(source_registers+",");
    }

    switch(data_type_)
    {
        case arm_data_process_instruction_data_type::OPERNAD2:
            res+=(operand2_.to_string());
            break;
        case arm_data_process_instruction_data_type::IMMED_16R:
            res+=(immed_16r_.to_string());
            break;
        case arm_data_process_instruction_data_type::NONE:
            res.pop_back();
            break;
        default:
            break;
    }

    return res;
}

string Arm_cpu_multiple_registers_load_and_store_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+address_mode_output_map[address_mode_]+"\t",source_registers=source_registers_.to_string();

    if(!source_registers_.registers_.empty())
    {
        res+=(source_registers_.to_string()+(write_back_Rn_?"!":"")+",");
    }
    res+=("{"+destination_registers_.to_string()+"}");

    return res;
}

string Arm_cpu_single_register_load_and_store_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+data_type_output_map[data_type_]+"\t"+destination_registers_.to_string()+",";
    switch(single_register_load_and_store_type_)
    {
        case arm_single_register_load_and_store_type::ZERO_OFFSET:
            res+=("["+source_registers_.to_string()+"]");
            break;
        case arm_single_register_load_and_store_type::PRE_INDEXED_OFFSET:
            res+=("["+source_registers_.to_string()+","+flexoffset_.to_string()+"]");
            if(write_back_Rn_)
            {
                res+="!";
            }
            break;
        case arm_single_register_load_and_store_type::PROGRAM_RELATIVE:
            res+=label_;
            break;
        case arm_single_register_load_and_store_type::POST_INDEXED_OFFSET:  
            res+=("["+source_registers_.to_string()+"],"+flexoffset_.to_string());
            break;
        default:
            break;
    }
    return res;
}

string Arm_vfp_register_transfer_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+"\t"+destination_registers_.to_string()+",";

    if(source_registers_.registers_.empty())
    {
        res+=::to_string(imm_);
    }
    else
    {
        res+=source_registers_.to_string();
    }

    return res;
}

string Arm_vfp_data_process_instruction::to_string() const
{
    string res="\t"+op_output_map[op_];

    if(E_)
    {
        res+="E";
    }

    res+=(condition_output_map[cond_]+precision_output_map[precision_]+"\t"+destination_registers_.to_string());

    if(op_==arm_op::VCMP)
    {
        if(compare_with_zero_)
        {
            res+=",#0";
        }
    }
    else
    {
        res+=(","+source_registers_.to_string());
    }

    return res;
}

string Arm_vfp_multiple_registers_load_and_store_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+address_mode_output_map[address_mode_]+"\t";

    if(!source_registers_.registers_.empty())
    {
        res+=(source_registers_.to_string()+(write_back_Rn_?"!":"")+",");
    }
    res+=("{"+destination_registers_.to_string()+"}");

    return res;
}

string Arm_vfp_single_register_load_and_store_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+precision_output_map[precision_]+"\t"+destination_registers_.to_string()+",";

    switch(single_register_load_and_store_type_)
    {
        case arm_single_register_load_and_store_type::ZERO_OFFSET:
            res+=("["+source_registers_.to_string()+"]");
            break;
        case arm_single_register_load_and_store_type::PRE_INDEXED_OFFSET:
            res+=("["+source_registers_.to_string()+","+flexoffset_.to_string()+"]");
            break;
        case arm_single_register_load_and_store_type::PROGRAM_RELATIVE:
            res+=label_;
            break;
        default:
            break;
    }

    return res;
}

string Arm_label::to_string(bool is_define) const
{
    return name_+(is_define?":":"");
}

string Arm_label::to_string() const
{
    return name_+":";
}

Arm_instruction_generator::Arm_instruction_generator(map<reg_index,string> regs_info):Instruction_generator(regs_info)
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
    //intermediate_codes_=intermediate_codes_flow_graph;
    generate_arm_flow_graph();
    asm_codes_=arm_flow_graph_.to_string();
}

/*
根据中间代码生成arm汇编文件的流图形式，并将其赋值给arm_flow_graph_成员
*/
void Arm_instruction_generator::generate_arm_flow_graph()
{
    // //先把全局变量的中间代码翻译成arm流图形式
    // for(auto i:(intermediate_codes_->global_defines))
    // {
    //     ic_to_arm_asm(i);
    // }
    // //再把函数的中间代码翻译成arm流图形式
    // for(auto i:(intermediate_codes_->func_flow_graphs))
    // {
    //     arm_flow_graph_.add_arm_asm_to_current_func(nullptr,i->func,false);
    //     notify(event(event_type::START_FUNC,i));
    //     for(auto j:(i->basic_blocks))
    //     {
    //         arm_flow_graph_.add_arm_asm_to_current_func(nullptr,nullptr,true);
    //         notify(event(event_type::START_BASIC_BLOCK,j));
    //         for(auto k:(j->ic_sequence))
    //         {
    //             ic_to_arm_asm(k.intermediate_code);
    //         }
    //         notify(event(event_type::END_BASIC_BLOCK,j));
    //     }
    //     notify(event(event_type::END_FUNC,i));
    // }
    struct quaternion ic=get_next_intermediate_code();
    while(!ic.is_invalid())
    {
        ic_to_arm_asm(ic);
        ic=get_next_intermediate_code();
    }
}

/*
根据一条中间代码生成最终的arm汇编代码

Parameters
----------
intermediate_code:要转换的一条中间代码
*/
//#include<iostream>
//using namespace std;
void Arm_instruction_generator::ic_to_arm_asm(struct quaternion intermediate_code)
{
    //cout<<((int)intermediate_code.op)<<endl;
    switch(intermediate_code.op)
    {
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
        case ic_op::L_SHIFT:
            l_shift_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::R_SHIFT:
            r_shift_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::NOT:
            not_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::AND:
            and_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::OR:
            or_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
        case ic_op::XOR:
            xor_ic_to_arm_asm((struct ic_data *)intermediate_code.arg1.second,(struct ic_data *)intermediate_code.arg2.second,(struct ic_data *)intermediate_code.result.second);
            break;
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
    int const_value;
    reg_index Rm;
    if(ic_data->is_const() && shift_op!=operand2_shift_op::RRX)
    {
        const_value=ic_data->get_value().int_data;
        switch(shift_op)
        {
            case operand2_shift_op::ASR_N:
                ASR_N(const_value,8,n);
                break;
            case operand2_shift_op::LSL_N:
                LSL_N(const_value,8,n);
                break;
            case operand2_shift_op::LSR_N:
                LSR_N(const_value,8,n);
                break;
            case operand2_shift_op::ROR_N:
                ROR_N(const_value,8,n);
                break;
            default:
                break;
        }
        res=get_operand2(const_value);
    }
    else
    {
        if(ic_data->is_const())
        {
            res=operand2((reg_index)notify(event(event_type::GET_REG_FOR_INT_CONST,ic_data->get_value().int_data)).int_data,shift_op,n);
        }
        else
        {
            res=operand2((reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)ic_data)).int_data,shift_op,n);
        }
    }
    return res;
}

/*
根据int常数获得一个operand2

Parameters
----------
const_int_value:要处理的int常数

Return
------
返回得到的operand2
*/
struct operand2 Arm_instruction_generator::get_operand2(int const_int_value)
{
    struct operand2 res;
    reg_index Rm;
    if(operand2::is_legal_immed_8r(const_int_value))
    {
        res=operand2(const_int_value);
    }
    else
    {
        Rm=(reg_index)notify(event(event_type::GET_REG_FOR_INT_CONST,const_int_value)).int_data;
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
    if(flexoffset::is_legal_expr(offset))
    {
        res=flexoffset(offset);
    }
    else
    {
        res=flexoffset(false,(reg_index)notify(event(event_type::GET_REG_FOR_INT_CONST,offset)).int_data,flexoffset_shift_op::NONE,0);
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
            res=flexoffset(is_negative,(reg_index)notify(event(event_type::GET_REG_FOR_INT_CONST,ic_data->get_value().int_data)).int_data,shift_op,n);
        }
        else
        {
            res=flexoffset(is_negative,(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,ic_data)).int_data,shift_op,n);
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
            //我们知道：当这条指令执行完毕之后，由于b的值被改写了，因此如果原本有脏值寄存器里面存在着a[b]的值，那么这个值需要在b被正式改写之前写回寄存器，并将该寄存器设置为NOT_USED
            //那么如果此时我们先获取b所需的寄存器，然后再去获取a[b]所需的寄存器
            //那么在获取b所需的寄存器的时候会先把a[b]所在的寄存器设置为NOT_USED（如果原本是脏值的话还会进行写回）
            //接着在获取a[b]所需的寄存器的时候就又会把a[b]的值从内存中读入某一个寄存器，并且这个寄存器会被设置为USED
            //然后接下来这条ADD指令翻译完毕之后，就会有一个USED寄存器中存放的是该指令执行之前的a[b]的值，这样是错误的
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,Rd,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
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
    pair<struct ic_data *,int> * event_data;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            if(arg1->is_const() && !arg1->is_array_var())
            {
                //如果SUB的第一个操作数是常数
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                operand2=get_operand2(arg1);
                Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::RSB,arm_condition::NONE,false,Rd,Rn,operand2));
            }
            else
            {
                //如果SUB的第二个操作数是常数
                //或者两个操作数都是变量
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                operand2=get_operand2(arg2);
                Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
                push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,Rd,Rn,operand2));
            }
            break;
        case language_data_type::FLOAT:
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
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
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MUL,arm_condition::NONE,false,Rd,Rm,Rs));
            break;
        case language_data_type::FLOAT:
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
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
    pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * event_data;
    reg_index Rd,Rm,Rs;
    switch(result->get_data_type())
    {
        case language_data_type::INT:
            r_params=new list<struct ic_data * >;
            event_data=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
            event_data->first.first="__aeabi_idiv";
            r_params->push_back(arg1);
            r_params->push_back(arg2);
            event_data->first.second=r_params;
            event_data->second.first=result;
            event_data->second.second=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
            notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
            delete r_params;
            delete event_data;
            break;
        case language_data_type::FLOAT:
            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            Rs=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
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
    pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * event_data;
    //不能对浮点数进行mod
    r_params=new list<struct ic_data * >;
    event_data=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
    event_data->first.first="__aeabi_idivmod";
    r_params->push_back(arg1);
    r_params->push_back(arg2);
    event_data->first.second=r_params;
    event_data->second.first=result;
    event_data->second.second=(reg_index)notify(event(event_type::GET_R1_REG,nullptr)).int_data;
    notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
    delete r_params;
    delete event_data;
}

/*
根据一条L_SHIFT中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::l_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{

}

/*
根据一条R_SHIFT中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::r_shift_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{

}

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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            op2=get_operand2((int)0);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,op2));
            event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::EQ));
            notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
            delete event_data;
            notify(event(event_type::END_INSTRUCTION,nullptr));

            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction("eq","e"));
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::EQ,false,Rd,operand2((int)1)));
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NE,false,Rd,operand2((int)0)));
            notify(event(event_type::END_INSTRUCTION,nullptr));
            break;
        case language_data_type::FLOAT:
            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            event_data=new pair<struct ic_data *,int>(result,(int)(arm_condition::EQ));
            notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
            delete event_data;
            notify(event(event_type::END_INSTRUCTION,nullptr));

            notify(event(event_type::START_INSTRUCTION,nullptr));
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)result)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction("eq","e"));
            push_instruction(new Arm_vfp_register_transfer_instruction(arm_condition::EQ,Rd,1.f));
            push_instruction(new Arm_vfp_data_process_instruction(arm_op::VSUB,arm_condition::NE,precision::S,Rd,Rd,Rd));
            notify(event(event_type::END_INSTRUCTION,nullptr));
            break;
        default:
            break;
    }
}

/*
根据一条AND中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::and_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{

}

/*
根据一条OR中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::or_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{

}

/*
根据一条XOR中间代码生成最终的arm汇编代码
*/
void Arm_instruction_generator::xor_ic_to_arm_asm(struct ic_data * arg1,struct ic_data * arg2,struct ic_data * result)
{

}

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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                cond=arm_condition::LE;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                cond=arm_condition::GE;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                cond=arm_condition::LT;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
            Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
            operand2=get_operand2(arg2);
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::CMP,arm_condition::NONE,Rn,operand2));
            break;
        case language_data_type::FLOAT:
            if(arg1->is_const() && !arg1->is_array_var() && arg1->get_value().float_data==0)
            {
                //如果EQ的第一个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                cond=arm_condition::GT;
            }
            else if(arg2->is_const() && !arg2->is_array_var() && arg2->get_value().float_data==0)
            {
                //如果EQ的第二个操作数是常数0
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
            }
            else
            {
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg2)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn,Rm));
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
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为NOT_USED
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
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
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
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        default:
            break;
    }
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为NOT_USED
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
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
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
                Rn=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)arg1)).int_data;
                push_instruction(new Arm_vfp_data_process_instruction(false,arm_condition::NONE,precision::S,Rn));
                event_data=new pair<struct ic_data *,int>(arg1,(int)(arm_condition::NE));
                notify(event(event_type::CHANGE_FLAGS_FOR_VAR,(void *)event_data));
                delete event_data;
                notify(event(event_type::END_INSTRUCTION,nullptr));
            }
            break;
        default:
            break;
    }
    //跳转语句一定会结束某一个基本块，因此在实际的跳转开始之前，必须将所有的寄存器写回内存并设置为NOT_USED
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
    size_t zero_array_member_num=0,array_member_offset=0;
    vector<OAA> * array_init_values;
    reg_index Rd;
    struct operand2 operand2;
    pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * event_data_1;
    pair<struct ic_data *,struct ic_data * > * event_data_2;
    list<struct ic_data * > * r_params;
    Symbol_table * symbol_table=Symbol_table::get_instance();
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
                event_data_1=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
                event_data_1->first.first=string("memset");
                r_params=new list<struct ic_data *>;
                r_params->push_back(result);
                r_params->push_back(symbol_table->const_entry(language_data_type::INT,OAA((int)0)));
                r_params->push_back(symbol_table->const_entry(language_data_type::INT,OAA((int)result->get_byte_size())));
                event_data_1->first.second=r_params;
                event_data_1->second.first=nullptr;
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
                    event_data_2->second=symbol_table->array_member_entry(result,result->dimensions_len->size(),symbol_table->const_entry(language_data_type::INT,OAA((int)array_member_offset)));
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
    reg_index sp;
    size_t local_vars_total_byte_size=0;
    list<reg_index> * context_saved_regs,* f_param_regs;
    //首先通知内存管理器和寄存器管理器，新的函数定义开始了
    notify(event(event_type::FUNC_DEFINE,(void *)result));
    //然后保护现场，把那些在函数的调用者看来不会改变的寄存器入栈保存
    //先保存CPU中的寄存器
    context_saved_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS,(void *)result)).pointer_data;
    if(context_saved_regs->size()>0)
    {
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(*context_saved_regs)));
    }
    delete context_saved_regs;
    //再保存VFP中的寄存器
    context_saved_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS,(void *)result)).pointer_data;
    if(context_saved_regs->size()>0)
    {
        push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(*context_saved_regs)));
    }
    delete context_saved_regs;
    //接着把存放在r0-r3中的前4*32bits的整型函数参数入栈保存，入栈的顺序是从r3到r0（此时函数的后面的参数如果有的话已经由函数的调用者将其入栈了）
    f_param_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_F_PARAM_CPU_REGS,(void *)result)).pointer_data;
    if(f_param_regs->size()>0)
    {
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(*f_param_regs)));
    }
    delete f_param_regs;
    //再把存放在s0-s15中的前16*32bits的浮点函数参数入栈保存，入栈的顺序是s15到s0
    f_param_regs=(list<reg_index> *)notify(event(event_type::READY_TO_PUSH_F_PARAM_VFP_REGS,(void *)result)).pointer_data;
    if(f_param_regs->size()>0)
    {
        push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(*f_param_regs)));
    }
    delete f_param_regs;
    //最后在栈中为函数的局部变量开辟空间
    local_vars_total_byte_size=notify(event(event_type::READY_TO_PUSH_LOCAL_VARS,(void *)result)).int_data;
    sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    if(local_vars_total_byte_size>0)
    {
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::SUB,arm_condition::NONE,false,sp,sp,get_operand2((int)local_vars_total_byte_size)));
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
    pair<int,reg_index> * event_data;
    //先把此时留在寄存器中的脏值全部写回内存
    notify(event(event_type::FUNC_RET,nullptr));
    //再把返回值放入寄存器
    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(result)
    {
        notify(event(event_type::RETURN_VAR,(void *)result));
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
    //把使用到的临时变量退栈
    stack_offset+=notify(event(event_type::READY_TO_POP_TEMP_VARS,nullptr)).int_data;
    //然后把函数的局部变量退栈
    stack_offset+=notify(event(event_type::READY_TO_POP_LOCAL_VARS,nullptr)).int_data;
    //再把函数的前16*32bits的浮点型参数出栈
    stack_offset+=notify(event(event_type::READY_TO_POP_F_PARAM_VFP_REGS,nullptr)).int_data;
    //接着把函数的前4*4bytes的整型参数出栈
    stack_offset+=notify(event(event_type::READY_TO_POP_F_PARAM_CPU_REGS,nullptr)).int_data;
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
            if(notify(event(event_type::CHECK_CONST_INT_OWN_VALUE_REG,(int)stack_offset)).bool_data)
            {
                //如果此时该stack_offset的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                op2=operand2((reg_index)notify(event(event_type::GET_CONST_INT_S_VALUE_REG,(int)stack_offset)).int_data);
            }
            else
            {
                //如果此时没有寄存器中存放着stack_offset的值，那么就需要给该值新分配一个寄存器
                const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                //分配完之后还要再得到新的stack_offset，因为上面的ALLOCATE_IDLE_REG可能会使得新的临时变量入栈，从而栈顶的位置发送变化
                stack_offset=(size_t)(notify(event(event_type::READY_TO_POP_TEMP_VARS,nullptr)).int_data+notify(event(event_type::READY_TO_POP_LOCAL_VARS,nullptr)).int_data+notify(event(event_type::READY_TO_POP_F_PARAM_VFP_REGS,nullptr)).int_data+notify(event(event_type::READY_TO_POP_F_PARAM_CPU_REGS,nullptr)).int_data);
                //然后把新的stack_offset值写入寄存器中
                push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,const_reg,to_string(stack_offset)));
                //再通知内存管理模块：寄存器被新的stack_offset占用了
                event_data=new pair<int,reg_index>(stack_offset,const_reg);
                notify(event(event_type::ATTACH_CONST_INT_TO_REG,(void *)event_data));
                delete event_data;
                //最后把得到的寄存器作为operand2的一部分即可
                op2=operand2(const_reg);
            }
        }
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,op2));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
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
把一行instruction插入到正在生成的函数的最后

Parameters
----------
instruction:要插入的instruction
*/
void Arm_instruction_generator::push_instruction(Arm_instruction * instruction)
{
    arm_flow_graph_.add_arm_asm_to_current_func(instruction,nullptr,false);
}

/*
把一行pseudo instruction插入到正在生成的函数的最后

Parameters
----------
pseudo instruction:要插入的pseudo instruction
*/
void Arm_instruction_generator::push_pseudo_instruction(Arm_pseudo_instruction * pseudo_instruction)
{
    arm_flow_graph_.add_arm_asm_to_current_func(pseudo_instruction,nullptr,false);
}

/*
把一行label插入到当前的汇编函数中

Parameters
----------
label:要插入的label
*/
void Arm_instruction_generator::push_label_to_func(Arm_label * label)
{
    arm_flow_graph_.add_arm_asm_to_current_func(label,nullptr,false);
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

void Arm_instruction_generator::handle_WRITE_CONST_INT_TO_REG(int const_int_data,reg_index reg)
{
    push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,reg,to_string(const_int_data)));
}

void Arm_instruction_generator::handle_WRITE_CONST_FLOAT_TO_REG(float const_float_data,reg_index reg)
{
    //push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,reg,to_string(const_int_data)));
}

void Arm_instruction_generator::handle_STORE_VAR_TO_MEM(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp;
    reg_index addr_reg;

    if(var->is_const() || var->is_tmp_var() || (var->is_array_var() && !var->is_f_param()) || (var->is_array_member() && var->is_array_var()))
    {
        //常量数据和临时变量不需要写回
        //对于数组变量来说，只有那些函数形参才能写回
        //如果一个变量即是数组，又是数组取元素，那么就意味着这个变量其实是一个数组取元素，只不过没有全部维度都用于取元素，这种变量也是不能写回的
        return;
    }

    notify(event(event_type::START_INSTRUCTION,nullptr));
    switch(var->get_data_type())
    {
        case language_data_type::INT:
            if(var->is_array_member())
            {
                //对于数组取元素的变量，需要先获取其所属数组的地址，然后取偏移
                addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
                //取偏移的时候需要乘上数组元素类型的大小（这里默认是4）
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
            }
            else if(var->is_global())
            {
                //对于全局变量
                addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
                //push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,var->get_var_name()));
            }
            else
            {
                //对于那些非数组取元素的局部变量和函数形参，使用栈指针sp的偏移量来取它们的地址
                addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::STR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
            }
            break;
        case language_data_type::FLOAT:
            if(var->is_array_member())
            {
                //对于数组取元素的变量，需要先获取其所属数组的地址，然后取偏移
                addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
                //取偏移的时候需要乘上数组元素类型的大小（这里默认是4）
                push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VSTR,arm_condition::NONE,precision::S,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false)));
            }
            else if(var->is_global())
            {
                //对于全局变量
                push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VSTR,arm_condition::NONE,precision::S,reg,var->get_var_name()));
            }
            else
            {
                //对于那些非数组取元素的局部变量和函数形参，使用栈指针sp的偏移量来取它们的地址
                addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VSTR,arm_condition::NONE,precision::S,reg,addr_reg));
            }
            break;
        default:
            break;
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_LOAD_VAR_TO_REG(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp;
    reg_index addr_reg,const_reg;
    struct operand2 op2;
    pair<int,reg_index> * event_data;

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
            addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
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
                if(notify(event(event_type::CHECK_CONST_INT_OWN_VALUE_REG,(int)var_stack_pos_from_sp)).bool_data)
                {
                    //如果此时该var_stack_pos_from_sp的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                    op2=operand2((reg_index)notify(event(event_type::GET_CONST_INT_S_VALUE_REG,(int)var_stack_pos_from_sp)).int_data);
                }
                else
                {
                    //如果此时没有寄存器中存放着var_stack_pos_from_sp的值，那么就需要给该值新分配一个寄存器
                    const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                    //分配完之后还要再进行一次GET_VAR_STACK_POS_FROM_SP，得到新的var_stack_pos_from_sp，因为上面的ALLOCATE_IDLE_REG可能会使得栈顶的位置发送变化
                    var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
                    //然后把新的var_stack_pos_from_sp值写入寄存器中
                    push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,const_reg,to_string(var_stack_pos_from_sp)));
                    //再通知内存管理模块：寄存器被新的var_stack_pos_from_sp占用了
                    event_data=new pair<int,reg_index>(var_stack_pos_from_sp,const_reg);
                    notify(event(event_type::ATTACH_CONST_INT_TO_REG,(void *)event_data));
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
        switch(var->get_data_type())
        {
            case language_data_type::INT:
                if(var->is_array_member())
                {
                    //如果要load到寄存器中的变量是数组取元素,并且该数组取元素不是数组
                    addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
                    //默认数组元素的大小都是4bytes
                    push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
                }
                else if(var->is_global())
                {
                    //如果要load到寄存器中的变量是全局变量
                    addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                    push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
                    //push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,var->get_var_name()));
                }
                else
                {
                    //如果要load到寄存器中的变量是局部变量或者函数形参或者临时变量
                    addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                    push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
                }
                break;
            case language_data_type::FLOAT:
                if(var->is_array_member())
                {
                    //如果要load到寄存器中的变量是数组取元素,并且该数组取元素不是数组
                    addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
                    //默认数组元素的大小都是4bytes
                    push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false)));
                }
                else if(var->is_global())
                {
                    //如果要load到寄存器中的变量是全局变量
                    addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                    push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,addr_reg));
                    //push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,var->get_var_name()));
                }
                else
                {
                    //如果要load到寄存器中的变量是全局变量或者局部变量或者函数形参或者临时变量
                    addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
                    push_instruction(new Arm_vfp_single_register_load_and_store_instruction(arm_op::VLDR,arm_condition::NONE,precision::S,reg,addr_reg));
                }
                break;
            default:
                break;
        }
    }
    /*else if(var->is_array_member())
    {
        //如果要load到寄存器中的变量是数组取元素,并且该数组取元素不是数组
        addr_reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(var->get_belong_array()))).int_data;
        //默认数组元素的大小都是4bytes
        push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg,get_flexoffset((var->get_offset()),flexoffset_shift_op::LSL_N,2,false),false));
    }
    else if(var->is_global())
    {
        //如果要load到寄存器中的变量是全局变量
        addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
        push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
    }
    else
    {
        //如果要load到寄存器中的变量是局部变量或者函数形参或者临时变量
        addr_reg=(reg_index)notify(event(event_type::GET_ADDR_REG,(void *)var)).int_data;
        push_instruction(new Arm_cpu_single_register_load_and_store_instruction(arm_op::LDR,arm_condition::NONE,arm_data_type::W,reg,addr_reg));
    }*/
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_WRITE_ADDR_TO_REG(struct ic_data * var,reg_index reg)
{
    size_t var_stack_pos_from_sp;
    reg_index sp,const_reg;
    struct operand2 op2;
    pair<int,reg_index> * event_data;

    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(var->is_global())
    {
        //如果要获取地址的变量是全局变量
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVW,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::LOWER16)));
        push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOVT,arm_condition::NONE,reg,immed_16r(var,immed_16r_var_addr_type::UPPER16)));
    }
    else
    {
        //如果要获取地址的变量是局部变量或者函数形参或者临时变量
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
            if(notify(event(event_type::CHECK_CONST_INT_OWN_VALUE_REG,(int)var_stack_pos_from_sp)).bool_data)
            {
                //如果此时该var_stack_pos_from_sp的值是否已经被放入某一个寄存器中了，那么只需要获取该寄存器即可
                op2=operand2((reg_index)notify(event(event_type::GET_CONST_INT_S_VALUE_REG,(int)var_stack_pos_from_sp)).int_data);
            }
            else
            {
                //如果此时没有寄存器中存放着var_stack_pos_from_sp的值，那么就需要给该值新分配一个寄存器
                const_reg=(reg_index)notify(event(event_type::ALLOCATE_IDLE_CPU_REG,nullptr)).int_data;
                //分配完之后还要再进行一次GET_VAR_STACK_POS_FROM_SP，得到新的var_stack_pos_from_sp，因为上面的ALLOCATE_IDLE_REG可能会使得栈顶的位置发送变化
                var_stack_pos_from_sp=(size_t)notify(event(event_type::GET_VAR_STACK_POS_FROM_SP,(void *)var)).int_data;
                //然后把新的var_stack_pos_from_sp值写入寄存器中即可
                push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,const_reg,to_string(var_stack_pos_from_sp)));
                //通知内存管理模块：寄存器被新的var_stack_pos_from_sp占用了
                event_data=new pair<int,reg_index>(var_stack_pos_from_sp,const_reg);
                notify(event(event_type::ATTACH_CONST_INT_TO_REG,(void *)event_data));
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
        switch(var->get_data_type())
        {
            case language_data_type::INT:
                push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            case language_data_type::FLOAT:
                push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            default:
                break;
        }
        notify(event(event_type::PUSH_VAR_TO_STACK,(void *)var));
    }
}

void Arm_instruction_generator::handle_CALL_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg)
{
    //size_t r_params_num=r_params->size();
    size_t int_r_params_num=0,float_r_params_num=0;
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
    /*for(list<struct ic_data * >::reverse_iterator i=r_params->rbegin();i!=r_params->rend();i++)
    {
        if(r_params_num<=4)
        {
            break;
        }
        notify(event(event_type::START_INSTRUCTION,nullptr));
        reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(*i))).int_data;
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)(*i)));
        notify(event(event_type::END_INSTRUCTION,nullptr));
        r_params_num--;
    }*/
    for(auto i:*r_params)
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            if(int_r_params_num>=4)
            {
                r_params_passed_by_stack.push_front(i);
            }
            int_r_params_num++;
        }
        else if(i->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num>=16)
            {
                r_params_passed_by_stack.push_front(i);
            }
            float_r_params_num++;
        }
    }
    for(auto i:r_params_passed_by_stack)
    {
        notify(event(event_type::START_INSTRUCTION,nullptr));
        reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)i)).int_data;
        switch(i->get_data_type())
        {
            case language_data_type::INT:
                push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            case language_data_type::FLOAT:
                push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            default:
                break;
        }
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)i));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
    //再把需要存放在寄存器中的参数放入寄存器
    notify(event(event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC,(void *)r_params));
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

void Arm_instruction_generator::handle_CALL_ABI_FUNC(string func_name,list<struct ic_data * > * r_params,struct ic_data * return_value,reg_index return_reg)
{
    //size_t r_params_num=r_params->size();
    size_t int_r_params_num=0,float_r_params_num=0;
    int pop_stack_size=0;
    reg_index reg,sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    pair<struct ic_data *,reg_index> * event_data;
    set<reg_index> * regs_unaccessible;
    list<struct ic_data * > r_params_passed_by_stack;
    //先把参数寄存器中需要保存的进行保存，因为在函数调用的时候不会保证这些寄存器的值不变
    notify(event(event_type::SAVE_REGS_WHEN_CALLING_ABI_FUNC,nullptr));
    //把需要入栈的参数从右到左入栈
    //默认每一参数都是4bytes的
    /*for(list<struct ic_data *>::reverse_iterator i=r_params->rbegin();i!=r_params->rend();i++)
    {
        if(r_params_num<=4)
        {
            break;
        }
        notify(event(event_type::START_INSTRUCTION,nullptr));
        reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)(*i))).int_data;
        push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)(*i)));
        notify(event(event_type::END_INSTRUCTION,nullptr));
        r_params_num--;
    }*/
    for(auto i:*r_params)
    {
        if(i->get_data_type()==language_data_type::INT || i->is_array_var())
        {
            if(int_r_params_num>=4)
            {
                r_params_passed_by_stack.push_front(i);
            }
            int_r_params_num++;
        }
        else if(i->get_data_type()==language_data_type::FLOAT)
        {
            if(float_r_params_num>=16)
            {
                r_params_passed_by_stack.push_front(i);
            }
            float_r_params_num++;
        }
    }
    for(auto i:r_params_passed_by_stack)
    {
        notify(event(event_type::START_INSTRUCTION,nullptr));
        reg=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)i)).int_data;
        switch(i->get_data_type())
        {
            case language_data_type::INT:
                push_instruction(new Arm_cpu_multiple_registers_load_and_store_instruction(arm_op::PUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            case language_data_type::FLOAT:
                push_instruction(new Arm_vfp_multiple_registers_load_and_store_instruction(arm_op::VPUSH,arm_condition::NONE,arm_registers(1,reg)));
                break;
            default:
                break;
        }
        notify(event(event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC,(void *)i));
        notify(event(event_type::END_INSTRUCTION,nullptr));
    }
    //再把需要存放在寄存器中的参数放入寄存器
    notify(event(event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC,(void *)r_params));
    //执行函数跳转
    push_instruction(new Arm_cpu_branch_instruction(arm_op::BL,arm_condition::NONE,func_name));
    //如果函数有返回值，通知寄存器管理器：寄存器r0已经被相应的返回值占用了
    //同时也会通知内存管理器，下一步将要把入栈的参数清除，需要其返回要出栈的大小
    event_data=new pair<struct ic_data *,reg_index>(return_value,return_reg);
    pop_stack_size=notify(event(event_type::RET_FROM_CALLED_ABI_FUNC,(void *)event_data)).int_data;
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

void Arm_instruction_generator::handle_MOVE_DATA_BETWEEN_CPU_REGS(reg_index from,reg_index to)
{
    if(from!=to)
    {
        if(notify(event(event_type::IS_CPU_REG,(int)from)).bool_data && notify(event(event_type::IS_CPU_REG,(int)from)).bool_data)
        {
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NONE,to,operand2(from)));
        }
        else
        {
            push_instruction(new Arm_vfp_register_transfer_instruction(arm_condition::NONE,to,from));
        }
    }
}

void Arm_instruction_generator::handle_ASSIGN_VAR(struct ic_data * from,struct ic_data * to)
{
    reg_index Rd,Rm;
    struct operand2 operand2;
    list<struct ic_data * > * r_params;
    pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * event_data;
    notify(event(event_type::START_INSTRUCTION,nullptr));
    if(from->get_data_type()==language_data_type::INT && to->get_data_type()==language_data_type::INT)
    {
        if(from->is_const() && !from->is_array_var())
        {
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,Rd,to_string(from->get_value().int_data)));
        }
        else
        {
            operand2=get_operand2(from);
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_instruction(new Arm_cpu_data_process_instruction(arm_op::MOV,arm_condition::NONE,false,Rd,operand2));
        }
    }
    else if(from->get_data_type()==language_data_type::FLOAT && to->get_data_type()==language_data_type::FLOAT)
    {
        if(from->is_const() && !from->is_array_var())
        {
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,precision::S,Rd,to_string(from->get_value().float_data)));
        }
        else
        {
            Rm=(reg_index)notify(event(event_type::GET_REG_FOR_READING_VAR,(void *)from)).int_data;
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_instruction(new Arm_vfp_register_transfer_instruction(arm_condition::NONE,Rd,Rm));
        }
    }
    else if(from->get_data_type()==language_data_type::INT && to->get_data_type()==language_data_type::FLOAT)
    {
        if(from->is_const() && !from->is_array_var())
        {
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,precision::S,Rd,to_string((float)(from->get_value().int_data))));
        }
        else
        {
            r_params=new list<struct ic_data * >;
            event_data=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
            event_data->first.first="__aeabi_i2f";
            r_params->push_back(from);
            event_data->first.second=r_params;
            event_data->second.first=to;
            event_data->second.second=(reg_index)notify(event(event_type::GET_S0_REG,nullptr)).int_data;
            notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
            delete r_params;
            delete event_data;
        }
    }
    else if(from->get_data_type()==language_data_type::FLOAT && to->get_data_type()==language_data_type::INT)
    {
        if(from->is_const() && !from->is_array_var())
        {
            Rd=(reg_index)notify(event(event_type::GET_REG_FOR_WRITING_VAR,(void *)to)).int_data;
            push_pseudo_instruction(new Arm_pseudo_instruction(arm_condition::NONE,Rd,to_string((int)(from->get_value().float_data))));
        }
        else
        {
            r_params=new list<struct ic_data * >;
            event_data=new pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> >;
            event_data->first.first="__aeabi_f2iz";
            r_params->push_back(from);
            event_data->first.second=r_params;
            event_data->second.first=to;
            event_data->second.second=(reg_index)notify(event(event_type::GET_R0_REG,nullptr)).int_data;
            notify(event(event_type::CALL_ABI_FUNC,(void *)event_data));
            delete r_params;
            delete event_data;
        }
    }
    notify(event(event_type::END_INSTRUCTION,nullptr));
}

void Arm_instruction_generator::handle_POP_STACK(size_t pop_size)
{
    reg_index sp=(reg_index)notify(event(event_type::GET_SP_REG,nullptr)).int_data;
    push_instruction(new Arm_cpu_data_process_instruction(arm_op::ADD,arm_condition::NONE,false,sp,sp,get_operand2((int)pop_size)));
}

void Arm_instruction_generator::handle_START_FUNC(struct ic_func * func)
{
    arm_flow_graph_.add_arm_asm_to_current_func(nullptr,func,false);
}

void Arm_instruction_generator::handle_START_BASIC_BLOCK()
{
    arm_flow_graph_.add_arm_asm_to_current_func(nullptr,nullptr,true);
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
        case event_type::WRITE_CONST_INT_TO_REG:
            handle_WRITE_CONST_INT_TO_REG(((pair<int,reg_index> *)event.pointer_data)->first,((pair<int,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::WRITE_CONST_FLOAT_TO_REG:
            handle_WRITE_CONST_FLOAT_TO_REG(((pair<float,reg_index> *)event.pointer_data)->first,((pair<float,reg_index> *)event.pointer_data)->second);
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
        case event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK:
            handle_PUSH_TEMP_VAR_FROM_REG_TO_STACK(((pair<struct ic_data *,reg_index> *)event.pointer_data)->first,((pair<struct ic_data *,reg_index> *)event.pointer_data)->second);
            break;
        case event_type::CALL_FUNC:
            handle_CALL_FUNC(((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.second,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.second);
            break;
        case event_type::CALL_ABI_FUNC:
            handle_CALL_ABI_FUNC(((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->first.second,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.first,((pair<pair<string,list<struct ic_data * > * >,pair<struct ic_data *,reg_index> > * )event.pointer_data)->second.second);
            break;
        case event_type::MOVE_DATA_BETWEEN_CPU_REGS:
            handle_MOVE_DATA_BETWEEN_CPU_REGS(((pair<reg_index,reg_index> *)event.pointer_data)->first,((pair<reg_index,reg_index> *)event.pointer_data)->second);
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
            handle_START_BASIC_BLOCK();
            break;
        default:
            break;
    }
    return res;
}