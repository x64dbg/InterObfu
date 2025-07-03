#include "test_framework.h"
#include "Instruction.h"
#include "State.h"

// Mock the X86_INS constants since we don't have capstone headers in tests
#define X86_INS_NOP 1
#define X86_INS_MOV 2
#define X86_INS_PUSH 3
#define X86_INS_POP 4
#define X86_INS_ADD 5
#define X86_INS_SUB 6
#define X86_INS_XOR 7
#define X86_INS_INVALID 0

TEST(Instruction_DefaultConstruction)
{
    Instruction instr;
    ASSERT_EQ(0, instr.opCount);
    ASSERT_EQ(0, instr.address);
    ASSERT_EQ(0, instr.size);
    ASSERT_EQ(0, instr.eflags);
}

TEST(Instruction_MnemonicConstruction)
{
    Instruction instr(X86_INS_MOV);
    ASSERT_EQ(X86_INS_MOV, instr.opcode.mnem);
    ASSERT_EQ(0, instr.opCount);
}

TEST(Instruction_MnemonicWithOperandCountConstruction)
{
    Instruction instr(X86_INS_MOV, 2);
    ASSERT_EQ(X86_INS_MOV, instr.opcode.mnem);
    ASSERT_EQ(2, instr.opCount);
}

TEST(Instruction_SingleOperandConstruction)
{
    Operand op(Register::EAX);
    Instruction instr(X86_INS_PUSH, op);
    
    ASSERT_EQ(X86_INS_PUSH, instr.opcode.mnem);
    ASSERT_EQ(1, instr.opCount);
    ASSERT_EQ(Operand::Reg, instr.operands[0].type);
    ASSERT_EQ(Register::EAX, instr.operands[0].reg.reg);
}

TEST(Instruction_TwoOperandConstruction)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    Instruction instr(X86_INS_MOV, op1, op2);
    
    ASSERT_EQ(X86_INS_MOV, instr.opcode.mnem);
    ASSERT_EQ(2, instr.opCount);
    ASSERT_EQ(Operand::Reg, instr.operands[0].type);
    ASSERT_EQ(Operand::Reg, instr.operands[1].type);
    ASSERT_EQ(Register::EAX, instr.operands[0].reg.reg);
    ASSERT_EQ(Register::EBX, instr.operands[1].reg.reg);
}

TEST(Instruction_ThreeOperandConstruction)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    Operand op3(Value(42));
    Instruction instr(X86_INS_ADD, op1, op2, op3);
    
    ASSERT_EQ(X86_INS_ADD, instr.opcode.mnem);
    ASSERT_EQ(3, instr.opCount);
    ASSERT_EQ(Register::EAX, instr.operands[0].reg.reg);
    ASSERT_EQ(Register::EBX, instr.operands[1].reg.reg);
    ASSERT_EQ(42, instr.operands[2].imm.val);
}

TEST(Instruction_FourOperandConstruction)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    Operand op3(Value(42));
    Operand op4(Memory(Value(0x1000)));
    Instruction instr(X86_INS_ADD, op1, op2, op3, op4);
    
    ASSERT_EQ(X86_INS_ADD, instr.opcode.mnem);
    ASSERT_EQ(4, instr.opCount);
    ASSERT_EQ(Register::EAX, instr.operands[0].reg.reg);
    ASSERT_EQ(Register::EBX, instr.operands[1].reg.reg);
    ASSERT_EQ(42, instr.operands[2].imm.val);
    ASSERT_EQ(0x1000, instr.operands[3].mem.disp.val);
}

TEST(Instruction_OperandAccess)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    Instruction instr(X86_INS_MOV, op1, op2);
    
    ASSERT_EQ(Register::EAX, instr[0].reg.reg);
    ASSERT_EQ(Register::EBX, instr[1].reg.reg);
    
    // Test const access
    const Instruction& constInstr = instr;
    ASSERT_EQ(Register::EAX, constInstr[0].reg.reg);
    ASSERT_EQ(Register::EBX, constInstr[1].reg.reg);
}

TEST(Instruction_BasicEquality)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    
    Instruction instr1(X86_INS_MOV, op1, op2);
    Instruction instr2(X86_INS_MOV, op1, op2);
    Instruction instr3(X86_INS_PUSH, op1);
    
    State state;
    ASSERT_TRUE(instr1.Equals(instr2, state));
    ASSERT_FALSE(instr1.Equals(instr3, state));
}

TEST(Instruction_DifferentMnemonicFails)
{
    Operand op(Register::EAX);
    
    Instruction instr1(X86_INS_PUSH, op);
    Instruction instr2(X86_INS_POP, op);
    
    State state;
    ASSERT_FALSE(instr1.Equals(instr2, state));
}

TEST(Instruction_DifferentOperandCountFails)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    
    Instruction instr1(X86_INS_PUSH, op1);
    Instruction instr2(X86_INS_MOV, op1, op2);
    
    State state;
    ASSERT_FALSE(instr1.Equals(instr2, state));
}

