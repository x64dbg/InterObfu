#include "test_framework.h"
#include "Value.h"
#include "State.h"

TEST(Value_DefaultConstruction)
{
    Value v;
    ASSERT_EQ(0, v.val);
}

TEST(Value_IntConstruction)
{
    Value v(0x1234);
    ASSERT_EQ(0x1234, v.val);
}

TEST(Value_NegativeIntConstruction)
{
    Value v(-100);
    ASSERT_EQ(-100, v.val);
}

TEST(Value_LargeValueConstruction)
{
    Value v(0x123456789ABCDEFLL);
    ASSERT_EQ(0x123456789ABCDEFLL, v.val);
}

TEST(Value_Equality)
{
    Value v1(42);
    Value v2(42);
    Value v3(43);
    
    State state;
    ASSERT_TRUE(v1.Equals(v2, state));
    ASSERT_FALSE(v1.Equals(v3, state));
}

TEST(Value_ToString)
{
    Value v1(0);
    Value v2(0xFF);
    Value v3(0x1234ABCD);
    
    ASSERT_STREQ("0x0", v1.ToString().c_str());
    ASSERT_STREQ("0xFF", v2.ToString().c_str());
    ASSERT_STREQ("0x1234ABCD", v3.ToString().c_str());
}

TEST(Value_CustomCompareFunction)
{
    // Test with wildcard compare function
    auto wildcardCompare = [](const Value& a, const Value& b, State& state) -> bool {
        return true; // Always matches
    };
    
    Value v1(wildcardCompare);
    Value v2(123);
    Value v3(456);
    
    State state;
    ASSERT_TRUE(v1.Equals(v2, state));
    ASSERT_TRUE(v1.Equals(v3, state));
    ASSERT_TRUE(v2.Equals(v1, state)); // Test reverse direction
}

TEST(Value_StateWildcards)
{
    Value v1(State::MakeValueN(1));
    Value v2(42);
    Value v3(100);
    
    State state;
    
    // First comparison should succeed and bind value
    ASSERT_TRUE(v1.Equals(v2, state));
    ASSERT_TRUE(state.values[1]);
    ASSERT_EQ(42, state.values[1]().val);
    
    // Second comparison with same wildcard should only succeed if same value
    ASSERT_TRUE(v1.Equals(v2, state));  // Same value
    ASSERT_FALSE(v1.Equals(v3, state)); // Different value
}

TEST(Value_ZeroValues)
{
    Value v1(0);
    Value v2(0x0);
    Value v3(-0);
    
    State state;
    ASSERT_TRUE(v1.Equals(v2, state));
    ASSERT_TRUE(v1.Equals(v3, state));
}

TEST(Value_BoundaryValues)
{
    Value vMax(LLONG_MAX);
    Value vMin(LLONG_MIN);
    
    State state;
    ASSERT_FALSE(vMax.Equals(vMin, state));
    
    Value vMax2(LLONG_MAX);
    ASSERT_TRUE(vMax.Equals(vMax2, state));
}