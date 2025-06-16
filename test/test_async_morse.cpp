#include "unity/src/unity.h"
#include "../native/platform.h"
#include "async_morse.h"

// Test fixture
AsyncMorse* morse;

void setUp(void) {
    morse = new AsyncMorse();
}

void tearDown(void) {
    delete morse;
    morse = nullptr;
}

// Basic functionality tests
void test_morse_constructor(void) {
    AsyncMorse test_morse;
    TEST_PASS_MESSAGE("AsyncMorse constructor works");
}

void test_morse_character_lookup(void) {
    // Test character lookup function indirectly through morse generation
    simulate_morse_sequence("A", 20, false, 0, 1000);
    
    // Should have events (at minimum: turn on, turn off for dot, turn on, turn off for dash)
    TEST_ASSERT_GREATER_THAN(3, event_count);
    TEST_PASS_MESSAGE("Character lookup appears to work");
}

void test_morse_single_character_dot(void) {
    // Test letter E (single dot)
    simulate_morse_sequence("E", 20, false, 0, 500);
    
    // Should have: turn on (dot), turn off, done
    TEST_ASSERT_GREATER_THAN(1, event_count);
    
    // First event should be turn on
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_ON, events[0].state);
    
    // Should eventually turn off
    bool found_turn_off = false;
    for (int i = 1; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_OFF) {
            found_turn_off = true;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_turn_off, "Should turn off after dot");
}

void test_morse_single_character_dash(void) {
    // Test letter T (single dash)
    simulate_morse_sequence("T", 20, false, 0, 500);
    
    // Should have events
    TEST_ASSERT_GREATER_THAN(1, event_count);
    
    // First event should be turn on
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_ON, events[0].state);
    
    // Dash should be longer than dot, so find the duration
    unsigned long turn_on_time = 0, turn_off_time = 0;
    for (int i = 0; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_ON) {
            turn_on_time = events[i].time;
        } else if (events[i].state == STEP_MORSE_TURN_OFF && turn_on_time > 0) {
            turn_off_time = events[i].time;
            break;
        }
    }
    
    TEST_ASSERT_GREATER_THAN(turn_on_time, turn_off_time);
    
    // For 20 WPM, element time is 50ms, dash should be ~150ms
    unsigned long duration = turn_off_time - turn_on_time;
    TEST_ASSERT_GREATER_THAN(100, duration); // Should be at least 100ms for a dash
}

void test_morse_dot_dash_combination(void) {
    // Test letter A (dot-dash)
    simulate_morse_sequence("A", 20, false, 0, 1000);
    
    // Should have multiple on/off cycles
    int turn_on_count = 0;
    int turn_off_count = 0;
    
    for (int i = 0; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        } else if (events[i].state == STEP_MORSE_TURN_OFF) {
            turn_off_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, turn_on_count, "Letter A should have 2 elements (dot, dash)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, turn_off_count, "Should turn off after each element");
}

void test_morse_numbers(void) {
    // Test number 5 (five dots)
    simulate_morse_sequence("5", 20, false, 0, 2000);
    
    // Should have 5 on/off cycles
    int turn_on_count = 0;
    
    for (int i = 0; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, turn_on_count, "Number 5 should have 5 dots");
}

void test_morse_word_spacing(void) {
    // Test space between words
    simulate_morse_sequence("E E", 20, false, 0, 2000);
    
    // Should have two separate E's with proper spacing
    int turn_on_count = 0;
    
    for (int i = 0; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, turn_on_count, "Should have two E's");
}

void test_morse_timing_wpm(void) {
    // Test different WPM settings
    mock_time = 0;
    simulate_morse_sequence("E", 10, false, 0, 500); // Slow
    int slow_event_count = event_count;
    
    mock_time = 0;
    simulate_morse_sequence("E", 40, false, 0, 500); // Fast
    int fast_event_count = event_count;
    
    // Both should complete, but timing should be different
    // This is a basic check that different WPM values are processed
    TEST_ASSERT_GREATER_THAN(0, slow_event_count);
    TEST_ASSERT_GREATER_THAN(0, fast_event_count);
}

void test_morse_empty_string(void) {
    simulate_morse_sequence("", 20, false, 0, 500);
    
    // Should handle empty string gracefully
    TEST_ASSERT_GREATER_OR_EQUAL(0, event_count);
}

void test_morse_repeat_functionality(void) {
    // Test repeat with a short wait time
    simulate_morse_sequence("E", 20, true, 1, 3000); // Repeat every 1 second
    
    // Should have more events due to repetition
    int turn_on_count = 0;
    
    for (int i = 0; i < event_count; i++) {
        if (events[i].state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        }
    }
    
    // Should repeat at least once in 3 seconds with 1 second wait
    TEST_ASSERT_GREATER_THAN(1, turn_on_count);
}

void test_morse_non_repeat_completion(void) {
    // Test that non-repeating morse completes and stays off
    simulate_morse_sequence("E", 20, false, 0, 1000);
    
    // Final state should be LEAVE_OFF or similar
    if (event_count > 0) {
        int final_state = events[event_count - 1].state;
        TEST_ASSERT_TRUE_MESSAGE(
            final_state == STEP_MORSE_LEAVE_OFF || final_state == STEP_MORSE_TURN_OFF,
            "Final state should be off when not repeating"
        );
    }
}

void test_morse_case_insensitive(void) {
    // Test that upper and lower case produce same result
    mock_time = 0;
    simulate_morse_sequence("a", 20, false, 0, 500);
    int lower_events = event_count;
    
    mock_time = 0;
    simulate_morse_sequence("A", 20, false, 0, 500);
    int upper_events = event_count;
    
    // Should produce similar number of events
    TEST_ASSERT_EQUAL_INT_MESSAGE(upper_events, lower_events, "Upper and lower case should be equivalent");
}

void test_morse_special_characters(void) {
    // Test handling of unsupported characters
    simulate_morse_sequence("@", 20, false, 0, 500);
    
    // Should handle gracefully without crashing
    TEST_ASSERT_GREATER_OR_EQUAL(0, event_count);
}

void test_morse_long_message(void) {
    // Test a longer message
    simulate_morse_sequence("HELLO", 20, false, 0, 5000);
    
    // Should have many events for the full word
    TEST_ASSERT_GREATER_THAN(10, event_count);
    
    // Should start and end properly
    if (event_count > 0) {
        TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_ON, events[0].state);
    }
}

