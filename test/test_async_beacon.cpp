#include "unity/src/unity.h"
#include "../native/platform.h"
#include "../include/async_beacon.h"

// Test fixture
AsyncBeacon* beacon;

void setUp(void) {
    beacon = new AsyncBeacon();
}

void tearDown(void) {
    delete beacon;
    beacon = nullptr;
}

// =====================================================
// BASIC FUNCTIONALITY TESTS
// =====================================================

void test_beacon_basic_initialization(void) {
    TEST_ASSERT_NOT_NULL(beacon);
    TEST_PASS_MESSAGE("AsyncBeacon can be instantiated");
}

void test_beacon_state_constants(void) {
    // Test that state constants are properly defined and unique
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_TURN_ON, STEP_BEACON_TURN_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_TURN_ON, STEP_BEACON_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_TURN_ON, STEP_BEACON_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_TURN_OFF, STEP_BEACON_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_TURN_OFF, STEP_BEACON_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_BEACON_LEAVE_ON, STEP_BEACON_LEAVE_OFF);
    
    TEST_PASS_MESSAGE("All beacon state constants are properly defined");
}

void test_beacon_pattern_constants(void) {
    // Test that pattern constants are defined
    TEST_ASSERT_EQUAL_INT(0, BEACON_PATTERN_SIMPLE);
    TEST_ASSERT_EQUAL_INT(1, BEACON_PATTERN_FAST_KEYER);
    TEST_ASSERT_EQUAL_INT(2, BEACON_PATTERN_HEARTBEAT);
    TEST_ASSERT_EQUAL_INT(3, BEACON_PATTERN_LIGHTHOUSE);
    
    TEST_PASS_MESSAGE("All beacon pattern constants are properly defined");
}

// =====================================================
// SIMPLE PATTERN TESTS
// =====================================================

void test_beacon_simple_pattern_starts_with_turn_on(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 100, 100, true);
    
    int first_state = beacon->step_beacon(0);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_ON, first_state);
    TEST_PASS_MESSAGE("Simple beacon starts with TURN_ON");
}

void test_beacon_simple_pattern_transitions(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 50, 50, true);
    
    // Should start ON
    int state = beacon->step_beacon(0);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_ON, state);
    
    // Should stay ON during on-time
    state = beacon->step_beacon(25);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_LEAVE_ON, state);
    
    // Should turn OFF at end of on-time
    state = beacon->step_beacon(50);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_OFF, state);
    
    // Should stay OFF during off-time
    state = beacon->step_beacon(75);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_LEAVE_OFF, state);
    
    // Should turn ON again for next cycle
    state = beacon->step_beacon(100);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_ON, state);
    
    TEST_PASS_MESSAGE("Simple beacon has correct state transitions");
}

void test_beacon_timing_precision(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 100, 200, true);
    
    // Track transitions
    unsigned long turn_off_time = 0;
    unsigned long turn_on_time = 0;
    
    // Find first TURN_OFF
    for (unsigned long time = 0; time <= 150; time += 1) {
        int state = beacon->step_beacon(time);
        if (state == STEP_BEACON_TURN_OFF && turn_off_time == 0) {
            turn_off_time = time;
            break;
        }
    }
    
    // Find next TURN_ON
    for (unsigned long time = turn_off_time + 1; time <= 350; time += 1) {
        int state = beacon->step_beacon(time);
        if (state == STEP_BEACON_TURN_ON) {
            turn_on_time = time;
            break;
        }
    }
    
    TEST_ASSERT_EQUAL_UINT32(100, turn_off_time);
    TEST_ASSERT_EQUAL_UINT32(300, turn_on_time);  // 100 on + 200 off
    
    TEST_PASS_MESSAGE("Simple beacon timing is precise");
}

// =====================================================
// FAST KEYER PATTERN TESTS  
// =====================================================

void test_beacon_fast_keyer_pattern(void) {
    beacon->start_beacon(BEACON_PATTERN_FAST_KEYER, 120, 300, true);
    
    // Should start with TURN_ON
    int state = beacon->step_beacon(0);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_ON, state);
    
    // Should have multiple quick transitions
    bool found_multiple_transitions = false;
    int transition_count = 0;
    
    for (unsigned long time = 1; time <= 200; time += 1) {
        int current_state = beacon->step_beacon(time);
        if (current_state == STEP_BEACON_TURN_ON || current_state == STEP_BEACON_TURN_OFF) {
            transition_count++;
        }
    }
    
    // Fast keyer should have more transitions than simple pattern
    TEST_ASSERT_GREATER_THAN_INT(3, transition_count);
    TEST_PASS_MESSAGE("Fast keyer pattern has multiple quick transitions");
}

// =====================================================
// HEARTBEAT PATTERN TESTS
// =====================================================

