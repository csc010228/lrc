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

inline string reg_ro_string(reg_index reg)
{
    if(Instruction_generator::regs_info_.find(reg)!=Instruction_generator::regs_info_.end())
    {
        return Instruction_generator::regs_info_.at(reg);
    }
    else
    {
        return "VR"+::to_string((int)reg);
    }
    return "";
}

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
    {arm_op::VMRS,"vmrs"},
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
    {precision::S,".f32"},
    {precision::D,".f64"},
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
    {arm_pseudo_op::VLDR,"vldr"},
    {arm_pseudo_op::NOP,"nop"},
    {arm_pseudo_op::IT,"it"}
};

set<enum arm_op> cpu_ops=
{
    arm_op::B,
    arm_op::BL,
    arm_op::BLX,
    arm_op::BX,
    arm_op::ADD,
    arm_op::SUB,
    arm_op::RSB,
    arm_op::ADC,
    arm_op::SBC,
    arm_op::RSC,
    arm_op::AND,
    arm_op::ORR,
    arm_op::EOR,
    arm_op::BIC,
    arm_op::CLZ,
    arm_op::CMP,
    arm_op::CMN,
    arm_op::MOV,
    arm_op::MVN,
    arm_op::MOVW,
    arm_op::MOVT,
    arm_op::MUL,
    arm_op::MLA,
    arm_op::TST,
    arm_op::TEQ,
    arm_op::LDM,
    arm_op::STM,
    arm_op::PUSH,
    arm_op::POP,
    arm_op::LDR,
    arm_op::STR,
};

set<enum arm_op> cpu_branch_ops=
{
    arm_op::B,
    arm_op::BL,
    arm_op::BLX,
    arm_op::BX,
};

set<enum arm_op> cpu_data_process_ops=
{
    arm_op::ADD,
    arm_op::SUB,
    arm_op::RSB,
    arm_op::ADC,
    arm_op::SBC,
    arm_op::RSC,
    arm_op::AND,
    arm_op::ORR,
    arm_op::EOR,
    arm_op::BIC,
    arm_op::CLZ,
    arm_op::CMP,
    arm_op::CMN,
    arm_op::MOV,
    arm_op::MVN,
    arm_op::MOVW,
    arm_op::MOVT,
    arm_op::MUL,
    arm_op::MLA,
    arm_op::TST,
    arm_op::TEQ,
};

set<enum arm_op> cpu_multiple_registers_load_and_store_ops=
{
    arm_op::LDM,
    arm_op::STM,
    arm_op::PUSH,
    arm_op::POP,
};

set<enum arm_op> cpu_single_register_load_and_store_ops=
{
    arm_op::LDR,
    arm_op::STR,
};

set<enum arm_op> vfp_ops=
{
    arm_op::VMOV,
    arm_op::VMRS,
    arm_op::VABS,
    arm_op::VCPY,
    arm_op::VNEG,
    arm_op::VSQRT,
    arm_op::VADD,
    arm_op::VSUB,
    arm_op::VDIV,
    arm_op::VMLA,
    arm_op::VNMLA,
    arm_op::VMLS,
    arm_op::VNMLS,
    arm_op::VMUL,
    arm_op::VNMUL,
    arm_op::VCMP,
    arm_op::VLDM,
    arm_op::VSTM,
    arm_op::VPOP,
    arm_op::VPUSH,
    arm_op::VLDR,
    arm_op::VSTR,
};

set<enum arm_op> vfp_register_transfer_ops=
{
    arm_op::VMOV,
    arm_op::VMRS,
};

set<enum arm_op> vfp_data_process_ops=
{
    arm_op::VABS,
    arm_op::VCPY,
    arm_op::VNEG,
    arm_op::VSQRT,
    arm_op::VADD,
    arm_op::VSUB,
    arm_op::VDIV,
    arm_op::VMLA,
    arm_op::VNMLA,
    arm_op::VMLS,
    arm_op::VNMLS,
    arm_op::VMUL,
    arm_op::VNMUL,
    arm_op::VCMP,
};

