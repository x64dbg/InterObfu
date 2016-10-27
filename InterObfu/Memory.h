#pragma once

#include "Utils.h"
#include "Segment.h"

struct Memory
{
    Segment seg;
    Register base;
    Register index;
    Value scale; //TODO: is this the right way?
    Value disp;

    CompareFunction<Memory> compare = nullptr;

    explicit Memory() { }

    explicit Memory(CompareFunction<Memory> compare)
        : compare(compare) { }

    explicit Memory(Segment seg, Register base, Register index, Value scale, Value disp)
        : seg(seg),
          base(base),
          index(index),
          scale(scale),
          disp(disp) { }

    explicit Memory(Register base, Register index, Value scale, Value disp)
        : Memory(Segment(), base, index, scale, disp) { }

    explicit Memory(Value disp)
        : Memory(Register(), Register(), Value(), disp) { }

    bool Equals(const Memory & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        return seg.Equals(other.seg) &&
               base.Equals(other.base) &&
               index.Equals(other.index) &&
               scale.Equals(other.scale) &&
               disp.Equals(other.disp);
    }

    bool operator==(const Memory & other) const = delete;

    OPNEQ(Memory);
};