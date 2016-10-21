#include "Instruction.h"
#include <cstdio>

struct WildcardImm : Value
{
    explicit WildcardImm()
        : Value([](const Value & a, const Value & b)
    {
        return true;
    }) { }
};

int main()
{
    const int PUSH = 1;
    const int MOV = 2;
    //representation of "push 0x300"
    auto push300 = Instruction(PUSH, Operand(Value(0x300)));

    //representation of "push imm"
    auto pushImm = Instruction(PUSH, Operand(WildcardImm()));

    Instruction push300_;
    push300_.opcode.mnem = PUSH;
    push300_.opCount = 1;
    push300_.operands[0].type = Operand::Imm;
    push300_.operands[0].imm.val = 0x300;

    printf("equal: %d %d %d\n", push300 == push300_, push300 == pushImm, pushImm == push300);

    //representation of "mov eax, ebx"
    Instruction movReg;
    movReg.opcode.mnem = MOV;
    movReg.opCount = 2;
    movReg.operands[0].type = Operand::Reg;
    movReg.operands[0].reg.reg = Register::EAX;
    movReg.operands[1].type = Operand::Reg;
    movReg.operands[1].reg.reg = Register::EBX;

    //representation of "mov [ebx + 0x401000], 0x3"
    Instruction movMem;
    movMem.opcode.mnem = MOV;
    movMem.opCount = 2;
    movMem.operands[0].type = Operand::Mem;
    movMem.operands[0].mem.base.reg = Register::EBX;
    movMem.operands[0].mem.disp.val = 0x401000;
    movMem.operands[1].type = Operand::Imm;
    movMem.operands[1].imm.val = 0x3;

    struct State
    {
        Register::Registers registers[4];
        struct ValState
        {
            bool set = false;
            int64_t value = 0;

            explicit ValState() { }

            explicit ValState(int64_t value)
                : set(true), value(value) { }
        };
        ValState values[4];
        std::string memorys[4];
        Opcode::Mnemonics opcodes[4];

        void clear()
        {
            memset(registers, 0, sizeof(registers));
            memset(values, 0, sizeof(values));
        }

        CompareFunction<Register> makeRegisterN(int regIndex)
        {
            return [this, regIndex](const Register & aThis, const Register & bOther)
            {
                if(registers[regIndex] == Register::INVALID) //didnt match yet
                {
                    registers[regIndex] = bOther.reg;
                    return true;
                }
                else
                {
                    return registers[regIndex] == bOther.reg;
                }
            };
        }

        CompareFunction<Memory> makeMemoryN(int memIndex)
        {
            return [this, memIndex](const Memory & aThis, const Memory & bOther)
            {
                char memhax[256] = "";
                sprintf_s(memhax, "%d+%d*%lld+%lld", bOther.base.reg, bOther.index.reg, bOther.scale.val, bOther.disp.val);
                if(memorys[memIndex].empty()) //didnt match yet
                {
                    memorys[memIndex].assign(memhax);
                    return true;
                }
                else
                {
                    return memorys[memIndex] == memhax;
                }
            };
        }

        CompareFunction<Value> makeValueN(int valIndex)
        {
            return [this, valIndex](const Value & aThis, const Value & bOther)
            {
                if(!values[valIndex].set) //didnt match yet
                {
                    values[valIndex] = ValState(bOther.val);
                    return true;
                }
                else
                {
                    return values[valIndex].value == bOther.val;
                }
            };
        }

        CompareFunction<Opcode> makeOpcodeNList(int opIndex, const std::vector<Opcode::Mnemonics> & possible)
        {
            return[this, opIndex, possible](const Opcode & aThis, const Opcode & bOther)
            {   
                if(!opcodes[opIndex]) //didn't match yet
                {
                    for(auto & mnem : possible)
                    {
                        if(mnem == bOther.mnem)
                        {
                            opcodes[opIndex] = bOther.mnem;
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

    struct Pattern
    {
        State state;
        std::vector<Instruction> insns;
    } pattern1;


    Instruction i;
    //insn = "push"
    i.opcode.mnem = PUSH;

    //first_reg = "reg2"
    i.opCount++;
    i.operands[0] = Operand(Register(pattern1.state.makeRegisterN(2)));

    //mov memory1, 0x3
    movMem.operands[0] = Operand(Memory(pattern1.state.makeMemoryN(1)));

    Pattern pattern2;

    //mov [base1 + index2 * 1 + 0], imm1
    
    Instruction h = Instruction(MOV, 2);
    auto base1 = Register(pattern2.state.makeRegisterN(1));
    auto index2 = Register(pattern2.state.makeRegisterN(2));
    h.operands[0] = Operand(Memory(base1, index2, Value(1), Value(0)));
    h.operands[1] = Operand(Value(pattern2.state.makeValueN(1)));

    //xor eax, ecx
    

    getchar();

    return 0;
}