#ifndef SEGMENT_H
#define SEGMENT_H

#include "Register.h"

struct Segment
{
    enum Segments
    {
        Cs,
        Ss,
        Ds,
        Es,
        Fs,
        Gs
    };
};

#endif //SEGMENT_H