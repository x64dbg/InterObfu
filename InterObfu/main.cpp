#include "Instruction.h"

int main()
{
    //representation of "push 0x300"
    Instruction push300;
    push300.opcode.mnem = Opcode::Push;
    push300.opCount.val = 1;
    push300.operands[0].type = Operand::Imm;
    push300.operands[0].imm.val = 0x300;

    //representation of "mov eax, ebx"
    Instruction movReg;
    movReg.opcode.mnem = Opcode::Mov;
    movReg.opCount.val = 2;
    movReg.operands[0].type = Operand::Reg;
    movReg.operands[0].reg.reg = Register::Eax;
    movReg.operands[1].type = Operand::Reg;
    movReg.operands[1].reg.reg = Register::Ebx;

    //representation of "mov [ebx + 0x401000], 0x3"
    Instruction movMem;
    movMem.opcode.mnem = Opcode::Mov;
    movMem.opCount.val = 2;
    movMem.operands[0].type = Operand::Mem;
    movMem.operands[0].mem.base.reg = Register::Ebx;
    movMem.operands[0].mem.disp.val = 0x401000;
    movMem.operands[1].type = Operand::Imm;
    movMem.operands[1].imm.val = 0x3;

    return 0;
}