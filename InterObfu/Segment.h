#pragma once

#include "Utils.h"

struct Segment
{
    enum Segments
    {
        Invalid,
        CS,
        SS,
        DS,
        ES,
        FS,
        GS
    };

    Segments seg;

    CompareFunction<Segment> compare = nullptr;

    explicit Segment()
        : seg(Invalid) { }

    explicit Segment(CompareFunction<Segment> compare)
        : seg(Invalid), compare(compare) { }

    bool operator==(const Segment & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        return seg == other.seg;
    }

    OPNEQ(Segment);
};