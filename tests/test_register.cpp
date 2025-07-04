#include "test_framework.h"
#include "Register.h"
#include "State.h"

TEST(Register_DefaultConstruction)
{
    Register r;
    ASSERT_EQ(Register::INVALID, r.reg);
}

TEST(Register_EnumConstruction)
{
    Register r(Register::EAX);
    ASSERT_EQ(Register::EAX, r.reg);
}

TEST(Register_Equality)
{
    Register r1(Register::EAX);
    Register r2(Register::EAX);
    Register r3(Register::EBX);
    
    State state;
    ASSERT_TRUE(r1.Equals(r2, state));
    ASSERT_FALSE(r1.Equals(r3, state));
}

TEST(Register_8BitSizes)
{
    ASSERT_EQ(1, Register(Register::AL).Size());
    ASSERT_EQ(1, Register(Register::AH).Size());
    ASSERT_EQ(1, Register(Register::BL).Size());
    ASSERT_EQ(1, Register(Register::BH).Size());
    ASSERT_EQ(1, Register(Register::CL).Size());
    ASSERT_EQ(1, Register(Register::CH).Size());
    ASSERT_EQ(1, Register(Register::DL).Size());
    ASSERT_EQ(1, Register(Register::DH).Size());
    ASSERT_EQ(1, Register(Register::R8B).Size());
    ASSERT_EQ(1, Register(Register::R15B).Size());
}

TEST(Register_16BitSizes)
{
    ASSERT_EQ(2, Register(Register::AX).Size());
    ASSERT_EQ(2, Register(Register::BX).Size());
    ASSERT_EQ(2, Register(Register::CX).Size());
    ASSERT_EQ(2, Register(Register::DX).Size());
    ASSERT_EQ(2, Register(Register::SI).Size());
    ASSERT_EQ(2, Register(Register::DI).Size());
    ASSERT_EQ(2, Register(Register::BP).Size());
    ASSERT_EQ(2, Register(Register::SP).Size());
    ASSERT_EQ(2, Register(Register::R8W).Size());
    ASSERT_EQ(2, Register(Register::R15W).Size());
}

TEST(Register_32BitSizes)
{
    ASSERT_EQ(4, Register(Register::EAX).Size());
    ASSERT_EQ(4, Register(Register::EBX).Size());
    ASSERT_EQ(4, Register(Register::ECX).Size());
    ASSERT_EQ(4, Register(Register::EDX).Size());
    ASSERT_EQ(4, Register(Register::ESI).Size());
    ASSERT_EQ(4, Register(Register::EDI).Size());
    ASSERT_EQ(4, Register(Register::EBP).Size());
    ASSERT_EQ(4, Register(Register::ESP).Size());
    ASSERT_EQ(4, Register(Register::R8D).Size());
    ASSERT_EQ(4, Register(Register::R15D).Size());
}

TEST(Register_64BitSizes)
{
    ASSERT_EQ(8, Register(Register::RAX).Size());
    ASSERT_EQ(8, Register(Register::RBX).Size());
    ASSERT_EQ(8, Register(Register::RCX).Size());
    ASSERT_EQ(8, Register(Register::RDX).Size());
    ASSERT_EQ(8, Register(Register::RSI).Size());
    ASSERT_EQ(8, Register(Register::RDI).Size());
    ASSERT_EQ(8, Register(Register::RBP).Size());
    ASSERT_EQ(8, Register(Register::RSP).Size());
    ASSERT_EQ(8, Register(Register::R8).Size());
    ASSERT_EQ(8, Register(Register::R15).Size());
}

TEST(Register_HighByteOffset)
{
    ASSERT_EQ(1, Register(Register::AH).Offset());
    ASSERT_EQ(1, Register(Register::BH).Offset());
    ASSERT_EQ(1, Register(Register::CH).Offset());
    ASSERT_EQ(1, Register(Register::DH).Offset());
    
    // Low bytes and other registers should have offset 0
    ASSERT_EQ(0, Register(Register::AL).Offset());
    ASSERT_EQ(0, Register(Register::BL).Offset());
    ASSERT_EQ(0, Register(Register::EAX).Offset());
    ASSERT_EQ(0, Register(Register::RAX).Offset());
}

TEST(Register_StateWildcards)
{
    Register r1(State::MakeRegisterN(1));
    Register r2(Register::EAX);
    Register r3(Register::EBX);
    
    State state;
    
    // First comparison should succeed and bind register
    ASSERT_TRUE(r1.Equals(r2, state));
    ASSERT_TRUE(state.registers[1]);
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    
    // Second comparison with same wildcard should only succeed if same register
    ASSERT_TRUE(r1.Equals(r2, state));   // Same register
    ASSERT_FALSE(r1.Equals(r3, state));  // Different register
}

TEST(Register_SizeBasedMatching)
{
    Register r32bit(State::MakeRegisterSize(32));
    Register r64bit(State::MakeRegisterSize(64));
    
    Register eax(Register::EAX);  // 32-bit
    Register rax(Register::RAX);  // 64-bit
    Register ax(Register::AX);    // 16-bit
    
    State state1, state2;
    
    ASSERT_TRUE(r32bit.Equals(eax, state1));
    ASSERT_FALSE(r32bit.Equals(rax, state1));
    ASSERT_FALSE(r32bit.Equals(ax, state1));
    
    ASSERT_TRUE(r64bit.Equals(rax, state2));
    ASSERT_FALSE(r64bit.Equals(eax, state2));
    ASSERT_FALSE(r64bit.Equals(ax, state2));
}

TEST(Register_WildcardMatching)
{
    Register rwild(State::RegWild());
    Register r1(Register::EAX);
    Register r2(Register::BL);
    Register r3(Register::XMM0);
    
    State state;
    
    // Wildcard should match any register
    ASSERT_TRUE(rwild.Equals(r1, state));
    ASSERT_TRUE(rwild.Equals(r2, state));
    ASSERT_TRUE(rwild.Equals(r3, state));
}

TEST(Register_MultipleWildcards)
{
    Register r1(State::MakeRegisterN(1));
    Register r2(State::MakeRegisterN(2));
    Register r3(State::MakeRegisterN(1)); // Same as r1
    
    Register eax(Register::EAX);
    Register ebx(Register::EBX);
    
    State state;
    
    // Bind different wildcards to different registers
    ASSERT_TRUE(r1.Equals(eax, state));
    ASSERT_TRUE(r2.Equals(ebx, state));
    
    // r3 should match eax (same wildcard as r1)
    ASSERT_TRUE(r3.Equals(eax, state));
    ASSERT_FALSE(r3.Equals(ebx, state));
}