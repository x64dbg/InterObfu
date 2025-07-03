#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>

class TestFramework
{
public:
    struct TestResult
    {
        std::string name;
        bool passed;
        std::string message;
    };

    static TestFramework& instance()
    {
        static TestFramework instance;
        return instance;
    }

    void registerTest(const std::string& name, std::function<void()> test)
    {
        tests.push_back({name, test});
    }

    void runAllTests()
    {
        std::cout << "Running " << tests.size() << " tests...\n\n";
        
        int passed = 0;
        int failed = 0;

        for(auto& test : tests)
        {
            std::cout << "Running: " << test.name << " ... ";
            
            try
            {
                test.func();
                std::cout << "PASSED\n";
                passed++;
            }
            catch(const std::exception& e)
            {
                std::cout << "FAILED: " << e.what() << "\n";
                failed++;
            }
            catch(...)
            {
                std::cout << "FAILED: Unknown exception\n";
                failed++;
            }
        }

        std::cout << "\n=== Test Results ===\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "Total:  " << tests.size() << "\n";

        if(failed > 0)
        {
            std::cout << "\nSome tests failed!\n";
            exit(1);
        }
        else
        {
            std::cout << "\nAll tests passed!\n";
        }
    }

private:
    struct Test
    {
        std::string name;
        std::function<void()> func;
    };

    std::vector<Test> tests;
};

// Test macros
#define TEST(name) \
    void test_##name(); \
    static bool registered_##name = []() { \
        TestFramework::instance().registerTest(#name, test_##name); \
        return true; \
    }(); \
    void test_##name()

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("ASSERT_TRUE failed: " #condition); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            throw std::runtime_error("ASSERT_FALSE failed: " #condition); \
        } \
    } while(0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw std::runtime_error("ASSERT_EQ failed: expected " + std::to_string(expected) + " but got " + std::to_string(actual)); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            throw std::runtime_error("ASSERT_NE failed: values should not be equal"); \
        } \
    } while(0)

#define ASSERT_STREQ(expected, actual) \
    do { \
        if (std::string(expected) != std::string(actual)) { \
            throw std::runtime_error("ASSERT_STREQ failed: expected '" + std::string(expected) + "' but got '" + std::string(actual) + "'"); \
        } \
    } while(0)

#define EXPECT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch(...) { \
            throw std::runtime_error("EXPECT_NO_THROW failed: " #statement " threw an exception"); \
        } \
    } while(0)