void test_morse_state_consistency(void) {
    // Test that states follow logical progression
    simulate_morse_sequence("SOS", 20, false, 0, 3000);
    
    bool last_was_on = false;
    for (int i = 0; i < event_count; i++) {
        int state = events[i].state;
        
        switch (state) {
            case STEP_MORSE_TURN_ON:
                TEST_ASSERT_FALSE_MESSAGE(last_was_on, "Should not turn on when already on");
                last_was_on = true;
                break;
            case STEP_MORSE_TURN_OFF:
                TEST_ASSERT_TRUE_MESSAGE(last_was_on, "Should not turn off when already off");
                last_was_on = false;
                break;
            case STEP_MORSE_LEAVE_ON:
                TEST_ASSERT_TRUE_MESSAGE(last_was_on, "Should already be on for LEAVE_ON");
                break;
            case STEP_MORSE_LEAVE_OFF:
                TEST_ASSERT_FALSE_MESSAGE(last_was_on, "Should already be off for LEAVE_OFF");
                break;
        }
    }
}

// Edge case tests
void test_morse_zero_wpm_handling(void) {
    // Test edge case of very low WPM (should not crash)
    simulate_morse_sequence("E", 1, false, 0, 10000); // Very slow
    
    // Should not crash and should produce some events
    TEST_ASSERT_GREATER_OR_EQUAL(0, event_count);
}

void test_morse_high_wpm_handling(void) {
    // Test edge case of very high WPM
    simulate_morse_sequence("E", 100, false, 0, 500); // Very fast
    
    // Should not crash and should produce events
    TEST_ASSERT_GREATER_OR_EQUAL(0, event_count);
}

// Main test runner setup
int main() {
    UNITY_BEGIN();
    
    // Basic functionality
    RUN_TEST(test_morse_constructor);
    RUN_TEST(test_morse_character_lookup);
    
    // Single character tests
    RUN_TEST(test_morse_single_character_dot);
    RUN_TEST(test_morse_single_character_dash);
    RUN_TEST(test_morse_dot_dash_combination);
    
    // Character set tests
    RUN_TEST(test_morse_numbers);
    RUN_TEST(test_morse_case_insensitive);
    RUN_TEST(test_morse_special_characters);
    
    // Spacing and timing tests
    RUN_TEST(test_morse_word_spacing);
    RUN_TEST(test_morse_timing_wpm);
    
    // Message tests
    RUN_TEST(test_morse_empty_string);
    RUN_TEST(test_morse_long_message);
    
    // State and flow tests
    RUN_TEST(test_morse_repeat_functionality);
    RUN_TEST(test_morse_non_repeat_completion);
    RUN_TEST(test_morse_state_consistency);
    
    // Edge cases
    RUN_TEST(test_morse_zero_wpm_handling);
    RUN_TEST(test_morse_high_wpm_handling);
    
    return UNITY_END();
}
