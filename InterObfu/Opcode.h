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

    bool Equals(const Opcode & other, State & state) const
    {
        if(compare)
            return compare(*this, other, state);
        if(other.compare)
            return other.compare(other, *this, state);

        return mnem == other.mnem;
    }

    bool operator==(const Opcode & other) const = delete;

    OPNEQ(Opcode);

    std::string ToString() const;
};