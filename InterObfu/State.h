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
            : set(false) { }

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

    StateValue<Operand> operands[MaxStateCount];
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
        for(auto i = 0; i < MaxStateCount; i++)
        {
            operands[i] = StateValue<Operand>();
            registers[i] = StateValue<Register>();
            values[i] = StateValue<Value>();
            memorys[i] = StateValue<Memory>();
            opcodes[i] = StateValue<Opcode>();
        }
    }

    template<typename T>
    static CompareFunction<T> MakeTN(StateValue<T>(&state)[MaxStateCount], int index)
    {
        return [&state, index](const T & aThis, const T & bOther)
        {
            if(state[index]) //already matched before
                return state[index] == bOther;
            state[index] = bOther;
            return true;
        };
    }

    CompareFunction<Operand> MakeOperandN(int opIndex)
    {
        return MakeTN(operands, opIndex);
    }

    CompareFunction<Register> MakeRegisterN(int regIndex)
    {
        return MakeTN(registers, regIndex);
    }

    CompareFunction<Register> MakeRegisterSize(int bitsize)
    {
        return [bitsize](const Register & aThis, const Register & bOther)
        {
            return bOther.Size() * 8 == bitsize;
        };
    }

    CompareFunction<Memory> MakeMemoryN(int memIndex)
    {
        return MakeTN(memorys, memIndex);
    }

    CompareFunction<Value> MakeValueN(int valIndex)
    {
        return MakeTN(values, valIndex);
    }

    CompareFunction<Opcode> MakeOpcodeN(int opIndex)
    {
        return MakeTN(opcodes, opIndex);
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
