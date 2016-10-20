#pragma once

#include "Utils.h"

struct Opcode
{
    using Mnemonics = int;

    Mnemonics mnem;

    CompareFunction<Opcode> compare = nullptr;

    explicit Opcode()
        : mnem(0) { }

    explicit Opcode(CompareFunction<Opcode> compare)
        : mnem(0), compare(compare) { }

    explicit Opcode(Mnemonics mnem)
        : mnem(mnem) { }

    bool operator==(const Opcode & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        return mnem == other.mnem;
    }

    OPNEQ(Opcode);
};