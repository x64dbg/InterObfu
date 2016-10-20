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