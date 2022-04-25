/*
 *
 *  arm_asm.cpp
 *  arm汇编指令
 *  Creator:Sichao Chen
 *  Create time:2022/4/24
 *
*/
#include "arm_asm.h"
#include "instruction_generator.h"

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
            res+=(operand2_shift_Rs_op_output_map[Rm_shift.shift_op]+" "+Instruction_generator::regs_info_[Rm_shift.Rs]);
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