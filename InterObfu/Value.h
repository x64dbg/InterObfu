#ifndef VALUE_H
#define VALUE_H

#include "Utils.h"

template<typename T>
struct Value
{
    T val;
    CompareFunction<Value<T>> compare;

    explicit Value(T val, CompareFunction<Value> compare)
        : val(val),
        compare(compare) { }

    explicit Value(T val)
        : Value(val, nullptr) { }

    explicit Value(CompareFunction<Value> compare)
        : Value(T(), compare) { }

    explicit Value()
        : Value(T()) { }

    bool operator==(const Value & other) const
    {
        if (compare)
            return compare(*this, other);
        if (other.compare)
            return other.compare(other, *this);

        return val == other.val;
    }

    OPNEQ(Value);
};

#endif //VALUE_H