#include "unity.h"
#include "../native/platform.h"
#include "../include/async_jammer.h"

// Unity setup/teardown functions
void setUp(void) {}
void tearDown(void) {}

// Test helper functions
void test_jammer_basic_initialization() {
    AsyncJammer jammer;
    
    // Should start inactive
    TEST_ASSERT_EQUAL(STEP_JAMMER_LEAVE_OFF, jammer.step_jammer(0));
    TEST_ASSERT_EQUAL(JAMMER_STATE_MUTED, jammer.get_current_state());
    TEST_ASSERT_EQUAL(0.0, jammer.get_frequency_offset());
}

void test_jammer_transmission_start() {
    AsyncJammer jammer;
    unsigned long time = 0;
    
    // Start transmission
    jammer.start_jammer_transmission(true);
    
    // Should start transmitting
    TEST_ASSERT_EQUAL(JAMMER_STATE_TRANSMITTING, jammer.get_current_state());
    TEST_ASSERT_EQUAL(STEP_JAMMER_TURN_ON, jammer.step_jammer(time));
}

void test_jammer_frequency_drift() {
    AsyncJammer jammer;
    unsigned long time = 0;
    
    jammer.start_jammer_transmission(true);
    jammer.step_jammer(time); // Initial turn on
    
    float initial_offset = jammer.get_frequency_offset();
    
    // Advance time and step multiple times to see frequency drift
    bool frequency_changed = false;
    for (int i = 0; i < 20; i++) {
        time += JAMMER_STEP_INTERVAL;
        jammer.step_jammer(time);
        
        if (jammer.get_frequency_offset() != initial_offset) {
            frequency_changed = true;
            break;
        }
    }
    
    // Frequency should have drifted
    TEST_ASSERT_TRUE(frequency_changed);
    
    // Frequency should stay within bounds
    TEST_ASSERT_TRUE(jammer.get_frequency_offset() >= -JAMMER_MAX_DRIFT);
    TEST_ASSERT_TRUE(jammer.get_frequency_offset() <= JAMMER_MAX_DRIFT);
}

void test_jammer_muting_behavior() {
    AsyncJammer jammer;
    unsigned long time = 0;
    
    jammer.start_jammer_transmission(true);
    jammer.step_jammer(time); // Initial turn on
    
    // Step many times to eventually trigger muting
    bool muting_occurred = false;
    for (int i = 0; i < 100; i++) {
        time += JAMMER_STEP_INTERVAL;
        int result = jammer.step_jammer(time);
        
        if (result == STEP_JAMMER_TURN_OFF || jammer.get_current_state() == JAMMER_STATE_MUTED) {
            muting_occurred = true;
            break;
        }
    }
    
    // Muting should eventually occur due to random probability
    TEST_ASSERT_TRUE(muting_occurred);
}

void test_jammer_timing_consistency() {
    AsyncJammer jammer;
    unsigned long time = 0;
    
    jammer.start_jammer_transmission(true);
    
    // Test that jammer responds at consistent intervals
    jammer.step_jammer(time); // Turn on
    
    // Should not be ready for next step immediately
    TEST_ASSERT_EQUAL(STEP_JAMMER_LEAVE_ON, jammer.step_jammer(time + 1));
    
    // Should be ready after interval
    time += JAMMER_STEP_INTERVAL;
    int result = jammer.step_jammer(time);
    TEST_ASSERT_TRUE(result == STEP_JAMMER_CHANGE_FREQ || result == STEP_JAMMER_TURN_OFF || result == STEP_JAMMER_LEAVE_ON);
}

void test_jammer_bounds_enforcement() {
    AsyncJammer jammer;
    unsigned long time = 0;
    
    jammer.start_jammer_transmission(true);
    jammer.step_jammer(time); // Initial turn on
    
    // Step many times to test bounds enforcement
    for (int i = 0; i < 1000; i++) {
        time += JAMMER_STEP_INTERVAL;
        jammer.step_jammer(time);
        
        // Frequency should always stay within bounds
        TEST_ASSERT_TRUE(jammer.get_frequency_offset() >= -JAMMER_MAX_DRIFT);
        TEST_ASSERT_TRUE(jammer.get_frequency_offset() <= JAMMER_MAX_DRIFT);
    }
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_jammer_basic_initialization);
    RUN_TEST(test_jammer_transmission_start);
    RUN_TEST(test_jammer_frequency_drift);
    RUN_TEST(test_jammer_muting_behavior);
    RUN_TEST(test_jammer_timing_consistency);
    RUN_TEST(test_jammer_bounds_enforcement);
    
    return UNITY_END();
}
