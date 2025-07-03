#include "test_framework.h"

// Only include basic functionality tests
#include "test_value.cpp"
#include "test_register.cpp"
#include "test_memory.cpp"
#include "test_operand.cpp"
#include "test_state.cpp"

int main()
{
    std::cout << "=== InterObfu Core Component Test Suite ===\n";
    std::cout << "Testing core components without assembly dependencies\n\n";
    
    TestFramework::instance().runAllTests();
    
    return 0;
}