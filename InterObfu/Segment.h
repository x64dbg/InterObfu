#pragma once

#include "Utils.h"

struct Segment
{
    enum Segments
    {
        INVALID,
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
        : seg(INVALID) { }

    explicit Segment(Segments seg)
        : seg(seg) { }

    explicit Segment(CompareFunction<Segment> compare)
        : seg(INVALID), compare(compare) { }

    bool Equals(const Segment & other) const
    {
        if(compare)
            return compare(*this, other);
        if(other.compare)
            return other.compare(other, *this);

        return seg == other.seg;
    }

    bool operator==(const Segment & other) const = delete;

    OPNEQ(Segment);
};