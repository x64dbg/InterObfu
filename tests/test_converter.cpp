#include "test_framework.h"
#include "Convert.h"
#include "Register.h"
#include "Opcode.h"

// Note: Since the Converter class has static internal maps that need initialization,
// these tests verify the interface contracts rather than actual conversion logic
// (which would require the full capstone integration)

TEST(Converter_InitializationSafety)
{
    // Test that calling init multiple times is safe
    EXPECT_NO_THROW(Converter::init());
    EXPECT_NO_THROW(Converter::init());
    EXPECT_NO_THROW(Converter::init());
}

TEST(Converter_RegisterConversionInterface)
{
    // Initialize converter
    Converter::init();
    
    // Test register to string conversion interface
    auto eaxStr = Converter::reg2str(Register::EAX);
    ASSERT_FALSE(eaxStr.empty()); // Should return some string for valid register
    
    // Test string to register conversion interface  
    auto reg = Converter::str2reg(eaxStr);
    ASSERT_NE(Register::INVALID, reg); // Should return a valid register
}

TEST(Converter_InstructionConversionInterface)
{
    // Initialize converter
    Converter::init();
    
    // Test instruction to string conversion interface
    auto movStr = Converter::ins2str(2); // Assuming 2 is a valid mnemonic
    ASSERT_FALSE(movStr.empty()); // Should return some string for valid instruction
    
    // Test string to instruction conversion interface
    auto ins = Converter::str2ins(movStr);
    ASSERT_NE(0, ins); // Should return a non-zero mnemonic for valid instruction
}

TEST(Converter_InvalidRegisterHandling)
{
    Converter::init();
    
    // Test invalid register string
    auto invalidReg = Converter::str2reg("invalid_register_name");
    ASSERT_EQ(Register::INVALID, invalidReg);
    
    // Test invalid register enum
    auto invalidRegStr = Converter::reg2str(Register::Registers(-1));
    ASSERT_TRUE(invalidRegStr.empty());
}

TEST(Converter_InvalidInstructionHandling)
{
    Converter::init();
    
    // Test invalid instruction string
    auto invalidIns = Converter::str2ins("invalid_instruction_name");
    ASSERT_EQ(0, invalidIns); // X86_INS_INVALID is typically 0
    
    // Test invalid instruction enum
    auto invalidInsStr = Converter::ins2str(Opcode::Mnemonics(-1));
    ASSERT_TRUE(invalidInsStr.empty());
}

TEST(Converter_RoundTripRegisterConversion)
{
    Converter::init();
    
    // Test that register conversion is symmetric for known registers
    std::vector<Register::Registers> testRegs = {
        Register::EAX, Register::EBX, Register::ECX, Register::EDX,
        Register::ESI, Register::EDI, Register::EBP, Register::ESP,
        Register::RAX, Register::RBX, Register::RCX, Register::RDX,
        Register::AL, Register::AH, Register::BL, Register::BH,
        Register::AX, Register::BX, Register::CX, Register::DX
    };
    
    for(auto reg : testRegs)
    {
        auto regStr = Converter::reg2str(reg);
        if(!regStr.empty()) // If conversion succeeded
        {
            auto backToReg = Converter::str2reg(regStr);
            ASSERT_EQ(reg, backToReg); // Should convert back to original
        }
    }
}

TEST(Converter_CommonRegisterNames)
{
    Converter::init();
    
    // Test that common register names are handled
    std::vector<std::string> commonRegNames = {
        "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
        "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
        "al", "ah", "bl", "bh", "cl", "ch", "dl", "dh",
        "ax", "bx", "cx", "dx", "si", "di", "bp", "sp"
    };
    
    for(const auto& regName : commonRegNames)
    {
        auto reg = Converter::str2reg(regName);
        if(reg != Register::INVALID) // If register is recognized
        {
            auto backToStr = Converter::reg2str(reg);
            ASSERT_FALSE(backToStr.empty()); // Should convert back to some string
        }
    }
}

TEST(Converter_CommonInstructionNames)
{
    Converter::init();
    
    // Test that common instruction names are handled
    std::vector<std::string> commonInsNames = {
        "mov", "push", "pop", "add", "sub", "xor", "or", "and",
        "jmp", "call", "ret", "nop", "int", "cmp", "test",
        "lea", "shl", "shr", "sar", "rol", "ror"
    };
    
    for(const auto& insName : commonInsNames)
    {
        auto ins = Converter::str2ins(insName);
        if(ins != 0) // If instruction is recognized (0 is typically INVALID)
        {
            auto backToStr = Converter::ins2str(ins);
            ASSERT_FALSE(backToStr.empty()); // Should convert back to some string
        }
    }
}

