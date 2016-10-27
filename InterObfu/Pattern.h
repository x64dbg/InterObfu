#pragma once

#include "Instruction.h"
#include "State.h"
#include "Assemble.h"

struct Pattern
{
    State state;
    std::vector<Instruction> instrs;

    //This function append the replacement instructions to 'out'.
    //It should use state to replace the correct registers etc.
    bool Produce(std::vector<Instruction> & out) const
    {
        //TODO: properly implement with wildcard replacements
        return true;
    }

    int64_t Search(const std::vector<Instruction> & data, size_t start = 0)
    {
        state.Clear();
        if(instrs.size() > data.size() - start)
            return -1;
        for(size_t i = start, pos = 0; i < data.size(); i++)
        {
            if(data[i].Equals(instrs[pos]))
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

    bool Match(const std::vector<Instruction> & data, size_t start)
    {
        state.Clear();
        if(instrs.size() > data.size() - start)
            return false;
        for(size_t i = start, pos = 0; i < data.size(); i++)
        {
            if(data[i].Equals(instrs[pos]))
            {
                pos++;
                if(pos == instrs.size())
                    return true;
            }
            else
                break;
        }
        return false;
    }

    void Add(const Instruction & instr)
    {
        instrs.push_back(instr);
    }

    void Add(const std::string & instr, int mode = 32, uint64_t addr = 0)
    {
        instrs.push_back(Assemble(instr, mode, addr));
    }
};