set<enum arm_op> vfp_multiple_registers_load_and_store_ops=
{
    arm_op::VLDM,
    arm_op::VSTM,
    arm_op::VPOP,
    arm_op::VPUSH,
};

set<enum arm_op> vfp_single_register_load_and_store_ops=
{
    arm_op::VLDR,
    arm_op::VSTR,
};
 


//===================================== struct arm_registers =====================================//

size_t arm_registers::get_regs_num() const
{
    return registers_.size();
}

reg_index arm_registers::get_only_member() const
{
    if(get_regs_num()==1)
    {
        for(auto reg:registers_)
        {
            return reg;
        }
    }
    return 0;
}

string arm_registers::to_string() const
{
    string res;
    if(registers_.size()>0)
    {
        for(auto i : registers_)
        {
            res+=reg_ro_string(i)+",";
        }
        res.pop_back();
    }
    return res;
}

//==========================================================================//



//===================================== struct operand2 =====================================//

string operand2::to_string() const
{
    string res;
    if(type==operand2_type::IMMED_8R)
    {
        res="#"+std::to_string(immed_8r);
    }
    else if(type==operand2_type::RM_SHIFT)
    {
        res=reg_ro_string(Rm_shift.Rm)+",";
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
            res+=(operand2_shift_Rs_op_output_map[Rm_shift.shift_op]+" "+reg_ro_string(Rm_shift.Rs));
        }
        else if(operand2_shift_neither_n_and_Rs_op_output_map.find(Rm_shift.shift_op)!=operand2_shift_neither_n_and_Rs_op_output_map.end())
        {
            res+=operand2_shift_neither_n_and_Rs_op_output_map[Rm_shift.shift_op];
        }
    }

    return res;
}

//==========================================================================//



//===================================== struct immed_16r =====================================//

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

//==========================================================================//



//===================================== struct flexoffset =====================================//

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
        res+=(reg_ro_string(this->Rm_shift.Rm)+",");
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

//==========================================================================//



//===================================== class Arm_asm_file_line =====================================//

void Arm_asm_file_line::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    Arm_instruction * ins;
    Arm_cpu_instruction * cpu_ins;
    Arm_cpu_data_process_instruction * cpu_data_process_ins;
    Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins;
    Arm_pseudo_instruction * pseudo_ins;
    if(is_instruction())
    {
        ins=dynamic_cast<Arm_instruction *>((Arm_asm_file_line *)this);
        if(ins->is_cpu_instruction())
        {
            cpu_ins=dynamic_cast<Arm_cpu_instruction *>(ins);
            if(cpu_ins->is_data_process_instruction())
            {
                cpu_data_process_ins=dynamic_cast<Arm_cpu_data_process_instruction *>(cpu_ins);
                cpu_data_process_ins->replace_regs(regs_map);
            }
            else if(cpu_ins->is_single_register_load_and_store_instruction())
            {
                cpu_single_register_load_and_store_ins=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(cpu_ins);
                cpu_single_register_load_and_store_ins->replace_regs(regs_map);
            }
            else
            {
                cpu_ins->replace_regs(regs_map);
            }
        }
        else
        {
            ins->replace_regs(regs_map);
        }
    }
    else if(is_pseudo_instruction())
    {
        pseudo_ins=dynamic_cast<Arm_pseudo_instruction *>((Arm_asm_file_line *)this);
        pseudo_ins->replace_regs(regs_map);
    }
}

