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

    getchar();

    return 0;
}