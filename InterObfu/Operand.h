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

    Value size;
    uint8_t access = 0;

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

    bool Equals(const Operand & other, State & state) const
    {
        if(compare)
            return compare(*this, other, state);
        if(other.compare)
            return other.compare(other, *this, state);

        if(type != other.type)
            return false;
        switch(type)
        {
        case Invalid:
            return true;
        case Reg:
            return reg.Equals(other.reg, state);
        case Imm:
            return imm.Equals(other.imm, state);
        case Mem:
            return mem.Equals(other.mem, state);
        default:
            return false;
        }
    }

    bool operator==(const Operand & other) const = delete;

    OPNEQ(Operand);
};