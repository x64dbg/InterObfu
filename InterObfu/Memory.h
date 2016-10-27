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

    bool Equals(const Memory & other, State & state) const
    {
        if(compare)
            return compare(*this, other, state);
        if(other.compare)
            return other.compare(other, *this, state);

        return seg.Equals(other.seg, state) &&
               base.Equals(other.base, state) &&
               index.Equals(other.index, state) &&
               scale.Equals(other.scale, state) &&
               disp.Equals(other.disp, state);
    }

    bool operator==(const Memory & other) const = delete;

    OPNEQ(Memory);
};