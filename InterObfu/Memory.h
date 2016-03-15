#ifndef MEMORY_H
#define MEMORY_H

#include "Utils.h"
#include "Segment.h"
#include "Immediate.h"

struct Memory
{
    Segment seg;
    Register base;
    Register index;
    Immediate scale; //TODO: is this the right way?
    Immediate disp;

    explicit Memory() { }

    explicit Memory(Segment seg, Register base, Register index, Immediate scale, Immediate disp)
        : seg(seg),
        base(base),
        index(index),
        scale(scale),
        disp(disp) { }

    explicit Memory(Register base, Register index, Immediate scale, Immediate disp)
        : Memory(Segment(), base, index, scale, disp) { }

    explicit Memory(Immediate disp)
        : Memory(Register(), Register(), Immediate(), disp) { }

    bool operator==(const Memory & other) const
    {
        return seg == seg &&
            base == base &&
            index == index &&
            scale == scale &&
            disp == disp;
    }

    OPNEQ(Memory);
};

#endif //MEMORY_H