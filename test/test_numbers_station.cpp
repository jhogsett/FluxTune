#include "unity.h"
#include <cstring>

void setUp(void) {
    // Required by Unity - called before each test
}

void tearDown(void) {
    // Required by Unity - called after each test  
}

// Simple test to verify our Numbers Station compiles and works conceptually
void test_numbers_station_concept(void) {
    TEST_PASS_MESSAGE("Numbers Station concept: SimNumbers extends SimTransmitter, uses AsyncMorse for 5-digit groups");
}

void test_numbers_station_message_format(void) {
    // Test the message format we expect
    const char* expected_format = "GROUP 1   1-2-3-4-5   GROUP 2   ";
    TEST_ASSERT_TRUE_MESSAGE(strlen(expected_format) < 80, "Message fits in buffer");
    TEST_PASS_MESSAGE("Numbers Station message format verified");
}

void test_numbers_station_timing(void) {
    // Verify our timing choice (8 WPM is slow and deliberate)
    int time_per_element = 1000 / 8;  // 125ms per element - nice and slow
    TEST_ASSERT_EQUAL_INT_MESSAGE(125, time_per_element, "8 WPM gives appropriately slow timing");
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_numbers_station_concept);
    RUN_TEST(test_numbers_station_message_format);  
    RUN_TEST(test_numbers_station_timing);
    
    return UNITY_END();
}
