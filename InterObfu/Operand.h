#pragma once

#include "Utils.h"
#include "Register.h"
#include "Value.h"
#include "Memory.h"

struct Operand
{
    enum Types
    {
        Invalid,
        Reg,
        Imm,
        Mem
    };

    Types type;
    Register reg;
    Value imm;
    Memory mem;

    uint8_t access = 0;
    uint8_t size = 0;

    CompareFunction<Operand> compare = nullptr;

    explicit Operand()
        : type(Invalid) { }

    explicit Operand(CompareFunction<Operand> compare)
        : type(Invalid), compare(compare) { }

    explicit Operand(const Register & reg)
        : type(Reg),
          reg(reg) { }

    explicit Operand(Register::Registers reg)
        : Operand(Register(reg)) { }

    explicit Operand(const Value & imm)
        : type(Imm),
          imm(imm) { }

    explicit Operand(const Memory & mem)
        : type(Mem),
          mem(mem) { }

    bool operator==(const Operand & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        if(type != other.type)
            return false;
        switch(type)
        {
        case Invalid:
            return true;
        case Reg:
            return reg == other.reg;
        case Imm:
            return imm == other.imm;
        case Mem:
            return mem == other.mem;
        default:
            return false;
        }
    }

    OPNEQ(Operand);
};