// set<reg_index> Arm_asm_file_line::get_all_destination_regs(bool as_virtual_target_code) const
// {
//     set<reg_index> res;
//     Arm_instruction * ins;
//     Arm_cpu_instruction * cpu_ins;
//     Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins;
//     Arm_cpu_multiple_registers_load_and_store_instruction * cpu_multiple_registers_load_and_store_ins;
//     Arm_vfp_instruction * vfp_ins;
//     Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_ins;
//     Arm_vfp_multiple_registers_load_and_store_instruction * vfp_multiple_registers_load_and_store_ins;
//     Arm_pseudo_instruction * pseudo_ins;
//     list<reg_index> registers;
//     if(is_instruction())
//     {
//         ins=dynamic_cast<Arm_instruction *>((Arm_asm_file_line *)this);
//         if(as_virtual_target_code)
//         {
//             if(ins->is_cpu_instruction())
//             {
//                 cpu_ins=dynamic_cast<Arm_cpu_instruction *>(ins);
//                 if(cpu_ins->is_single_register_load_and_store_instruction())
//                 {
//                     cpu_single_register_load_and_store_ins=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(cpu_ins);
//                     if(cpu_single_register_load_and_store_ins->is_store())
//                     {
//                         return res;
//                     }
//                 }
//                 else if(cpu_ins->is_multiple_registers_load_and_store_instruction())
//                 {
//                     cpu_multiple_registers_load_and_store_ins=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(cpu_ins);
//                     if(cpu_multiple_registers_load_and_store_ins->is_store())
//                     {
//                         return res;
//                     }
//                 }
//             }
//             else if(ins->is_vfp_instruction())
//             {
//                 vfp_ins=dynamic_cast<Arm_vfp_instruction *>(ins);
//                 if(vfp_ins->is_single_register_load_and_store_instruction())
//                 {
//                     vfp_single_register_load_and_store_ins=dynamic_cast<Arm_vfp_single_register_load_and_store_instruction *>(vfp_ins);
//                     if(vfp_single_register_load_and_store_ins->is_store())
//                     {
//                         return res;
//                     }
//                 }
//                 else if(vfp_ins->is_multiple_registers_load_and_store_instruction())
//                 {
//                     vfp_multiple_registers_load_and_store_ins=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(vfp_ins);
//                     if(vfp_multiple_registers_load_and_store_ins->is_store())
//                     {
//                         return res;
//                     }
//                 }
//             }
//         }
//         for(auto reg:ins->get_destination_registers().registers_)
//         {
//             res.insert(reg);
//         }
//     }
//     else if(is_pseudo_instruction())
//     {
//         pseudo_ins=dynamic_cast<Arm_pseudo_instruction *>((Arm_asm_file_line *)this);
//         if(pseudo_ins->get_op()==arm_pseudo_op::ADR || 
//         pseudo_ins->get_op()==arm_pseudo_op::ADRL ||
//         pseudo_ins->get_op()==arm_pseudo_op::LDR)
//         {
//             res.insert(pseudo_ins->get_reg());
//         }
//     }
//     return res;
// }

