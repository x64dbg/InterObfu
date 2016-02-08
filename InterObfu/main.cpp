#include "Instruction.h"

int main()
{
    Instruction instr;
    memset(&instr, 0, sizeof(instr));

    //representation of "push 0x300"
    instr.opcode.mnem = Opcode::Push;
    instr.operands[0].type = Operand::Imm;
    instr.operands[0].imm.val = 0x300;

    return 0;
}