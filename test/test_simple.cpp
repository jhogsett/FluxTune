#include <Arduino.h>
#include <unity.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_basic_math(void) {
    TEST_ASSERT_EQUAL(4, 2 + 2);
}

void test_string_compare(void) {
    TEST_ASSERT_EQUAL_STRING("hello", "hello");
}

void setup() {
    // Wait for serial connection
    delay(2000);
    
    UNITY_BEGIN();
    RUN_TEST(test_basic_math);
    RUN_TEST(test_string_compare);
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
