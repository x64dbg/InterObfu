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

    std::vector<Register> regs_read;
    std::vector<Register> regs_written;
    std::vector<uint8_t> groups;
    uint8_t prefix[4];
    uint64_t address = 0;
    uint16_t size = 0;
    uint8_t bytes[16];
    uint64_t eflags = 0;

    CompareFunction<Instruction> compare = nullptr;

    explicit Instruction()
        : opCount(0) { }

    explicit Instruction(CompareFunction<Instruction> compare)
        : opCount(0), compare(compare) { }

    explicit Instruction(Opcode::Mnemonics mnem, int opCount = 0)
        : opcode(mnem),
          opCount(opCount) { }

    explicit Instruction(Opcode::Mnemonics mnem, Operand op1, Operand op2, Operand op3, Operand op4)
        : Instruction(mnem, 4)
    {
        operands[0] = op1;
        operands[1] = op2;
        operands[2] = op3;
        operands[3] = op4;
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
        if(index >= _countof(operands) || index >= size_t(opCount))
            __debugbreak();
#endif //_DEBUG
        return operands[index];
    }

    const Operand & operator[](size_t index) const
    {
#ifdef _DEBUG
        if(index >= _countof(operands) || index >= size_t(opCount))
            __debugbreak();
#endif //_DEBUG
        return operands[index];
    }

    bool Equals(const Instruction & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        if(!opcode.Equals(other.opcode))
            return false;

        if(opCount != other.opCount)
            return false;
        for(auto i = 0; i < opCount; i++)
            if(!operands[i].Equals(other.operands[i]))
                return false;
        return true;
    }

    bool operator==(const Instruction & other) const = delete;

    OPNEQ(Instruction);
};
