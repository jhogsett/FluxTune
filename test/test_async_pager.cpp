#include "unity.h"
#include "../include/async_pager.h"

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
    TEST_ASSERT_EQUAL(STEP_PAGER_LEAVE_ON, pager.step_pager(time));
    
    // Advance to end of Tone A
    time += PAGER_TONE_DURATION;
    TEST_ASSERT_EQUAL(STEP_PAGER_TURN_ON, pager.step_pager(time));  // Should transition to Tone B
    TEST_ASSERT_EQUAL(PAGER_STATE_TONE_B, pager.get_current_state());
    
    // Advance to end of Tone B  
    time += PAGER_TONE_DURATION;
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
    
    // Complete one full cycle (Tone A + Tone B + minimum silence)
    time += PAGER_TONE_DURATION * 2 + PAGER_SILENCE_MIN;
    
    // Should start new cycle with Tone A
    int result = pager.step_pager(time);
    TEST_ASSERT_TRUE(result == STEP_PAGER_TURN_ON || result == STEP_PAGER_LEAVE_ON);
    TEST_ASSERT_EQUAL(PAGER_STATE_TONE_A, pager.get_current_state());
}

void test_pager_no_repeat() {
    AsyncPager pager;
    unsigned long time = 0;
    
    pager.start_pager_transmission(false); // No repeat
    
    // Complete one full cycle
    time += PAGER_TONE_DURATION * 2 + PAGER_SILENCE_MIN;
    
    // Should stay silent (not repeat)
    TEST_ASSERT_EQUAL(STEP_PAGER_LEAVE_OFF, pager.step_pager(time));
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_pager_basic_initialization);
    RUN_TEST(test_pager_transmission_cycle);
    RUN_TEST(test_pager_repeat_cycle);
    RUN_TEST(test_pager_no_repeat);
    
    return UNITY_END();
}
