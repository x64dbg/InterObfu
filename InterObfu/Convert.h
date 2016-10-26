#pragma once

#include "Instruction.h"
#include "capstone_wrapper/capstone_wrapper.h"
#include <unordered_map>

class Converter
{
    Converter() = delete;

    static std::unordered_map<std::string, Register::Registers> mstr2reg;
    static std::unordered_map<Register::Registers, std::string> mreg2str;
    static std::unordered_map<std::string, Opcode::Mnemonics> mstr2ins;
    static std::unordered_map<Opcode::Mnemonics, std::string> mins2str;

    template<typename K, typename V>
    static V k2v(const std::unordered_map<K, V> & m, const K & k)
    {
        auto found = m.find(k);
        return found == m.end() ? V() : found->second;
    }

public:
    static void init();
    static Register::Registers str2reg(const std::string & str);
    static std::string reg2str(Register::Registers reg);
    static Opcode::Mnemonics str2ins(const std::string & str);
    static std::string ins2str(Opcode::Mnemonics ins);
};

inline Register::Registers convertReg(x86_reg reg)
{
    if(int(X86_REG_R15W) != int(Register::R15W))
        __debugbreak();
    return Register::Registers(reg);
}

inline Segment::Segments convertSeg(x86_reg reg)
{
    switch(reg)
    {
    case X86_REG_INVALID:
        return Segment::INVALID;
    case X86_REG_CS:
        return Segment::CS;
    case X86_REG_SS:
        return Segment::SS;
    case X86_REG_DS:
        return Segment::DS;
    case X86_REG_ES:
        return Segment::ES;
    case X86_REG_FS:
        return Segment::FS;
    case X86_REG_GS:
        return Segment::GS;
    default:
        __debugbreak();
    }
    return Segment::INVALID;
}

inline Instruction FromCapstone(const cs_insn* insn)
{
    auto instr = Instruction(insn->id);
    auto detail = insn->detail;
    for(auto i = 0; i < detail->regs_read_count; i++)
        instr.regs_read.push_back(Register(convertReg(x86_reg(detail->regs_read[i]))));
    for(auto i = 0; i < detail->regs_write_count; i++)
        instr.regs_written.push_back(Register(convertReg(x86_reg(detail->regs_write[i]))));
    for(auto i = 0; i < detail->groups_count; i++)
        instr.groups.push_back(detail->groups[i]);
    const auto & x86 = detail->x86;
    memcpy(instr.prefix, x86.prefix, sizeof(instr.prefix));
    instr.address = insn->address;
    instr.size = insn->size;
    memcpy(instr.bytes, insn->bytes, sizeof(instr.bytes));
    instr.eflags = x86.eflags;
    instr.opCount = x86.op_count;
    for(auto i = 0; i < instr.opCount; i++)
    {
        const auto & op = x86.operands[i];
        switch(op.type)
        {
        case X86_OP_REG:
            instr.operands[i] = Operand(Register(convertReg(op.reg)));
            break;
        case X86_OP_IMM:
            instr.operands[i] = Operand(Value(op.imm));
            break;
        case X86_OP_MEM:
            instr.operands[i] = Operand(Memory(
                                            Segment(convertSeg(op.mem.segment)),
                                            Register(convertReg(op.mem.base)),
                                            Register(convertReg(op.mem.index)),
                                            Value(op.mem.scale),
                                            Value(op.mem.disp)));
            break;
        default:
            __debugbreak();
        }
        instr.operands[i].size = Value(op.size);
        instr.operands[i].access = op.access;
    }
    return instr;
}
