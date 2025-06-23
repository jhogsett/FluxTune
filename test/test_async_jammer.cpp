#include "unity/src/unity.h"
#include "async_jammer.h"

AsyncJammer *jammer;

void setUp(void) {
    jammer = new AsyncJammer();
}

void tearDown(void) {
    delete jammer;
}

void test_jammer_initialization(void) {
    // Test initial state
    TEST_ASSERT_EQUAL(JAMMER_STATE_MUTED, jammer->get_current_state());
    TEST_ASSERT_EQUAL_FLOAT(0.0, jammer->get_frequency_offset());
    
    // Test after starting transmission
    jammer->start_jammer_transmission(true);
    // Note: Current state changes on first step, not immediately
}

void test_jammer_basic_operation(void) {
    jammer->start_jammer_transmission(true);
    
    // First step should turn on
    int result = jammer->step_jammer(0);
    TEST_ASSERT_EQUAL(STEP_JAMMER_TURN_ON, result);
    TEST_ASSERT_EQUAL(JAMMER_STATE_TRANSMITTING, jammer->get_current_state());
}

void test_jammer_frequency_drift(void) {
    jammer->start_jammer_transmission(true);
    
    // Step through several cycles to allow drift
    unsigned long time = 0;
    jammer->step_jammer(time);
    
    // Advance time by several step intervals
    for (int i = 0; i < 10; i++) {
        time += JAMMER_STEP_INTERVAL;
        jammer->step_jammer(time);
    }
    
    // Frequency offset should be within bounds after drift
    float final_offset = jammer->get_frequency_offset();
    TEST_ASSERT_TRUE(final_offset >= -JAMMER_MAX_DRIFT);
    TEST_ASSERT_TRUE(final_offset <= JAMMER_MAX_DRIFT);
}

void test_jammer_timing_intervals(void) {
    jammer->start_jammer_transmission(true);
    
    unsigned long time = 0;
    
    // First step at time 0
    int result = jammer->step_jammer(time);
    TEST_ASSERT_EQUAL(STEP_JAMMER_TURN_ON, result);
    
    // Should stay on for subsequent calls within interval
    time += 10; // Less than JAMMER_STEP_INTERVAL
    result = jammer->step_jammer(time);
    TEST_ASSERT_TRUE(result == STEP_JAMMER_LEAVE_ON || result == STEP_JAMMER_CHANGE_FREQ);
}

void test_jammer_boundary_enforcement(void) {
    jammer->start_jammer_transmission(true);
    
    // Step through many cycles to test boundary enforcement
    unsigned long time = 0;
    for (int i = 0; i < 100; i++) {
        time += JAMMER_STEP_INTERVAL;
        jammer->step_jammer(time);
        
        // Verify frequency stays within bounds
        float offset = jammer->get_frequency_offset();
        TEST_ASSERT_TRUE(offset >= -JAMMER_MAX_DRIFT);
        TEST_ASSERT_TRUE(offset <= JAMMER_MAX_DRIFT);
    }
}

void test_jammer_state_constants(void) {
    // Verify all constants are unique
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_ON, STEP_JAMMER_TURN_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_ON, STEP_JAMMER_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_ON, STEP_JAMMER_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_ON, STEP_JAMMER_CHANGE_FREQ);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_OFF, STEP_JAMMER_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_OFF, STEP_JAMMER_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_JAMMER_TURN_OFF, STEP_JAMMER_CHANGE_FREQ);
    
    TEST_ASSERT_NOT_EQUAL(JAMMER_STATE_TRANSMITTING, JAMMER_STATE_MUTED);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_jammer_initialization);
    RUN_TEST(test_jammer_basic_operation);
    RUN_TEST(test_jammer_frequency_drift);
    RUN_TEST(test_jammer_timing_intervals);
    RUN_TEST(test_jammer_boundary_enforcement);
    RUN_TEST(test_jammer_state_constants);
    
    return UNITY_END();
}
