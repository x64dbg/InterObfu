#pragma once

#include <stdint.h>
#include "Utils.h"

struct Value
{
    int64_t val;
    CompareFunction<Value> compare;

    explicit Value(int64_t val, CompareFunction<Value> compare)
        : val(val),
          compare(compare) { }

    explicit Value(int64_t val)
        : Value(val, nullptr) { }

    explicit Value(CompareFunction<Value> compare)
        : Value(0, compare) { }

    explicit Value()
        : Value(0) { }

    bool operator==(const Value & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        return val == other.val;
    }

    OPNEQ(Value);
};