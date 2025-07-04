#include "test_framework.h"

// Include all test files
#include "test_instruction.cpp"
#include "test_pattern.cpp"
#include "test_peephole.cpp"
#include "test_converter.cpp"
#include "test_operand.cpp"
#include "test_register.cpp"
#include "test_memory.cpp"
#include "test_value.cpp"
#include "test_state.cpp"

int main()
{
    std::cout << "=== InterObfu Test Suite ===\n";
    std::cout << "Testing intermediate x86 instruction representation\n\n";
    
    TestFramework::instance().runAllTests();
    
    return 0;
}