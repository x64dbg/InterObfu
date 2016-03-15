#ifndef SEGMENT_H
#define SEGMENT_H

#include "Register.h"
#include "Utils.h"

struct Segment
{
    enum Segments
    {
        Invalid,
        Cs,
        Ss,
        Ds,
        Es,
        Fs,
        Gs
    };

    Segments seg;

    explicit Segment()
        : seg(Invalid) { }

    bool operator==(const Segment & other) const
    {
        return seg == other.seg;
    }

    OPNEQ(Segment);
};

#endif //SEGMENT_H