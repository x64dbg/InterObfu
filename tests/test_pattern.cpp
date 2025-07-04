#include "test_framework.h"
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
#define X86_INS_INVALID 0

TEST(Pattern_DefaultConstruction)
{
    Pattern pattern;
    ASSERT_EQ(0, pattern.priority);
    ASSERT_TRUE(pattern.instrs.empty());
    ASSERT_TRUE(pattern.repls.empty());
}

TEST(Pattern_AddInstruction)
{
    Pattern pattern;
    Instruction instr(X86_INS_PUSH, Operand(Register::EAX));
    
    pattern.Add(instr);
    
    ASSERT_EQ(1, pattern.instrs.size());
    ASSERT_EQ(X86_INS_PUSH, pattern.instrs[0].opcode.mnem);
    ASSERT_EQ(Register::EAX, pattern.instrs[0].operands[0].reg.reg);
}

TEST(Pattern_AddMultipleInstructions)
{
    Pattern pattern;
    
    Instruction push(X86_INS_PUSH, Operand(Register::EAX));
    Instruction pop(X86_INS_POP, Operand(Register::EBX));
    
    pattern.Add(push);
    pattern.Add(pop);
    
    ASSERT_EQ(2, pattern.instrs.size());
    ASSERT_EQ(X86_INS_PUSH, pattern.instrs[0].opcode.mnem);
    ASSERT_EQ(X86_INS_POP, pattern.instrs[1].opcode.mnem);
}

TEST(Pattern_SearchSimplePattern)
{
    // Create pattern: push eax; pop ebx
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register::EBX)));
    
    // Create instruction sequence
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_NOP));
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    instructions.push_back(Instruction(X86_INS_NOP));
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(1, result); // Pattern starts at index 1
}

TEST(Pattern_SearchPatternNotFound)
{
    // Create pattern: push eax; pop ebx
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register::EBX)));
    
    // Create instruction sequence without the pattern
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::ECX))); // Different register
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(-1, result); // Pattern not found
}

TEST(Pattern_SearchWithWildcards)
{
    // Create pattern: push reg1; pop reg1
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    
    // Create instruction sequence: push eax; pop eax
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::EAX)));
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(0, result); // Pattern starts at index 0
}

TEST(Pattern_SearchWithWildcardMismatch)
{
    // Create pattern: push reg1; pop reg1 (same register)
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    
    // Create instruction sequence: push eax; pop ebx (different registers)
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(-1, result); // Pattern not found due to wildcard constraint
}

TEST(Pattern_SearchWithOffset)
{
    // Create pattern: push reg1
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    
    // Create instruction sequence
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_NOP));
    instructions.push_back(Instruction(X86_INS_NOP));
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    
    int64_t result = pattern.Search(instructions, 1); // Start search from index 1
    ASSERT_EQ(2, result); // Pattern starts at index 2
}

TEST(Pattern_MatchExactPosition)
{
    // Create pattern: mov reg1, reg2
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_MOV, 
                           Operand(Register(State::MakeRegisterN(1))),
                           Operand(Register(State::MakeRegisterN(2)))));
    
    // Create instruction sequence
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_NOP));
    instructions.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Register::EBX)));
    
    bool result = pattern.Match(instructions, 1);
    ASSERT_TRUE(result);
    
    // Verify state bindings
    ASSERT_TRUE(pattern.state.registers[1]);
    ASSERT_TRUE(pattern.state.registers[2]);
    ASSERT_EQ(Register::EAX, pattern.state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, pattern.state.registers[2]().reg);
}

TEST(Pattern_MatchWrongPosition)
{
    // Create pattern: mov reg1, reg2
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_MOV, 
                           Operand(Register(State::MakeRegisterN(1))),
                           Operand(Register(State::MakeRegisterN(2)))));
    
    // Create instruction sequence with pattern at different position
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_MOV, Operand(Register::EAX), Operand(Register::EBX)));
    instructions.push_back(Instruction(X86_INS_NOP));
    
    bool result = pattern.Match(instructions, 1); // Try to match at wrong position
    ASSERT_FALSE(result);
}

