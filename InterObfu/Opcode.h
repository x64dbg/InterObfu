#ifndef OPCODE_H
#define OPCODE_H

#include "Utils.h"

struct Opcode
{
    enum Mnemonics //temporary
    {
        Invalid,
        Push,
        Pop,
        Mov,
        Sub,
        Add,
        Jmp,
        Call,
        Ret
    };

    Mnemonics mnem;

    explicit Opcode()
        : mnem(Invalid) { }

    explicit Opcode(Mnemonics mnem)
        : mnem(mnem) { }

    bool operator==(const Opcode & other) const
    {
        return mnem == other.mnem;
    }

    OPNEQ(Opcode);
};

#endif //OPCODE_H