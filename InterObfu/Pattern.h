#pragma once

#include "Instruction.h"
#include "State.h"

struct Pattern
{
    State state;
    std::vector<Instruction> instrs;
};
