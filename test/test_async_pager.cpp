#include "unity.h"
#include "../native/platform.h"
#include "../include/async_pager.h"

// Unity setup/teardown functions
void setUp(void) {}
void tearDown(void) {}

// Test helper to advance time and check state transitions
void test_pager_basic_initialization() {
    AsyncPager pager;
    
    // Should start inactive
    TEST_ASSERT_EQUAL(STEP_PAGER_LEAVE_OFF, pager.step_pager(0));
    TEST_ASSERT_EQUAL(PAGER_STATE_SILENCE, pager.get_current_state());
}

void test_pager_transmission_cycle() {
    AsyncPager pager;
    unsigned long time = 0;
    
    // Start transmission
    pager.start_pager_transmission(true);
    
    // Should start with Tone A
    TEST_ASSERT_EQUAL(PAGER_STATE_TONE_A, pager.get_current_state());
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_ON, pager.step_pager(time));     // First call should turn on
    
    // Advance to end of Tone A (longer tone)
    time += PAGER_TONE_A_DURATION;
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_OFF, pager.step_pager(time));  // Should turn off for gap
    TEST_ASSERT_EQUAL(PAGER_STATE_GAP, pager.get_current_state());
    
    // Advance through gap
    time += PAGER_INTER_TONE_GAP;
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_ON, pager.step_pager(time));   // Should turn on for Tone B
    TEST_ASSERT_EQUAL(PAGER_STATE_TONE_B, pager.get_current_state());
    
    // Advance to end of Tone B (shorter tone)
    time += PAGER_TONE_B_DURATION;
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_OFF, pager.step_pager(time)); // Should transition to silence
    TEST_ASSERT_EQUAL(PAGER_STATE_SILENCE, pager.get_current_state());
    
    // During silence, should stay off
    time += 1000; // 1 second into silence
    TEST_ASSERT_EQUAL(STEP_PAGER_LEAVE_OFF, pager.step_pager(time));
}

void test_pager_repeat_cycle() {
    AsyncPager pager;
    unsigned long time = 0;
    
    pager.start_pager_transmission(true);
    
    // Complete one full cycle (Tone A + Gap + Tone B + maximum possible silence)
    time += PAGER_TONE_A_DURATION + PAGER_INTER_TONE_GAP + PAGER_TONE_B_DURATION + PAGER_SILENCE_MAX + 1000; // Extra margin
    
    // Should start new cycle with Tone A
    int result = pager.step_pager(time);
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_ON, result);
    TEST_ASSERT_EQUAL(PAGER_STATE_TONE_A, pager.get_current_state());
}

void test_pager_no_repeat() {
    AsyncPager pager;
    unsigned long time = 0;
    
    pager.start_pager_transmission(false); // No repeat
    
    // First call should turn on
    int result1 = pager.step_pager(time);
    
    // Advance through Tone A
    time += PAGER_TONE_A_DURATION;
    int result2 = pager.step_pager(time);
    
    // Advance through gap
    time += PAGER_INTER_TONE_GAP;
    int result3 = pager.step_pager(time);
    
    // Advance through Tone B - this should turn off and make pager inactive
    time += PAGER_TONE_B_DURATION;
    int result4 = pager.step_pager(time);
    
    // Complete one full cycle  
    time += PAGER_SILENCE_MAX + 1000; // Extra margin
    
    // Should stay silent (not repeat)
    int result5 = pager.step_pager(time);
    TEST_ASSERT_EQUAL(STEP_PAGER_LEAVE_OFF, result5);
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_pager_basic_initialization);
    RUN_TEST(test_pager_transmission_cycle);
    RUN_TEST(test_pager_repeat_cycle);
    RUN_TEST(test_pager_no_repeat);
    
    return UNITY_END();
}
