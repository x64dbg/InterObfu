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

    explicit Memory() { }

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

    bool operator==(const Memory & other) const
    {
        return seg == other.seg &&
            base == other.base &&
            index == other.index &&
            scale == other.scale &&
            disp == other.disp;
    }

    OPNEQ(Memory);
};