#include "test_framework.h"
#include "Operand.h"
#include "State.h"

TEST(Operand_DefaultConstruction)
{
    Operand op;
    ASSERT_EQ(Operand::Invalid, op.type);
}

TEST(Operand_RegisterConstruction)
{
    Register reg(Register::EAX);
    Operand op(reg);
    
    ASSERT_EQ(Operand::Reg, op.type);
    ASSERT_EQ(Register::EAX, op.reg.reg);
}

TEST(Operand_RegisterEnumConstruction)
{
    Operand op(Register::EBX);
    
    ASSERT_EQ(Operand::Reg, op.type);
    ASSERT_EQ(Register::EBX, op.reg.reg);
}

TEST(Operand_ImmediateConstruction)
{
    Value imm(0x1234);
    Operand op(imm);
    
    ASSERT_EQ(Operand::Imm, op.type);
    ASSERT_EQ(0x1234, op.imm.val);
}

TEST(Operand_MemoryConstruction)
{
    Memory mem(Register(Register::EBP), Register(), Value(), Value(0x8));
    Operand op(mem);
    
    ASSERT_EQ(Operand::Mem, op.type);
    ASSERT_EQ(Register::EBP, op.mem.base.reg);
    ASSERT_EQ(0x8, op.mem.disp.val);
}

TEST(Operand_InvalidEquality)
{
    Operand op1;
    Operand op2;
    
    State state;
    ASSERT_TRUE(op1.Equals(op2, state));
}

TEST(Operand_RegisterEquality)
{
    Operand op1(Register::EAX);
    Operand op2(Register::EAX);
    Operand op3(Register::EBX);
    
    State state;
    ASSERT_TRUE(op1.Equals(op2, state));
    ASSERT_FALSE(op1.Equals(op3, state));
}

TEST(Operand_ImmediateEquality)
{
    Operand op1(Value(42));
    Operand op2(Value(42));
    Operand op3(Value(43));
    
    State state;
    ASSERT_TRUE(op1.Equals(op2, state));
    ASSERT_FALSE(op1.Equals(op3, state));
}

TEST(Operand_MemoryEquality)
{
    Memory mem1(Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem2(Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem3(Register(Register::EBX), Register(), Value(), Value(0x10));
    
    Operand op1(mem1);
    Operand op2(mem2);
    Operand op3(mem3);
    
    State state;
    ASSERT_TRUE(op1.Equals(op2, state));
    ASSERT_FALSE(op1.Equals(op3, state));
}

TEST(Operand_TypeMismatchFails)
{
    Operand regOp(Register::EAX);
    Operand immOp(Value(42));
    Operand memOp(Memory(Value(0x1000)));
    
    State state;
    ASSERT_FALSE(regOp.Equals(immOp, state));
    ASSERT_FALSE(regOp.Equals(memOp, state));
    ASSERT_FALSE(immOp.Equals(memOp, state));
}

TEST(Operand_StateWildcards)
{
    Operand opWild(State::MakeOperandN(1));
    Operand opReg(Register::EAX);
    Operand opImm(Value(42));
    Operand opMem(Memory(Value(0x1000)));
    
    State state1, state2, state3;
    
    // Wildcard should match any operand type
    ASSERT_TRUE(opWild.Equals(opReg, state1));
    ASSERT_TRUE(state1.operands[1]);
    
    ASSERT_TRUE(opWild.Equals(opImm, state2));
    ASSERT_TRUE(state2.operands[1]);
    
    ASSERT_TRUE(opWild.Equals(opMem, state3));
    ASSERT_TRUE(state3.operands[1]);
}

TEST(Operand_WildcardBinding)
{
    Operand opWild(State::MakeOperandN(1));
    Operand op1(Register::EAX);
    Operand op2(Register::EBX);
    
    State state;
    
    // First match should bind the wildcard
    ASSERT_TRUE(opWild.Equals(op1, state));
    ASSERT_TRUE(state.operands[1]);
    
    // Subsequent matches should only succeed if same operand
    ASSERT_TRUE(opWild.Equals(op1, state));   // Same operand
    ASSERT_FALSE(opWild.Equals(op2, state));  // Different operand
}

TEST(Operand_ComplexMemoryOperand)
{
    // [ebx + esi*4 + 0x100]
    Memory complexMem(
        Register::EBX,
        Register::ESI,
        Value(4),
        Value(0x100)
    );
    
    Operand op1(complexMem);
    Operand op2(complexMem);
    
    State state;
    ASSERT_TRUE(op1.Equals(op2, state));
}

TEST(Operand_WildcardMemoryOperand)
{
    // Pattern: [reg1 + reg2*2 + imm1]
    Memory patternMem(
        Register(State::MakeRegisterN(1)),
        Register(State::MakeRegisterN(2)),
        Value(2),
        Value(State::MakeValueN(1))
    );
    
    // Concrete: [eax + ebx*2 + 0x20]
    Memory concreteMem(
        Register::EAX,
        Register::EBX,
        Value(2),
        Value(0x20)
    );
    
    Operand opPattern(patternMem);
    Operand opConcrete(concreteMem);
    
    State state;
    ASSERT_TRUE(opPattern.Equals(opConcrete, state));
    
    // Verify the wildcards were bound correctly
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_TRUE(state.values[1]);
    
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, state.registers[2]().reg);
    ASSERT_EQ(0x20, state.values[1]().val);
}

TEST(Operand_CustomCompareFunction)
{
    auto alwaysMatch = [](const Operand& a, const Operand& b, State& state) -> bool {
        return true;
    };
    
    Operand opWild(alwaysMatch);
    Operand opReg(Register::EAX);
    Operand opImm(Value(42));
    Operand opMem(Memory(Value(0x1000)));
    
    State state;
    
    // Custom compare function should always return true
    ASSERT_TRUE(opWild.Equals(opReg, state));
    ASSERT_TRUE(opWild.Equals(opImm, state));
    ASSERT_TRUE(opWild.Equals(opMem, state));
}

TEST(Operand_SizeAndAccessAttributes)
{
    Operand op(Register::EAX);
    op.size = Value(4);  // 32-bit
    op.access = 3;       // Read | Write
    
    ASSERT_EQ(4, op.size.val);
    ASSERT_EQ(3, op.access);
}