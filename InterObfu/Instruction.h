#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstring>

#include "Opcode.h"
#include "Value.h"
#include "Operand.h"

struct Instruction
{
    Instruction()
    {
        memset(this, 0, sizeof(Instruction));
    }

    Opcode opcode;

    Value<int> opCount;
    Operand operands[4];
};

#endif //INSTRUCTION_H