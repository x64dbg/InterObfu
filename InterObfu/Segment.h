#pragma once

#include "Register.h"
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

    explicit Segment()
        : seg(Invalid) { }

    bool operator==(const Segment & other) const
    {
        return seg == other.seg;
    }

    OPNEQ(Segment);
};