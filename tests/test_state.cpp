#include "test_framework.h"
#include "State.h"
#include "Operand.h"
#include "Register.h"
#include "Memory.h"
#include "Value.h"

TEST(State_DefaultConstruction)
{
    State state;
    
    // All state values should be unset initially
    for(int i = 0; i < State::MaxStateCount; i++)
    {
        ASSERT_FALSE(state.operands[i]);
        ASSERT_FALSE(state.registers[i]);
        ASSERT_FALSE(state.values[i]);
        ASSERT_FALSE(state.memorys[i]);
        ASSERT_FALSE(state.opcodes[i]);
    }
}

TEST(State_Clear)
{
    State state;
    
    // Set some values
    state.registers[0] = Register(Register::EAX);
    state.values[1] = Value(42);
    
    ASSERT_TRUE(state.registers[0]);
    ASSERT_TRUE(state.values[1]);
    
    // Clear should reset everything
    state.Clear();
    
    for(int i = 0; i < State::MaxStateCount; i++)
    {
        ASSERT_FALSE(state.operands[i]);
        ASSERT_FALSE(state.registers[i]);
        ASSERT_FALSE(state.values[i]);
        ASSERT_FALSE(state.memorys[i]);
        ASSERT_FALSE(state.opcodes[i]);
    }
}

TEST(State_RegisterWildcards)
{
    auto reg1Func = State::MakeRegisterN(1);
    auto reg2Func = State::MakeRegisterN(2);
    
    Register r1(reg1Func);
    Register r2(reg2Func);
    Register eax(Register::EAX);
    Register ebx(Register::EBX);
    
    State state;
    
    // First matches should bind the wildcards
    ASSERT_TRUE(r1.Equals(eax, state));
    ASSERT_TRUE(r2.Equals(ebx, state));
    
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, state.registers[2]().reg);
    
    // Subsequent matches should only succeed with same registers
    ASSERT_TRUE(r1.Equals(eax, state));   // Same register
    ASSERT_FALSE(r1.Equals(ebx, state));  // Different register
    ASSERT_TRUE(r2.Equals(ebx, state));   // Same register
    ASSERT_FALSE(r2.Equals(eax, state));  // Different register
}

TEST(State_ValueWildcards)
{
    auto val1Func = State::MakeValueN(1);
    auto val2Func = State::MakeValueN(2);
    
    Value v1(val1Func);
    Value v2(val2Func);
    Value val42(42);
    Value val100(100);
    
    State state;
    
    // First matches should bind the wildcards
    ASSERT_TRUE(v1.Equals(val42, state));
    ASSERT_TRUE(v2.Equals(val100, state));
    
    ASSERT_TRUE(state.values[1]);
    ASSERT_TRUE(state.values[2]);
    ASSERT_EQ(42, state.values[1]().val);
    ASSERT_EQ(100, state.values[2]().val);
    
    // Subsequent matches should only succeed with same values
    ASSERT_TRUE(v1.Equals(val42, state));   // Same value
    ASSERT_FALSE(v1.Equals(val100, state)); // Different value
    ASSERT_TRUE(v2.Equals(val100, state));  // Same value
    ASSERT_FALSE(v2.Equals(val42, state));  // Different value
}