// set<reg_index> Arm_asm_file_line::get_all_source_regs(bool as_virtual_target_code) const
// {
//     set<reg_index> res;
//     Arm_instruction * ins;
//     Arm_pseudo_instruction * pseudo_ins;
//     Arm_cpu_instruction * cpu_ins;
//     Arm_cpu_branch_instruction * cpu_branch_ins;
//     Arm_cpu_data_process_instruction * cpu_data_process_ins;
//     Arm_cpu_single_register_load_and_store_instruction * cpu_single_register_load_and_store_ins;
//     Arm_cpu_multiple_registers_load_and_store_instruction * cpu_multiple_registers_load_and_store_ins;
//     Arm_vfp_instruction * vfp_ins;
//     Arm_vfp_single_register_load_and_store_instruction * vfp_single_register_load_and_store_ins;
//     Arm_vfp_multiple_registers_load_and_store_instruction * vfp_multiple_registers_load_and_store_ins;
//     struct operand2 op2;
//     struct flexoffset fleoff;
//     if(is_instruction())
//     {
//         ins=dynamic_cast<Arm_instruction *>((Arm_asm_file_line *)this);
//         if(ins->is_cpu_instruction())
//         {
//             cpu_ins=dynamic_cast<Arm_cpu_instruction *>(ins);
//             if(cpu_ins->is_data_process_instruction())
//             {
//                 cpu_data_process_ins=dynamic_cast<Arm_cpu_data_process_instruction *>(cpu_ins);
//                 if(cpu_data_process_ins->get_data_type()==arm_data_process_instruction_data_type::OPERNAD2)
//                 {
//                     op2=cpu_data_process_ins->get_operand2();
//                     if(op2.type==operand2_type::RM_SHIFT)
//                     {
//                         res.insert(op2.Rm_shift.Rm);
//                         if(op2.Rm_shift.shift_op==operand2_shift_op::ASR_RS || 
//                         op2.Rm_shift.shift_op==operand2_shift_op::LSL_RS ||
//                         op2.Rm_shift.shift_op==operand2_shift_op::LSR_RS ||
//                         op2.Rm_shift.shift_op==operand2_shift_op::ROR_RS)
//                         {
//                             res.insert(op2.Rm_shift.Rs);
//                         }
//                     }
//                 }
//             }
//             else if(cpu_ins->is_single_register_load_and_store_instruction())
//             {
//                 cpu_single_register_load_and_store_ins=dynamic_cast<Arm_cpu_single_register_load_and_store_instruction *>(cpu_ins);
//                 fleoff=cpu_single_register_load_and_store_ins->get_flexoffset();
//                 if(fleoff.type==flexoffset_type::RM_SHIFT && (fleoff.Rm_shift.shift_op!=flexoffset_shift_op::NONE && fleoff.Rm_shift.shift_op!=flexoffset_shift_op::RRX))
//                 {
//                     res.insert(fleoff.Rm_shift.Rm);
//                 }
//                 if(as_virtual_target_code)
//                 {
//                     if(cpu_single_register_load_and_store_ins->is_store())
//                     {
//                         for(auto reg:ins->get_destination_registers().registers_)
//                         {
//                             res.insert(reg);
//                         }
//                     }
//                 }
//             }
//             else if(as_virtual_target_code && cpu_ins->is_multiple_registers_load_and_store_instruction())
//             {
//                 cpu_multiple_registers_load_and_store_ins=dynamic_cast<Arm_cpu_multiple_registers_load_and_store_instruction *>(cpu_ins);
//                 if(cpu_multiple_registers_load_and_store_ins->is_store())
//                 {
//                     for(auto reg:ins->get_destination_registers().registers_)
//                     {
//                         res.insert(reg);
//                     }
//                 }
//             }
//         }
//         else if(as_virtual_target_code && ins->is_vfp_instruction())
//         {
//             vfp_ins=dynamic_cast<Arm_vfp_instruction * >(ins);
//             if(vfp_ins->is_single_register_load_and_store_instruction())
//             {
//                 vfp_single_register_load_and_store_ins=dynamic_cast<Arm_vfp_single_register_load_and_store_instruction *>(vfp_ins);
//                 if(vfp_single_register_load_and_store_ins->is_store())
//                 {
//                     for(auto reg:ins->get_destination_registers().registers_)
//                     {
//                         res.insert(reg);
//                     }
//                 }
//             }
//             else if(vfp_ins->is_multiple_registers_load_and_store_instruction())
//             {
//                 vfp_multiple_registers_load_and_store_ins=dynamic_cast<Arm_vfp_multiple_registers_load_and_store_instruction *>(vfp_ins);
//                 if(vfp_multiple_registers_load_and_store_ins->is_store())
//                 {
//                     for(auto reg:ins->get_destination_registers().registers_)
//                     {
//                         res.insert(reg);
//                     }
//                 }
//             }
//         }
//         for(auto reg:ins->get_source_registers().registers_)
//         {
//             res.insert(reg);
//         }
//     }
//     return res;
// }

//==========================================================================//



//===================================== class Arm_directive =====================================//

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

//==========================================================================//



//===================================== class Arm_instruction =====================================//

void Arm_instruction::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    for(auto & reg:destination_registers_.registers_)
    {
        if(regs_map.find(reg)!=regs_map.end())
        {
            reg=regs_map.at(reg);
        }
    }
    for(auto & reg:source_registers_.registers_)
    {
        if(regs_map.find(reg)!=regs_map.end())
        {
            reg=regs_map.at(reg);
        }
    }
}

