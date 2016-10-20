#pragma once

#include "Utils.h"

struct Opcode
{
    using Mnemonics = int;

    Mnemonics mnem;

    explicit Opcode()
        : mnem(0) { }

    explicit Opcode(Mnemonics mnem)
        : mnem(mnem) { }

    bool operator==(const Opcode & other) const
    {
        return mnem == other.mnem;
    }

    OPNEQ(Opcode);
};