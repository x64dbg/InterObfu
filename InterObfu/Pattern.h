#pragma once

#include "Instruction.h"
#include "State.h"

struct Pattern
{
    State state;
    std::vector<Instruction> instrs;

    int64_t Match(const std::vector<Instruction> & data, size_t start = 0)
    {
        state.Clear();
        if(instrs.size() > data.size() - start)
            return -1;
        for(size_t i = start, pos = 0; i < data.size(); i++)
        {
            if(data[i] == instrs[pos])
            {
                pos++;
                if(pos == instrs.size())
                    return i - instrs.size() + 1;
            }
            else
            {
                i -= pos;
                pos = 0; //reset current pattern position
                state.Clear(); //clear the state
            }
        }
        return -1;
    }

    void Add(const Instruction & instr)
    {
        instrs.push_back(instr);
    }
};