bool Arm_instruction::is_cpu_instruction() const
{
    return cpu_ops.find(op_)!=cpu_ops.end();
}

bool Arm_instruction::is_vfp_instruction() const
{
    return vfp_ops.find(op_)!=cpu_ops.end();
}

//==========================================================================//



//===================================== class Arm_pseudo_instruction =====================================//

void Arm_pseudo_instruction::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    if(op_==arm_pseudo_op::ADR || 
    op_==arm_pseudo_op::ADRL ||
    op_==arm_pseudo_op::LDR)
    {
        if(regs_map.find(reg_)!=regs_map.end())
        {
            reg_=regs_map.at(reg_);
        }
    }
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
            res+=(condition_output_map[cond_]+" "+reg_ro_string(reg_)+",");
            if(op_==arm_pseudo_op::LDR || op_==arm_pseudo_op::VLDR)
            {
                res+="=";
            }
            res+=expr_;
            break;
    }
    return res;
}

//==========================================================================//



//===================================== class Arm_vfp_instruction =====================================//

bool Arm_vfp_instruction::is_register_transfer_instruction() const
{
    return vfp_register_transfer_ops.find(op_)!=vfp_register_transfer_ops.end();
}

bool Arm_vfp_instruction::is_data_process_instruction() const
{
    return vfp_data_process_ops.find(op_)!=vfp_data_process_ops.end();
}

bool Arm_vfp_instruction::is_multiple_registers_load_and_store_instruction() const
{
    return vfp_multiple_registers_load_and_store_ops.find(op_)!=vfp_multiple_registers_load_and_store_ops.end();
}

bool Arm_vfp_instruction::is_single_register_load_and_store_instruction() const
{
    return vfp_single_register_load_and_store_ops.find(op_)!=vfp_single_register_load_and_store_ops.end();
}

//==========================================================================//



//===================================== class Arm_cpu_branch_instruction =====================================//

Arm_label Arm_cpu_branch_instruction::get_label() const
{
    return label_;
}

string Arm_cpu_branch_instruction::to_string() const
{
    return "\t"+op_output_map[op_]+condition_output_map[cond_]+"\t"+source_registers_.to_string()+label_.to_string(false);
}

//==========================================================================//



//===================================== class Arm_cpu_data_process_instruction =====================================//

void Arm_cpu_data_process_instruction::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    Arm_cpu_instruction::replace_regs(regs_map);
    if(data_type_==arm_data_process_instruction_data_type::OPERNAD2)
    {
        if(operand2_.type==operand2_type::RM_SHIFT)
        {
            if(regs_map.find(operand2_.Rm_shift.Rm)!=regs_map.end())
            {
                operand2_.Rm_shift.Rm=regs_map.at(operand2_.Rm_shift.Rm);
            }
            if(operand2_.Rm_shift.shift_op==operand2_shift_op::ASR_RS || 
            operand2_.Rm_shift.shift_op==operand2_shift_op::LSL_RS ||
            operand2_.Rm_shift.shift_op==operand2_shift_op::LSR_RS ||
            operand2_.Rm_shift.shift_op==operand2_shift_op::ROR_RS)
            {
                if(regs_map.find(operand2_.Rm_shift.Rs)!=regs_map.end())
                {
                    operand2_.Rm_shift.Rs=regs_map.at(operand2_.Rm_shift.Rs);
                }
            }
        }
    }
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

//==========================================================================//



//===================================== class Arm_cpu_multiple_registers_load_and_store_instruction =====================================//

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

//==========================================================================//



//===================================== class Arm_cpu_single_register_load_and_store_instruction =====================================//

void Arm_cpu_single_register_load_and_store_instruction::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    Arm_cpu_instruction::replace_regs(regs_map);
    if(flexoffset_.type==flexoffset_type::RM_SHIFT)
    {
        if(regs_map.find(flexoffset_.Rm_shift.Rm)!=regs_map.end())
        {
            flexoffset_.Rm_shift.Rm=regs_map.at(flexoffset_.Rm_shift.Rm);
        }
    }
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