TEST(Instruction_WildcardMatching)
{
    // Pattern: mov reg1, reg2
    Operand regWild1(Register(State::MakeRegisterN(1)));
    Operand regWild2(Register(State::MakeRegisterN(2)));
    Instruction pattern(X86_INS_MOV, regWild1, regWild2);
    
    // Concrete: mov eax, ebx
    Operand regEAX(Register::EAX);
    Operand regEBX(Register::EBX);
    Instruction concrete(X86_INS_MOV, regEAX, regEBX);
    
    State state;
    ASSERT_TRUE(pattern.Equals(concrete, state));
    
    // Verify bindings
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, state.registers[2]().reg);
}

TEST(Instruction_SameWildcardConstraint)
{
    // Pattern: mov reg1, reg1 (same register)
    Operand regWild1(Register(State::MakeRegisterN(1)));
    Operand regWild1_same(Register(State::MakeRegisterN(1)));
    Instruction patternSame(X86_INS_MOV, regWild1, regWild1_same);
    
    // Concrete: mov eax, eax
    Operand regEAX1(Register::EAX);
    Operand regEAX2(Register::EAX);
    Instruction concreteSame(X86_INS_MOV, regEAX1, regEAX2);
    
    // Concrete: mov eax, ebx  
    Operand regEBX(Register::EBX);
    Instruction concreteDiff(X86_INS_MOV, regEAX1, regEBX);
    
    State state1, state2;
    ASSERT_TRUE(patternSame.Equals(concreteSame, state1));  // Same register should match
    ASSERT_FALSE(patternSame.Equals(concreteDiff, state2)); // Different registers should fail
}

TEST(Instruction_ComplexMemoryMatching)
{
    // Pattern: mov [reg1 + imm1], reg2
    Memory memPattern(
        Register(State::MakeRegisterN(1)),
        Register(),
        Value(),
        Value(State::MakeValueN(1))
    );
    Operand memOp(memPattern);
    Operand regOp(Register(State::MakeRegisterN(2)));
    Instruction pattern(X86_INS_MOV, memOp, regOp);
    
    // Concrete: mov [eax + 0x10], ebx
    Memory memConcrete(Register(Register::EAX), Register(), Value(), Value(0x10));
    Operand memOpConcrete(memConcrete);
    Operand regOpConcrete(Register::EBX);
    Instruction concrete(X86_INS_MOV, memOpConcrete, regOpConcrete);
    
    State state;
    ASSERT_TRUE(pattern.Equals(concrete, state));
    
    // Verify bindings
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_TRUE(state.values[1]);
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, state.registers[2]().reg);
    ASSERT_EQ(0x10, state.values[1]().val);
}

TEST(Instruction_CustomCompareFunction)
{
    auto alwaysMatch = [](const Instruction& a, const Instruction& b, State& state) -> bool {
        return true;
    };
    
    Instruction instrWild(alwaysMatch);
    Instruction instr1(X86_INS_MOV, Operand(Register::EAX), Operand(Register::EBX));
    Instruction instr2(X86_INS_PUSH, Operand(Value(42)));
    
    State state;
    ASSERT_TRUE(instrWild.Equals(instr1, state));
    ASSERT_TRUE(instrWild.Equals(instr2, state));
    ASSERT_TRUE(instr1.Equals(instrWild, state)); // Test reverse direction
}

TEST(Instruction_RegisterMetadata)
{
    Instruction instr(X86_INS_ADD);
    
    // Test register read/write tracking
    instr.regs_read.push_back(Register(Register::EAX));
    instr.regs_read.push_back(Register(Register::EBX));
    instr.regs_written.push_back(Register(Register::EAX));
    
    ASSERT_EQ(2, instr.regs_read.size());
    ASSERT_EQ(1, instr.regs_written.size());
    ASSERT_EQ(Register::EAX, instr.regs_read[0].reg);
    ASSERT_EQ(Register::EBX, instr.regs_read[1].reg);
    ASSERT_EQ(Register::EAX, instr.regs_written[0].reg);
}

TEST(Instruction_AddressAndSizeMetadata)
{
    Instruction instr(X86_INS_MOV);
    instr.address = 0x401000;
    instr.size = 5;
    instr.eflags = 0x80;
    
    ASSERT_EQ(0x401000, instr.address);
    ASSERT_EQ(5, instr.size);
    ASSERT_EQ(0x80, instr.eflags);
}

TEST(Instruction_BytesAndPrefixMetadata)
{
    Instruction instr(X86_INS_MOV);
    
    // Test instruction bytes
    instr.bytes[0] = 0x89;
    instr.bytes[1] = 0xC3;
    
    ASSERT_EQ(0x89, instr.bytes[0]);
    ASSERT_EQ(0xC3, instr.bytes[1]);
    
    // Test prefix bytes
    instr.prefix[0] = 0x66; // Operand size override
    instr.prefix[1] = 0x67; // Address size override
    
    ASSERT_EQ(0x66, instr.prefix[0]);
    ASSERT_EQ(0x67, instr.prefix[1]);
}

TEST(Instruction_GroupsMetadata)
{
    Instruction instr(X86_INS_ADD);
    
    instr.groups.push_back(1);  // Example group
    instr.groups.push_back(5);  // Another group
    
    ASSERT_EQ(2, instr.groups.size());
    ASSERT_EQ(1, instr.groups[0]);
    ASSERT_EQ(5, instr.groups[1]);
}