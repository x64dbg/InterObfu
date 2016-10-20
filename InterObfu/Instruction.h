#pragma once

#include "Utils.h"
#include "Opcode.h"
#include "Value.h"
#include "Operand.h"

struct Instruction
{
    Opcode opcode;

    int opCount;
    Operand operands[4];

    explicit Instruction()
        : opCount(0) { }

    explicit Instruction(Opcode::Mnemonics mnem, int opCount = 0)
        : opcode(mnem),
        opCount(opCount) { }

    explicit Instruction(Opcode::Mnemonics mnem, Operand op1, Operand op2, Operand op3, Operand op4)
        : Instruction(mnem, 4)
    {
        operands[0] = op1;
        operands[1] = op2;
        operands[2] = op3;
        operands[4] = op4;
    }

    explicit Instruction(Opcode::Mnemonics mnem, Operand op1, Operand op2, Operand op3)
        : Instruction(mnem, 3)
    {
        operands[0] = op1;
        operands[1] = op2;
        operands[2] = op3;
    }

    explicit Instruction(Opcode::Mnemonics mnem, Operand op1, Operand op2)
        : Instruction(mnem, 2)
    {
        operands[0] = op1;
        operands[1] = op2;
    }

    explicit Instruction(Opcode::Mnemonics mnem, Operand op1)
        : Instruction(mnem, 1)
    {
        operands[0] = op1;
    }

    Operand & operator[](size_t index)
    {
#ifdef _DEBUG
        if (index >= _countof(operands))
            __debugbreak();
#endif //_DEBUG
        return operands[index];
    }

    const Operand & operator[](size_t index) const
    {
#ifdef _DEBUG
        if (index >= _countof(operands))
            __debugbreak();
#endif //_DEBUG
        return operands[index];
    }

    bool operator==(const Instruction & other) const
    {
        if (opCount != other.opCount)
            return false;
        for (auto i = 0; i < opCount; i++)
            if (operands[i] != other.operands[i])
                return false;
        return true;
    }

    OPNEQ(Instruction);
};