//==========================================================================//



//===================================== class Arm_vfp_register_transfer_instruction =====================================//

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

//==========================================================================//



//===================================== class Arm_vfp_data_process_instruction =====================================//

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
            return res;
        }
    }
    res+=(","+source_registers_.to_string());

    return res;
}

//==========================================================================//



//===================================== class Arm_vfp_multiple_registers_load_and_store_instruction =====================================//

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

//==========================================================================//



//===================================== class Arm_vfp_single_register_load_and_store_instruction =====================================//

string Arm_vfp_single_register_load_and_store_instruction::to_string() const
{
    string res="\t"+op_output_map[op_]+condition_output_map[cond_]+precision_output_map[precision_]+"\t"+destination_registers_.to_string()+",";

    switch(single_register_load_and_store_type_)
    {
        case vfp_single_register_load_and_store_type::RN:
            res+=("["+source_registers_.to_string()+"]");
            break;
        case vfp_single_register_load_and_store_type::RN_OFFSET:
            res+=("["+source_registers_.to_string()+",#"+::to_string(offset_)+"]");
            break;
        case vfp_single_register_load_and_store_type::PRE_DECREMENT:
            res+=("["+source_registers_.to_string()+",#-"+::to_string(offset_)+"]!");
            break;
        case vfp_single_register_load_and_store_type::POST_INCREMENT:
            res+=("["+source_registers_.to_string()+"],#"+::to_string(offset_));
            break;
        case vfp_single_register_load_and_store_type::PROGRAM_RELATIVE:
            res+=label_;
            break;
        default:
            break;
    }

    return res;
}

//==========================================================================//



//===================================== class Arm_label =====================================//

bool Arm_label::is_func() const
{
    return (!name_.empty() && name_.at(0)!='.');
}

string Arm_label::to_string(bool is_define) const
{
    return name_+(is_define?":":"");
}

string Arm_label::to_string() const
{
    return name_+":";
}

//==========================================================================//



//===================================== class Arm_cpu_instruction =====================================//

bool Arm_cpu_instruction::is_branch_instruction() const
{
    return cpu_branch_ops.find(op_)!=cpu_branch_ops.end();
}

bool Arm_cpu_instruction::is_data_process_instruction() const
{
    return cpu_data_process_ops.find(op_)!=cpu_data_process_ops.end();
}

bool Arm_cpu_instruction::is_multiple_registers_load_and_store_instruction() const
{
    return cpu_multiple_registers_load_and_store_ops.find(op_)!=cpu_multiple_registers_load_and_store_ops.end();
}

bool Arm_cpu_instruction::is_single_register_load_and_store_instruction() const
{
    return cpu_single_register_load_and_store_ops.find(op_)!=cpu_single_register_load_and_store_ops.end();
}

void Arm_cpu_instruction::replace_regs(const map<reg_index,reg_index> & regs_map)
{
    Arm_instruction::replace_regs(regs_map);
}

//==========================================================================//



//===================================== struct arm_basic_block =====================================//

void arm_basic_block::set_sequential_next(struct arm_basic_block * next)
{
    sequential_next=next;
}

void arm_basic_block::set_jump_next(struct arm_basic_block * next)
{
    jump_next=next;
}

void arm_basic_block::add_arm_asm(Arm_asm_file_line * arm_asm)
{
    arm_sequence.push_back(arm_asm);
}

set<struct arm_basic_block * > arm_basic_block::get_successors() const
{
    set<struct arm_basic_block * > res;
    if(sequential_next)
    {
        res.insert(sequential_next);
    }
    if(jump_next)
    {
        res.insert(jump_next);
    }
    return res;
}