TEST(Pattern_ComplexMemoryPattern)
{
    // Create pattern: mov [reg1 + imm1], reg2
    Memory memPattern(
        Register(State::MakeRegisterN(1)),
        Register(),
        Value(),
        Value(State::MakeValueN(1))
    );
    
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_MOV,
                           Operand(memPattern),
                           Operand(Register(State::MakeRegisterN(2)))));
    
    // Create matching instruction: mov [eax + 0x10], ebx
    Memory memConcrete(Register(Register::EAX), Register(), Value(), Value(0x10));
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_MOV,
                                     Operand(memConcrete),
                                     Operand(Register::EBX)));
    
    bool result = pattern.Match(instructions, 0);
    ASSERT_TRUE(result);
    
    // Verify all bindings
    ASSERT_TRUE(pattern.state.registers[1]);
    ASSERT_TRUE(pattern.state.registers[2]);
    ASSERT_TRUE(pattern.state.values[1]);
    ASSERT_EQ(Register::EAX, pattern.state.registers[1]().reg);
    ASSERT_EQ(Register::EBX, pattern.state.registers[2]().reg);
    ASSERT_EQ(0x10, pattern.state.values[1]().val);
}

TEST(Pattern_ReplacementFunction)
{
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));
    
    // Add replacement function: push reg1; pop reg2 -> mov reg2, reg1
    pattern.repls.push_back([](const State& state) {
        return Instruction(X86_INS_MOV,
                          Operand(state.registers[2]()),
                          Operand(state.registers[1]()));
    });
    
    // Simulate pattern match by setting up state
    pattern.state.registers[1] = Register(Register::EAX);
    pattern.state.registers[2] = Register(Register::EBX);
    
    std::vector<Instruction> replacements;
    bool result = pattern.Produce(replacements);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(1, replacements.size());
    ASSERT_EQ(X86_INS_MOV, replacements[0].opcode.mnem);
    ASSERT_EQ(Register::EBX, replacements[0].operands[0].reg.reg);
    ASSERT_EQ(Register::EAX, replacements[0].operands[1].reg.reg);
}

TEST(Pattern_MultipleReplacements)
{
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_XOR, 
                           Operand(Register(State::MakeRegisterN(1))),
                           Operand(Register(State::MakeRegisterN(1)))));
    
    // Add multiple replacement instructions for "xor reg1, reg1" -> "mov reg1, 0"
    pattern.repls.push_back([](const State& state) {
        return Instruction(X86_INS_MOV,
                          Operand(state.registers[1]()),
                          Operand(Value(0)));
    });
    
    // Simulate pattern match
    pattern.state.registers[1] = Register(Register::EAX);
    
    std::vector<Instruction> replacements;
    bool result = pattern.Produce(replacements);
    
    ASSERT_TRUE(result);
    ASSERT_EQ(1, replacements.size());
    ASSERT_EQ(X86_INS_MOV, replacements[0].opcode.mnem);
    ASSERT_EQ(Register::EAX, replacements[0].operands[0].reg.reg);
    ASSERT_EQ(0, replacements[0].operands[1].imm.val);
}

TEST(Pattern_PriorityField)
{
    Pattern pattern1;
    Pattern pattern2;
    
    pattern1.priority = 10;
    pattern2.priority = 5;
    
    ASSERT_EQ(10, pattern1.priority);
    ASSERT_EQ(5, pattern2.priority);
}

TEST(Pattern_LongPattern)
{
    // Create a pattern with multiple instructions
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(1)))));
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register(State::MakeRegisterN(2)))));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(2)))));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register(State::MakeRegisterN(1)))));
    
    // Create matching instruction sequence
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EBX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::EBX)));
    instructions.push_back(Instruction(X86_INS_POP, Operand(Register::EAX)));
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(0, result); // Pattern starts at beginning
}

TEST(Pattern_EmptyPattern)
{
    Pattern pattern;
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_NOP));
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(-1, result); // Empty pattern should not match
}

TEST(Pattern_PatternLongerThanSequence)
{
    Pattern pattern;
    pattern.Add(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    pattern.Add(Instruction(X86_INS_POP, Operand(Register::EAX)));
    
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(X86_INS_PUSH, Operand(Register::EAX)));
    // Missing second instruction
    
    int64_t result = pattern.Search(instructions);
    ASSERT_EQ(-1, result); // Pattern longer than sequence
}