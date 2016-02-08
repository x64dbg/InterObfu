#ifndef MEMORY_H
#define MEMORY_H

#include "Segment.h"
#include "Immediate.h"

struct Memory
{
    Segment seg;
    Register base;
    Register index;
    Immediate scale;
    Immediate disp;
};

#endif //MEMORY_H