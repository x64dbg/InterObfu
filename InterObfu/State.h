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

        const T & operator()() const
        {
            return val;
        }

        bool Equals(const T & other, State & state) const
        {
            return val.Equals(other, state);
        }

        bool operator==(const T & other) const = delete;

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

    static CompareFunction<Operand> MakeOperandN(int index)
    {
        return [index](const Operand & aThis, const Operand & bOther, State & state)
        {
            if(state.operands[index]) //already matched before
                return state.operands[index].Equals(bOther, state);
            state.operands[index] = bOther;
            return true;
        };
    }

    static CompareFunction<Register> MakeRegisterN(int index)
    {
        return [index](const Register & aThis, const Register & bOther, State & state)
        {
            if(state.registers[index]) //already matched before
                return state.registers[index].Equals(bOther, state);
            state.registers[index] = bOther;
            return true;
        };
    }

    static CompareFunction<Register> MakeRegisterSize(int bitsize)
    {
        return [bitsize](const Register & aThis, const Register & bOther, State & state)
        {
            return bOther.Size() * 8 == bitsize;
        };
    }

    static CompareFunction<Memory> MakeMemoryN(int index)
    {
        return [index](const Memory & aThis, const Memory & bOther, State & state)
        {
            if(state.memorys[index]) //already matched before
                return state.memorys[index].Equals(bOther, state);
            state.memorys[index] = bOther;
            return true;
        };
    }

    static CompareFunction<Value> MakeValueN(int index)
    {
        return [index](const Value & aThis, const Value & bOther, State & state)
        {
            if(state.values[index]) //already matched before
                return state.values[index].Equals(bOther, state);
            state.values[index] = bOther;
            return true;
        };
    }

    static CompareFunction<Opcode> MakeOpcodeN(int index)
    {
        return [index](const Opcode & aThis, const Opcode & bOther, State & state)
        {
            if(state.opcodes[index]) //already matched before
                return state.opcodes[index].Equals(bOther, state);
            state.opcodes[index] = bOther;
            return true;
        };
    }

    static CompareFunction<Opcode> MakeOpcodeList(int opIndex, const std::vector<Opcode::Mnemonics> & possible)
    {
        return [opIndex, possible](const Opcode & aThis, const Opcode & bOther, State & state)
        {
            if(!state.opcodes[opIndex]) //didn't match yet
            {
                for(auto & mnem : possible)
                {
                    if(mnem == bOther.mnem)
                    {
                        state.opcodes[opIndex] = bOther;
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