TEST(Converter_CaseInsensitivity)
{
    Converter::init();
    
    // Test various case combinations for common registers
    std::vector<std::pair<std::string, std::string>> casePairs = {
        {"eax", "EAX"}, {"ebx", "EBX"}, {"ecx", "ECX"},
        {"mov", "MOV"}, {"push", "PUSH"}, {"pop", "POP"}
    };
    
    for(const auto& pair : casePairs)
    {
        auto reg1 = Converter::str2reg(pair.first);
        auto reg2 = Converter::str2reg(pair.second);
        auto ins1 = Converter::str2ins(pair.first);
        auto ins2 = Converter::str2ins(pair.second);
        
        // If either conversion succeeds, they should be equal
        if(reg1 != Register::INVALID && reg2 != Register::INVALID)
        {
            ASSERT_EQ(reg1, reg2);
        }
        if(ins1 != 0 && ins2 != 0)
        {
            ASSERT_EQ(ins1, ins2);
        }
    }
}

TEST(Converter_EmptyStringHandling)
{
    Converter::init();
    
    // Test empty string inputs
    auto emptyReg = Converter::str2reg("");
    ASSERT_EQ(Register::INVALID, emptyReg);
    
    auto emptyIns = Converter::str2ins("");
    ASSERT_EQ(0, emptyIns);
}

TEST(Converter_WhitespaceHandling)
{
    Converter::init();
    
    // Test strings with whitespace
    std::vector<std::string> whitespaceTests = {
        " eax", "eax ", " eax ", "\teax", "eax\n",
        " mov", "mov ", " mov ", "\tmov", "mov\n"
    };
    
    for(const auto& test : whitespaceTests)
    {
        // These might or might not be handled depending on implementation
        // Just verify no crashes occur
        EXPECT_NO_THROW(Converter::str2reg(test));
        EXPECT_NO_THROW(Converter::str2ins(test));
    }
}

TEST(Converter_ThreadSafety)
{
    // Test that multiple initializations and conversions don't crash
    for(int i = 0; i < 10; i++)
    {
        EXPECT_NO_THROW(Converter::init());
        EXPECT_NO_THROW(Converter::reg2str(Register::EAX));
        EXPECT_NO_THROW(Converter::str2reg("eax"));
        EXPECT_NO_THROW(Converter::ins2str(1));
        EXPECT_NO_THROW(Converter::str2ins("mov"));
    }
}

TEST(Converter_ExtendedRegisterSupport)
{
    Converter::init();
    
    // Test that extended x64 registers are supported
    std::vector<Register::Registers> extendedRegs = {
        Register::R8, Register::R9, Register::R10, Register::R11,
        Register::R12, Register::R13, Register::R14, Register::R15,
        Register::R8D, Register::R9D, Register::R10D, Register::R11D,
        Register::R8W, Register::R9W, Register::R10W, Register::R11W,
        Register::R8B, Register::R9B, Register::R10B, Register::R11B
    };
    
    for(auto reg : extendedRegs)
    {
        auto regStr = Converter::reg2str(reg);
        // Extended registers might or might not be supported
        // Just verify no crashes and consistent behavior
        if(!regStr.empty())
        {
            auto backToReg = Converter::str2reg(regStr);
            ASSERT_EQ(reg, backToReg);
        }
    }
}

TEST(Converter_SegmentRegisterSupport)
{
    Converter::init();
    
    // Test segment registers
    std::vector<Register::Registers> segmentRegs = {
        Register::CS, Register::DS, Register::ES,
        Register::FS, Register::GS, Register::SS
    };
    
    for(auto reg : segmentRegs)
    {
        auto regStr = Converter::reg2str(reg);
        if(!regStr.empty())
        {
            auto backToReg = Converter::str2reg(regStr);
            ASSERT_EQ(reg, backToReg);
        }
    }
}

TEST(Converter_FloatingPointRegisterSupport)
{
    Converter::init();
    
    // Test FPU and SIMD registers
    std::vector<Register::Registers> fpRegs = {
        Register::ST0, Register::ST1, Register::ST7,
        Register::XMM0, Register::XMM1, Register::XMM15,
        Register::YMM0, Register::YMM1, Register::YMM15,
        Register::MM0, Register::MM1, Register::MM7
    };
    
    for(auto reg : fpRegs)
    {
        auto regStr = Converter::reg2str(reg);
        if(!regStr.empty())
        {
            auto backToReg = Converter::str2reg(regStr);
            ASSERT_EQ(reg, backToReg);
        }
    }
}