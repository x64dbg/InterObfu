#ifndef OPERAND_H
#define OPERAND_H

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
    union
    {
        Register reg;
        Immediate imm;
        Floating fpu;
        Memory mem;
    };
};

#endif //OPERAND_H