set<struct arm_basic_block * > arm_basic_block::get_precursors() const
{
    set<struct arm_basic_block * > res;
    // if(precursors.empty())
    // {
        for(auto basic_block:belong_arm_func_flow_graph->basic_blocks)
        {
            if(basic_block->sequential_next==this || basic_block->jump_next==this)
            {
                //precursors.insert(basic_block);
                res.insert(basic_block);
            }
        }
    // }
    // return precursors;
    return res;
}

list<string> arm_basic_block::to_string()
{
    list<string> res;
    for(auto i:arm_sequence)
    {
        res.push_back(i->to_string());
    }
    return res;
}

//==========================================================================//



//===================================== struct arm_func_flow_graph =====================================//

void arm_func_flow_graph::build_nexts_between_basic_blocks()
{
    map<struct arm_basic_block * ,struct ic_basic_block * > arm_to_ic_basic_block_map;
    map<struct ic_basic_block * ,struct arm_basic_block * > ic_to_arm_basic_block_map;
    list<struct arm_basic_block * >::iterator arm_bb=basic_blocks.begin();
    list<struct ic_basic_block * >::iterator ic_bb=function->basic_blocks.begin();
    while(arm_bb!=basic_blocks.end() && ic_bb!=function->basic_blocks.end())
    {
        arm_to_ic_basic_block_map.insert(make_pair(*arm_bb,*ic_bb));
        ic_to_arm_basic_block_map.insert(make_pair(*ic_bb,*arm_bb));
        arm_bb++;
        ic_bb++;
    }
    for(auto & bb:basic_blocks)
    {
        if(arm_to_ic_basic_block_map.at(bb)->jump_next)
        {
            bb->set_jump_next(ic_to_arm_basic_block_map.at(arm_to_ic_basic_block_map.at(bb)->jump_next));
        }
        if(arm_to_ic_basic_block_map.at(bb)->sequential_next)
        {
            bb->set_sequential_next(ic_to_arm_basic_block_map.at(arm_to_ic_basic_block_map.at(bb)->sequential_next));
        }
    }
}

void arm_func_flow_graph::add_arm_asm(Arm_asm_file_line * arm_asm,bool new_basic_block)
{
    if(new_basic_block)
    {
        current_arm_basic_block=new struct arm_basic_block(this);
        basic_blocks.push_back(current_arm_basic_block);
    }
    if(arm_asm)
    {
        current_arm_basic_block->add_arm_asm(arm_asm);
    }
}

size_t arm_func_flow_graph::get_line_num() const
{
    size_t res=0;
    for(auto basic_block:basic_blocks)
    {
        for(auto line:basic_block->arm_sequence)
        {
            res++;
        }
    }
    return res;
}

list<string> arm_func_flow_graph::to_string()
{
    string func_name=function->func->name;
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
}

//==========================================================================//



//===================================== struct arm_flow_graph =====================================//

void arm_flow_graph::add_arm_asm_to_func(Arm_asm_file_line * arm_asm,bool new_basic_block,struct ic_func_flow_graph * new_func)
{
    //static struct arm_func_flow_graph * current_arm_func_flow_graph=nullptr;
    if(new_func!=nullptr)
    {
        current_arm_func_flow_graph=new struct arm_func_flow_graph(new_func);
        func_flow_graphs.insert(current_arm_func_flow_graph);
    }
    if(current_arm_func_flow_graph)
    {
        current_arm_func_flow_graph->add_arm_asm(arm_asm,new_basic_block);
    }
}

void arm_flow_graph::add_arm_asm_to_global(Arm_asm_file_line * arm_asm)
{
    //global_basic_block.add_arm_asm(arm_asm);
    global_defines.push_back(arm_asm);
}

list<string> arm_flow_graph::to_string()
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
    for(auto i:func_flow_graphs)
    {
        tmp=i->to_string();
        res.splice(res.end(),tmp);
    }
    //tmp=global_basic_block.to_string();
    for(auto i:global_defines)
    {
        res.push_back(i->to_string());
    }
    res.splice(res.end(),tmp);
    res.push_back("\t.ident	\"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0\"");
    res.push_back("\t.section	.note.GNU-stack,\"\",%progbits");
    return res;
}

//==========================================================================//