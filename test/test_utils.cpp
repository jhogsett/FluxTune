#include "unity.h"
#include "../native/platform.h"
#include "utils.h"

void setUp(void) {
    // Set up before each test
}

void tearDown(void) {
    // Clean up after each test
}

void test_load_f_string_function(void) {
    // Test loading flash strings (F() macro)
    char buffer[32];
    char* result = load_f_string(F("test"), buffer);
    TEST_ASSERT_EQUAL_STRING("test", result);
    TEST_ASSERT_EQUAL_PTR(buffer, result);
    
    // Test with NULL buffer (should use internal buffer)
    result = load_f_string(F("hello"), NULL);
    TEST_ASSERT_EQUAL_STRING("hello", result);
}

void test_random_unique_function(void) {
    // Test generating unique random numbers
    int result[3];
    random_unique(3, 10, result);
    
    // Check that all numbers are different
    TEST_ASSERT_NOT_EQUAL(result[0], result[1]);
    TEST_ASSERT_NOT_EQUAL(result[0], result[2]);
    TEST_ASSERT_NOT_EQUAL(result[1], result[2]);
    
    // Check that all numbers are within range
    for(int i = 0; i < 3; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL(0, result[i]);
        TEST_ASSERT_LESS_THAN(10, result[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_load_f_string_function);
    RUN_TEST(test_random_unique_function);
    
    return UNITY_END();
}
