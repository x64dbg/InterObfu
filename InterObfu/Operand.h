#ifndef OPERAND_H
#define OPERAND_H

#include "Utils.h"
#include "Register.h"
#include "Immediate.h"
#include "Floating.h"
#include "Memory.h"

struct Operand
{
    enum Types
    {
        Invalid,
        Reg,
        Imm,
        Fpu,
        Mem
    };

    Types type;
    Register reg;
    Immediate imm;
    Floating fpu;
    Memory mem;

    explicit Operand()
        : type(Invalid) { }

    explicit Operand(const Register & reg)
        : type(Reg),
        reg(reg) { }

    explicit Operand(Register::Registers reg)
        : Operand(Register(reg)) { }

    explicit Operand(const Immediate & imm)
        : type(Imm),
        imm(imm) { }

    explicit Operand(const Floating & fpu)
        : type(Fpu),
        fpu(fpu) { }

    explicit Operand(const Memory & mem)
        : type(Mem),
        mem(mem) { }

    bool operator==(const Operand & other) const
    {
        if (type != other.type)
            return false;
        switch (type)
        {
        case Invalid:
            return true;
        case Reg:
            return reg == other.reg;
        case Imm:
            return imm == other.imm;
        case Fpu:
            return fpu == other.fpu;
        case Mem:
            return mem == other.mem;
        default:
            return false;
        }
    }

    OPNEQ(Operand);
};

#endif //OPERAND_H