#include "test_framework.h"
#include "Peephole.h"
#include "Pattern.h"
#include "Instruction.h"
#include "State.h"

// Mock X86 instruction constants
#define X86_INS_NOP 1
#define X86_INS_MOV 2
#define X86_INS_PUSH 3
#define X86_INS_POP 4
#define X86_INS_ADD 5
#define X86_INS_SUB 6
#define X86_INS_XOR 7
#define X86_INS_SHL 8
#define X86_INS_INVALID 0

TEST(Peephole_EmptyPatternsConstruction)
{
    std::vector<Pattern> patterns;
    EXPECT_NO_THROW(Peephole peephole(patterns));
}

TEST(Peephole_SinglePatternConstruction)
{
    std::vector<Pattern> patterns;
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_NOP));
    patterns.push_back(pattern);
    
    EXPECT_NO_THROW(Peephole peephole(patterns));
}

TEST(Peephole_EmptyInputOptimization)
{
    std::vector<Pattern> patterns;
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_NOP));
    patterns.push_back(pattern);
    
    Peephole peephole(patterns);
    
    std::vector<Instruction> input;
    std::vector<Instruction> output;
    
    bool result = peephole.Optimize(input, output);
    ASSERT_FALSE(result); // Empty input should not be optimized
    ASSERT_TRUE(output.empty());
}

TEST(Peephole_NoMatchingPatterns)
{
    // Create a pattern that won't match
    std::vector<Pattern> patterns;
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    patterns.push_back(pattern);
    
    Peephole peephole(patterns);
    
    // Input that doesn't match the pattern
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Register::EBX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_FALSE(result); // No optimization should occur
}

TEST(Peephole_SimplePatternRemoval)
{
    // Create pattern for NOP removal
    std::vector<Pattern> patterns;
    Pattern nopPattern;
    nopPattern.Add(Instruction(X86_INS_NOP));
    // No replacement = removal
    patterns.push_back(nopPattern);
    
    Peephole peephole(patterns);
    
    // Input with NOP instructions
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Register::EBX)));
    input.push_back(Instruction(X86_INS_NOP));
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EBX), Operand(Register::ECX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(2, output.size()); // NOP should be removed
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem);
    ASSERT_EQ(X86_INS_MOV, output[1].opcode.mnem);
}

TEST(Peephole_WildcardPattern)
{
    // Create pattern: shl reg1, 0 (no-op shift)
    std::vector<Pattern> patterns;
    Pattern shlPattern;
    shlPattern.Add(Instruction(X86_INS_SHL,
                              Operand(Register(State::MakeRegisterN(1))),
                              Operand(Value(0))));
    // No replacement = removal
    patterns.push_back(shlPattern);
    
    Peephole peephole(patterns);
    
    // Input with shift by zero
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_SHL, Operand(Register::EAX), Operand(Value(0))));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EAX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(2, output.size()); // SHL should be removed
    ASSERT_EQ(X86_INS_PUSH, output[0].opcode.mnem);
    ASSERT_EQ(X86_INS_POP, output[1].opcode.mnem);
}

TEST(Peephole_PushPopSameRegisterRemoval)
{
    // Create pattern: push reg1; pop reg1
    std::vector<Pattern> patterns;
    Pattern pushPopPattern;
    pushPopPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pushPopPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    // No replacement = removal
    patterns.push_back(pushPopPattern);
    
    Peephole peephole(patterns);
    
    // Input with redundant push/pop
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Value(42))));
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EBX), Operand(Value(43))));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(2, output.size()); // push/pop pair should be removed
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem);
    ASSERT_EQ(42, output[0].operands[1].imm.val);
    ASSERT_EQ(X86_INS_MOV, output[1].opcode.mnem);
    ASSERT_EQ(43, output[1].operands[1].imm.val);
}

TEST(Peephole_PushPopDifferentRegistersToMov)
{
    // Create pattern: push reg1; pop reg2 -> mov reg2, reg1
    std::vector<Pattern> patterns;
    Pattern pushPopPattern;
    pushPopPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pushPopPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));
    
    // Add replacement
    pushPopPattern.repls.push_back([](const State& state) {
        return Instruction(X86_INS_MOV,
                          Operand(state.registers[2]()),
                          Operand(state.registers[1]()));
    });
    
    patterns.push_back(pushPopPattern);
    
    Peephole peephole(patterns);
    
    // Input with push/pop to different registers
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(1, output.size());
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem);
    ASSERT_EQ(Register::EBX, output[0].operands[0].reg.reg);
    ASSERT_EQ(Register::EAX, output[0].operands[1].reg.reg);
}

