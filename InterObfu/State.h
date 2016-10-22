#pragma once

#include "Opcode.h"
#include "Memory.h"
#include "Value.h"
#include "Register.h"

struct State
{
    template<typename T>
    struct StateValue
    {
        explicit StateValue()
            :set(false) { }

        explicit StateValue(const T & value)
            : set(true), val(value) { }

        operator bool() const
        {
            return set;
        }

        T & operator=(const T & value)
        {
            set = true;
            return val = value;
        }

        T & operator()()
        {
            return val;
        }

        bool operator==(const T & other) const
        {
            return val == other;
        }

        OPNEQ(T);

    private:
        bool set;
        T val;
    };

    static const int MaxStateCount = 4;

    StateValue<Register> registers[MaxStateCount];
    StateValue<Value> values[MaxStateCount];
    StateValue<Memory> memorys[MaxStateCount];
    StateValue<Opcode> opcodes[MaxStateCount];

    explicit State()
    {
        Clear();
    }

    void Clear()
    {
        for(int i = 0; i < MaxStateCount; i++)
        {
            registers[i] = StateValue<Register>();
            values[i] = StateValue<Value>();
            memorys[i] = StateValue<Memory>();
            opcodes[i] = StateValue<Opcode>();
        }
    }

    CompareFunction<Register> MakeRegisterN(int regIndex)
    {
        return [this, regIndex](const Register & aThis, const Register & bOther)
        {
            if(!registers[regIndex]) //didn't match yet
            {
                registers[regIndex] = bOther;
                return true;
            }
            else
                return registers[regIndex] == bOther;
        };
    }

    CompareFunction<Memory> MakeMemoryN(int memIndex)
    {
        return [this, memIndex](const Memory & aThis, const Memory & bOther)
        {
            if(!memorys[memIndex]) //didnt match yet
            {
                memorys[memIndex] = bOther;
                return true;
            }
            else
            {
                return memorys[memIndex] == bOther;
            }
        };
    }

    CompareFunction<Value> MakeValueN(int valIndex)
    {
        return [this, valIndex](const Value & aThis, const Value & bOther)
        {
            if(!values[valIndex]) //didnt match yet
            {
                values[valIndex] = bOther;
                return true;
            }
            else
            {
                return values[valIndex] == bOther;
            }
        };
    }

    CompareFunction<Opcode> MakeOpcodeList(int opIndex, const std::vector<Opcode::Mnemonics> & possible)
    {
        return [this, opIndex, possible](const Opcode & aThis, const Opcode & bOther)
        {
            if(!opcodes[opIndex]) //didn't match yet
            {
                for(auto & mnem : possible)
                {
                    if(mnem == bOther.mnem)
                    {
                        opcodes[opIndex] = bOther;
                        return true;
                    }
                }
                return false;
            }
            else //matched before
                return true;
        };
    }
};
