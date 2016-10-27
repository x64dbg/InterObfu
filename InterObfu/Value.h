#pragma once

#include <stdint.h>
#include "Utils.h"

struct Value
{
    int64_t val;
    CompareFunction<Value> compare = nullptr;

    explicit Value()
        : val(0) { }

    explicit Value(CompareFunction<Value> compare)
        : val(0), compare(compare) { }

    explicit Value(int64_t val)
        : val(val) { }

    bool Equals(const Value & other, State & state) const
    {
        if(compare)
            return compare(*this, other, state);
        if(other.compare)
            return other.compare(other, *this, state);

        return val == other.val;
    }

    bool operator==(const Value & other) const = delete;

    OPNEQ(Value);

    std::string ToString() const;
};