TEST(Peephole_MultiplePatterns)
{
    std::vector<Pattern> patterns;
    
    // Pattern 1: shl reg1, 0 -> remove
    Pattern shlPattern;
    shlPattern.Add(Instruction(X86_INS_SHL,
                              Operand(Register(State::MakeRegisterN(1))),
                              Operand(Value(0))));
    patterns.push_back(shlPattern);
    
    // Pattern 2: push reg1; pop reg1 -> remove
    Pattern pushPopPattern;
    pushPopPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pushPopPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    patterns.push_back(pushPopPattern);
    
    Peephole peephole(patterns);
    
    // Input with both patterns
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_SHL, Operand(Register::EAX), Operand(Value(0))));
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EBX)));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Value(42))));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(1, output.size()); // Both patterns should be removed
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem);
}

TEST(Peephole_PatternPriority)
{
    std::vector<Pattern> patterns;
    
    // Higher priority pattern (longer)
    Pattern longPattern;
    longPattern.priority = 10;
    longPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    longPattern.Add(Instruction(X86_INS_NOP));
    longPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    patterns.push_back(longPattern);
    
    // Lower priority pattern (shorter)
    Pattern shortPattern;
    shortPattern.priority = 5;
    shortPattern.Add(Instruction(X86_INS_NOP));
    patterns.push_back(shortPattern);
    
    Peephole peephole(patterns);
    
    // Input that matches both patterns
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_NOP));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EAX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.empty()); // Long pattern should be chosen (removes all 3 instructions)
}

TEST(Peephole_NoWildcardMismatch)
{
    // Create pattern: push reg1; pop reg1 (same register required)
    std::vector<Pattern> patterns;
    Pattern pushPopPattern;
    pushPopPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pushPopPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    patterns.push_back(pushPopPattern);
    
    Peephole peephole(patterns);
    
    // Input with different registers (should not match)
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EBX))); // Different register
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_FALSE(result); // Should not match due to wildcard constraint
}

TEST(Peephole_LongerPatternsPreferred)
{
    std::vector<Pattern> patterns;
    
    // Shorter pattern: push reg1
    Pattern shortPattern;
    shortPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    patterns.push_back(shortPattern);
    
    // Longer pattern: push reg1; pop reg2
    Pattern longPattern;
    longPattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    longPattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));
    longPattern.repls.push_back([](const State& state) {
        return Instruction(X86_INS_MOV,
                          Operand(state.registers[2]()),
                          Operand(state.registers[1]()));
    });
    patterns.push_back(longPattern);
    
    Peephole peephole(patterns);
    
    // Input that matches both patterns
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(1, output.size());
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem); // Longer pattern should be chosen
}

TEST(Peephole_PreserveUnmatchedInstructions)
{
    // Create simple pattern
    std::vector<Pattern> patterns;
    Pattern nopPattern;
    nopPattern.Add(Instruction(X86_INS_NOP));
    patterns.push_back(nopPattern);
    
    Peephole peephole(patterns);
    
    // Input with mixed instructions
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Value(1))));
    input.push_back(Instruction(X86_INS_NOP)); // This should be removed
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EBX), Operand(Value(2))));
    input.push_back(Instruction(X86_INS_NOP)); // This should be removed
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::ECX), Operand(Value(3))));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(3, output.size()); // Only MOV instructions should remain
    ASSERT_EQ(X86_INS_MOV, output[0].opcode.mnem);
    ASSERT_EQ(1, output[0].operands[1].imm.val);
    ASSERT_EQ(X86_INS_MOV, output[1].opcode.mnem);
    ASSERT_EQ(2, output[1].operands[1].imm.val);
    ASSERT_EQ(X86_INS_MOV, output[2].opcode.mnem);
    ASSERT_EQ(3, output[2].operands[1].imm.val);
}

TEST(Peephole_ComplexMemoryPattern)
{
    // Pattern involving memory operands
    std::vector<Pattern> patterns;
    Pattern memPattern;
    
    // mov [mem1], reg1; mov reg1, [mem1] -> nop (redundant load after store)
    memPattern.Add(Instruction(X86_INS_MOV,
                              Operand(Memory(State::MakeMemoryN(1))),
                              Operand(Register(State::MakeRegisterN(1)))));
    memPattern.Add(Instruction(X86_INS_MOV,
                              Operand(Register(State::MakeRegisterN(1))),
                              Operand(Memory(State::MakeMemoryN(1)))));
    // No replacement = removal
    patterns.push_back(memPattern);
    
    Peephole peephole(patterns);
    
    // Input with redundant memory access
    Memory mem(Register(Register::EBX), Register(), Value(), Value(0x10));
    std::vector<Instruction> input;
    input.push_back(Instruction(X86_INS_MOV, Operand(mem), Operand(Register::EAX)));
    input.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(mem)));
    
    std::vector<Instruction> output;
    bool result = peephole.Optimize(input, output);
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(output.empty()); // Both instructions should be removed
}