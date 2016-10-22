#include "Instruction.h"
#include <cstdio>
#include "State.h"
#include "Pattern.h"

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
    const int XOR = 3;

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

    Pattern pattern1;
    //push reg2
    auto pushReg = Instruction(PUSH, Operand(Register(pattern1.state.MakeRegisterN(2))));
    pattern1.instrs.push_back(pushReg);
    //mov [memory1], 0x3
    auto movMemImm1 = Instruction(MOV, Operand(Memory(pattern1.state.MakeMemoryN(1))), Operand(Value(3)));
    pattern1.instrs.push_back(movMemImm1);

    Pattern pattern2;
    //mov [base1 + index2 * 1 + 0], imm1
    auto base1 = Register(pattern2.state.MakeRegisterN(1));
    auto index2 = Register(pattern2.state.MakeRegisterN(2));
    auto movMemImm2 = Instruction(MOV, Operand(Memory(base1, index2, Value(1), Value(0))), Operand(Value(pattern2.state.MakeValueN(1))));
    pattern2.instrs.push_back(movMemImm2);
    //xor reg1, reg2
    auto xorRegReg = Instruction(XOR, Operand(Register(pattern2.state.MakeRegisterN(1))), Operand(Register(pattern2.state.MakeRegisterN(2))));
    pattern2.instrs.push_back(xorRegReg);

    getchar();

    return 0;
}