#include "test_framework.h"
#include "Memory.h"
#include "State.h"

TEST(Memory_DefaultConstruction)
{
    Memory mem;
    
    // Default construction should initialize all components to default values
    ASSERT_EQ(Segment::INVALID, mem.seg.seg);
    ASSERT_EQ(Register::INVALID, mem.base.reg);
    ASSERT_EQ(Register::INVALID, mem.index.reg);
    ASSERT_EQ(0, mem.scale.val);
    ASSERT_EQ(0, mem.disp.val);
}

TEST(Memory_FullConstruction)
{
    Segment seg(Segment::DS);
    Register base(Register::EBX);
    Register index(Register::ESI);
    Value scale(4);
    Value disp(0x100);
    
    Memory mem(seg, base, index, scale, disp);
    
    ASSERT_EQ(Segment::DS, mem.seg.seg);
    ASSERT_EQ(Register::EBX, mem.base.reg);
    ASSERT_EQ(Register::ESI, mem.index.reg);
    ASSERT_EQ(4, mem.scale.val);
    ASSERT_EQ(0x100, mem.disp.val);
}

TEST(Memory_NoSegmentConstruction)
{
    Register base(Register::EBP);
    Register index(Register::EDI);
    Value scale(2);
    Value disp(0x8);
    
    Memory mem(base, index, scale, disp);
    
    ASSERT_EQ(Segment::INVALID, mem.seg.seg);
    ASSERT_EQ(Register::EBP, mem.base.reg);
    ASSERT_EQ(Register::EDI, mem.index.reg);
    ASSERT_EQ(2, mem.scale.val);
    ASSERT_EQ(0x8, mem.disp.val);
}

TEST(Memory_DisplacementOnlyConstruction)
{
    Value disp(0x401000);
    Memory mem(disp);
    
    ASSERT_EQ(Segment::INVALID, mem.seg.seg);
    ASSERT_EQ(Register::INVALID, mem.base.reg);
    ASSERT_EQ(Register::INVALID, mem.index.reg);
    ASSERT_EQ(0, mem.scale.val);
    ASSERT_EQ(0x401000, mem.disp.val);
}

TEST(Memory_Equality)
{
    Register base(Register::EAX);
    Value disp(0x10);
    
    Memory mem1(base, Register(), Value(), disp);
    Memory mem2(base, Register(), Value(), disp);
    Memory mem3(Register(Register::EBX), Register(), Value(), disp);
    
    State state;
    ASSERT_TRUE(mem1.Equals(mem2, state));
    ASSERT_FALSE(mem1.Equals(mem3, state));
}

TEST(Memory_ComplexAddressingModes)
{
    // [ebx + esi*2 + 0x100]
    Memory mem1(Register(Register::EBX), Register(Register::ESI), Value(2), Value(0x100));
    
    // [ebp + 0x8] (no index, no scale)
    Memory mem2(Register(Register::EBP), Register(), Value(), Value(0x8));
    
    // [0x401000] (displacement only)
    Memory mem3(Value(0x401000));
    
    State state;
    ASSERT_FALSE(mem1.Equals(mem2, state));
    ASSERT_FALSE(mem1.Equals(mem3, state));
    ASSERT_FALSE(mem2.Equals(mem3, state));
}

TEST(Memory_StateWildcards)
{
    Memory mem1(State::MakeMemoryN(1));
    Memory mem2(Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem3(Register(Register::EBX), Register(), Value(), Value(0x20));
    
    State state;
    
    // First comparison should succeed and bind memory
    ASSERT_TRUE(mem1.Equals(mem2, state));
    ASSERT_TRUE(state.memorys[1]);
    
    // Second comparison with same wildcard should only succeed if same memory
    ASSERT_TRUE(mem1.Equals(mem2, state));   // Same memory
    ASSERT_FALSE(mem1.Equals(mem3, state));  // Different memory
}

TEST(Memory_ScaleFactors)
{
    Register base(Register::EAX);
    Register index(Register::EBX);
    
    // Valid x86 scale factors: 1, 2, 4, 8
    Memory mem1(base, index, Value(1), Value(0));
    Memory mem2(base, index, Value(2), Value(0));
    Memory mem4(base, index, Value(4), Value(0));
    Memory mem8(base, index, Value(8), Value(0));
    
    State state;
    ASSERT_FALSE(mem1.Equals(mem2, state));
    ASSERT_FALSE(mem1.Equals(mem4, state));
    ASSERT_FALSE(mem1.Equals(mem8, state));
    ASSERT_FALSE(mem2.Equals(mem4, state));
    ASSERT_FALSE(mem2.Equals(mem8, state));
    ASSERT_FALSE(mem4.Equals(mem8, state));
}

TEST(Memory_SegmentOverrides)
{
    Register base(Register::EBX);
    Value disp(0x100);
    
    Memory memDS(Segment::DS, base, Register(), Value(), disp);
    Memory memES(Segment::ES, base, Register(), Value(), disp);
    Memory memFS(Segment::FS, base, Register(), Value(), disp);
    Memory memGS(Segment::GS, base, Register(), Value(), disp);
    Memory memNone(base, Register(), Value(), disp); // No segment override
    
    State state;
    ASSERT_FALSE(memDS.Equals(memES, state));
    ASSERT_FALSE(memDS.Equals(memFS, state));
    ASSERT_FALSE(memDS.Equals(memGS, state));
    ASSERT_FALSE(memDS.Equals(memNone, state));
}

TEST(Memory_WildcardSegments)
{
    Memory mem1(State::SegWild(), Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem2(Segment::DS, Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem3(Segment::ES, Register(Register::EAX), Register(), Value(), Value(0x10));
    Memory mem4(Segment(), Register(Register::EAX), Register(), Value(), Value(0x10)); // No segment
    
    State state;
    
    // Wildcard segment should match any segment
    ASSERT_TRUE(mem1.Equals(mem2, state));
    ASSERT_TRUE(mem1.Equals(mem3, state));
    ASSERT_TRUE(mem1.Equals(mem4, state));
}

TEST(Memory_ComplexWildcardPattern)
{
    // Pattern: [reg1 + reg2*scale1 + disp1]
    Memory pattern(
        State::SegWild(),
        Register(State::MakeRegisterN(1)),
        Register(State::MakeRegisterN(2)),
        Value(State::MakeValueN(1)),
        Value(State::MakeValueN(2))
    );
    
    // Concrete: [ds:eax + ebx*4 + 0x100]
    Memory concrete(
        Segment::DS,
        Register::EAX,
        Register::EBX,
        Value(4),
        Value(0x100)
    );
    
    State state;
    ASSERT_TRUE(pattern.Equals(concrete, state));
    
    // Verify bindings
    ASSERT_TRUE(state.registers[1]);
    ASSERT_TRUE(state.registers[2]);
    ASSERT_TRUE(state.values[1]);
    ASSERT_TRUE(state.values[2]);
    
    ASSERT_EQ(Register::EAX, state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, state.registers[2]().reg);
    ASSERT_EQ(4, state.values[1]().val);
    ASSERT_EQ(0x100, state.values[2]().val);
}