void test_beacon_heartbeat_pattern(void) {
    beacon->start_beacon(BEACON_PATTERN_HEARTBEAT, 150, 500, true);
    
    // Should start with TURN_ON
    int state = beacon->step_beacon(0);
    TEST_ASSERT_EQUAL_INT(STEP_BEACON_TURN_ON, state);
    
    // Should have characteristic heartbeat timing
    int on_count = 0;
    int off_count = 0;
    
    for (unsigned long time = 0; time <= 800; time += 10) {
        int current_state = beacon->step_beacon(time);
        if (current_state == STEP_BEACON_TURN_ON || current_state == STEP_BEACON_LEAVE_ON) {
            on_count++;
        } else if (current_state == STEP_BEACON_TURN_OFF || current_state == STEP_BEACON_LEAVE_OFF) {
            off_count++;
        }
    }
    
    // Heartbeat should spend more time off than on (lub-dub pattern)
    TEST_ASSERT_GREATER_THAN_INT(on_count, off_count);
    TEST_PASS_MESSAGE("Heartbeat pattern has expected timing characteristics");
}

// =====================================================
// LIGHTHOUSE PATTERN TESTS
// =====================================================

void test_beacon_lighthouse_pattern(void) {
    beacon->start_beacon(BEACON_PATTERN_LIGHTHOUSE, 50, 400, true);
    
    // May start with TURN_OFF (lighthouse starts with beam away)
    int first_state = beacon->step_beacon(0);
    TEST_ASSERT_TRUE(first_state == STEP_BEACON_TURN_OFF || first_state == STEP_BEACON_LEAVE_OFF);
    
    // Should have brief flashes with long dark periods
    int flash_count = 0;
    
    for (unsigned long time = 0; time <= 2000; time += 20) {
        int state = beacon->step_beacon(time);
        if (state == STEP_BEACON_TURN_ON) {
            flash_count++;
        }
    }
    
    // Should have some flashes but not too many (lighthouse rotates slowly)
    TEST_ASSERT_GREATER_THAN_INT(0, flash_count);
    TEST_ASSERT_LESS_THAN_INT(10, flash_count);
    TEST_PASS_MESSAGE("Lighthouse pattern has brief flashes with long dark periods");
}

// =====================================================
// NON-REPEAT MODE TESTS
// =====================================================

void test_beacon_non_repeat_mode(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 50, 50, false);  // Non-repeating
    
    // Should eventually stop
    bool beacon_stopped = false;
    int final_state = STEP_BEACON_LEAVE_OFF;
    
    for (unsigned long time = 0; time <= 500; time += 10) {
        int state = beacon->step_beacon(time);
        if (state == STEP_BEACON_LEAVE_OFF) {
            final_state = state;
            // Check if it stays off
            int next_state = beacon->step_beacon(time + 100);
            if (next_state == STEP_BEACON_LEAVE_OFF) {
                beacon_stopped = true;
                break;
            }
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(beacon_stopped, "Non-repeating beacon should eventually stop");
}

// =====================================================
// EDGE CASES
// =====================================================

void test_beacon_zero_timing(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 0, 100, true);
    
    // Should handle zero timing gracefully
    int state1 = beacon->step_beacon(0);
    int state2 = beacon->step_beacon(1);
    
    // Should produce valid states
    TEST_ASSERT_TRUE(state1 >= STEP_BEACON_TURN_ON && state1 <= STEP_BEACON_LEAVE_OFF);
    TEST_ASSERT_TRUE(state2 >= STEP_BEACON_TURN_ON && state2 <= STEP_BEACON_LEAVE_OFF);
    
    TEST_PASS_MESSAGE("Beacon handles zero timing gracefully");
}

void test_beacon_large_timing_values(void) {
    beacon->start_beacon(BEACON_PATTERN_SIMPLE, 10000, 10000, true);
    
    // Should handle large timing values
    int state = beacon->step_beacon(5000);
    TEST_ASSERT_TRUE(state >= STEP_BEACON_TURN_ON && state <= STEP_BEACON_LEAVE_OFF);
    
    TEST_PASS_MESSAGE("Beacon handles large timing values");
}

// =====================================================
// MAIN TEST RUNNER
// =====================================================

int main() {
    UNITY_BEGIN();
    
    // Basic functionality
    RUN_TEST(test_beacon_basic_initialization);
    RUN_TEST(test_beacon_state_constants);
    RUN_TEST(test_beacon_pattern_constants);
    
    // Simple pattern tests
    RUN_TEST(test_beacon_simple_pattern_starts_with_turn_on);
    RUN_TEST(test_beacon_simple_pattern_transitions);
    RUN_TEST(test_beacon_timing_precision);
    
    // Pattern-specific tests
    RUN_TEST(test_beacon_fast_keyer_pattern);
    RUN_TEST(test_beacon_heartbeat_pattern);
    RUN_TEST(test_beacon_lighthouse_pattern);
    
    // Modes and edge cases
    RUN_TEST(test_beacon_non_repeat_mode);
    RUN_TEST(test_beacon_zero_timing);
    RUN_TEST(test_beacon_large_timing_values);
    
    return UNITY_END();
}