TEST(State_MemoryWildcards)
{
    auto mem1Func = State::MakeMemoryN(1);
    auto mem2Func = State::MakeMemoryN(2);
    
    Memory m1(mem1Func);
    Memory m2(mem2Func);
    Memory memEAX(Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory memEBX(Register(Register::EBX), Register(), Value(), Value(0x20));
    
    State state;
    
    // First matches should bind the wildcards
    ASSERT_TRUE(m1.Equals(memEAX, state));
    ASSERT_TRUE(m2.Equals(memEBX, state));
    
    ASSERT_TRUE(state.memorys[1]);
    ASSERT_TRUE(state.memorys[2]);
    
    // Subsequent matches should only succeed with same memory references
    ASSERT_TRUE(m1.Equals(memEAX, state));   // Same memory
    ASSERT_FALSE(m1.Equals(memEBX, state));  // Different memory
    ASSERT_TRUE(m2.Equals(memEBX, state));   // Same memory
    ASSERT_FALSE(m2.Equals(memEAX, state));  // Different memory
}

TEST(State_OperandWildcards)
{
    auto op1Func = State::MakeOperandN(1);
    auto op2Func = State::MakeOperandN(2);
    
    Operand o1(op1Func);
    Operand o2(op2Func);
    Operand opReg(Register::EAX);
    Operand opImm(Value(42));
    
    State state;
    
    // First matches should bind the wildcards
    ASSERT_TRUE(o1.Equals(opReg, state));
    ASSERT_TRUE(o2.Equals(opImm, state));
    
    ASSERT_TRUE(state.operands[1]);
    ASSERT_TRUE(state.operands[2]);
    
    // Subsequent matches should only succeed with same operands
    ASSERT_TRUE(o1.Equals(opReg, state));   // Same operand
    ASSERT_FALSE(o1.Equals(opImm, state));  // Different operand
    ASSERT_TRUE(o2.Equals(opImm, state));   // Same operand
    ASSERT_FALSE(o2.Equals(opReg, state));  // Different operand
}

TEST(State_RegisterSizeMatching)
{
    auto reg32 = State::MakeRegisterSize(32);
    auto reg64 = State::MakeRegisterSize(64);
    auto reg16 = State::MakeRegisterSize(16);
    auto reg8 = State::MakeRegisterSize(8);
    
    Register r32(reg32);
    Register r64(reg64);
    Register r16(reg16);
    Register r8(reg8);
    
    Register eax(Register::EAX);  // 32-bit
    Register rax(Register::RAX);  // 64-bit
    Register ax(Register::AX);    // 16-bit
    Register al(Register::AL);    // 8-bit
    
    State state;
    
    // Size-based matching
    ASSERT_TRUE(r32.Equals(eax, state));
    ASSERT_FALSE(r32.Equals(rax, state));
    ASSERT_FALSE(r32.Equals(ax, state));
    ASSERT_FALSE(r32.Equals(al, state));
    
    ASSERT_TRUE(r64.Equals(rax, state));
    ASSERT_FALSE(r64.Equals(eax, state));
    
    ASSERT_TRUE(r16.Equals(ax, state));
    ASSERT_FALSE(r16.Equals(eax, state));
    
    ASSERT_TRUE(r8.Equals(al, state));
    ASSERT_FALSE(r8.Equals(eax, state));
}

TEST(State_WildcardFunctions)
{
    // Test wildcard functions
    auto segWild = State::SegWild();
    auto regWild = State::RegWild();
    auto valWild = State::ValWild();
    
    Segment s1(segWild);
    Register r1(regWild);
    Value v1(valWild);
    
    Segment segDS(Segment::DS);
    Segment segES(Segment::ES);
    Register regEAX(Register::EAX);
    Register regEBX(Register::EBX);
    Value val1(100);
    Value val2(200);
    
    State state;
    
    // Wildcards should match anything
    ASSERT_TRUE(s1.Equals(segDS, state));
    ASSERT_TRUE(s1.Equals(segES, state));
    ASSERT_TRUE(r1.Equals(regEAX, state));
    ASSERT_TRUE(r1.Equals(regEBX, state));
    ASSERT_TRUE(v1.Equals(val1, state));
    ASSERT_TRUE(v1.Equals(val2, state));
}

TEST(State_MaxStateCount)
{
    // Verify the maximum state count
    ASSERT_EQ(4, State::MaxStateCount);
    
    State state;
    
    // Test that we can use all available slots
    state.registers[0] = Register(Register::EAX);
    state.registers[1] = Register(Register::EBX);
    state.registers[2] = Register(Register::ECX);
    state.registers[3] = Register(Register::EDX);
    
    ASSERT_TRUE(state.registers[0]);
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_TRUE(state.registers[3]);
    
    ASSERT_EQ(Register::EAX, state.registers[0]().reg);
    ASSERT_EQ(Register::EBX, state.registers[1]().reg);
    ASSERT_EQ(Register::ECX, state.registers[2]().reg);
    ASSERT_EQ(Register::EDX, state.registers[3]().reg);
}

TEST(State_CrossTypeWildcards)
{
    // Test using wildcards across different types in a complex pattern
    State state;
    
    // Create a pattern that uses multiple wildcard types
    auto regFunc = State::MakeRegisterN(1);
    auto valFunc = State::MakeValueN(1);
    auto memFunc = State::MakeMemoryN(1);
    
    Register reg(regFunc);
    Value val(valFunc);
    Memory mem(memFunc);
    
    Register concreteReg(Register::EAX);
    Value concreteVal(0x100);
    Memory concreteMem(Register(Register::EBX), Register(), Value(), Value(0x20));
    
    // Bind each wildcard
    ASSERT_TRUE(reg.Equals(concreteReg, state));
    ASSERT_TRUE(val.Equals(concreteVal, state));
    ASSERT_TRUE(mem.Equals(concreteMem, state));
    
    // Verify all are bound
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.values[1]);
    ASSERT_TRUE(state.memorys[1]);
    
    // Verify bindings are correct
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(0x100, state.values[1]().val);
    ASSERT_EQ(Register::EBX, state.memorys[1]().base.reg);
}