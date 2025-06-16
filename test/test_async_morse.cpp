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

// =====================================================
// BASIC FUNCTIONALITY TESTS
// =====================================================

void test_morse_basic_initialization(void) {
    TEST_ASSERT_NOT_NULL(morse);
    TEST_PASS_MESSAGE("AsyncMorse can be instantiated");
}

void test_morse_timing_calculation(void) {
    int time_20wpm = MORSE_TIME_FROM_WPM(20);
    int time_10wpm = MORSE_TIME_FROM_WPM(10);
    
    TEST_ASSERT_EQUAL_INT(time_20wpm * 2, time_10wpm);
    TEST_ASSERT_EQUAL_INT(50, time_20wpm);
}

void test_morse_state_constants(void) {
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_TURN_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_OFF, STEP_MORSE_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_OFF, STEP_MORSE_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_LEAVE_ON, STEP_MORSE_LEAVE_OFF);
    
    TEST_PASS_MESSAGE("All morse state constants are properly defined");
}

// =====================================================
// BEHAVIORAL VERIFICATION TESTS
// =====================================================

void test_morse_starts_with_turn_on(void) {
    // Test that morse eventually produces TURN_ON (accounting for startup delay)
    morse->start_morse("E", 20, false, 0);
    
    bool found_turn_on = false;
    int first_non_leave_off_state = -1;
    
    // Look for the first TURN_ON within a reasonable time window
    for (unsigned long time = 0; time <= 100; time += 5) {
        int state = morse->step_morse(time);
        
        // Record the first non-LEAVE_OFF state we see
        if (first_non_leave_off_state == -1 && state != STEP_MORSE_LEAVE_OFF) {
            first_non_leave_off_state = state;
        }
        
        if (state == STEP_MORSE_TURN_ON) {
            found_turn_on = true;
            break;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on, "Should eventually produce TURN_ON");
    
    // If there's a startup delay, the first active state should still be TURN_ON
    if (first_non_leave_off_state != -1) {
        TEST_ASSERT_EQUAL_INT_MESSAGE(STEP_MORSE_TURN_ON, first_non_leave_off_state, 
                                      "First active state should be TURN_ON");
    }
}

void test_morse_produces_valid_states(void) {
    // Test that all states returned are valid
    morse->start_morse("E", 20, false, 0);
    
    for (unsigned long time = 0; time <= 200; time += 5) {
        int state = morse->step_morse(time);
        TEST_ASSERT_TRUE_MESSAGE(
            state == STEP_MORSE_TURN_ON || 
            state == STEP_MORSE_TURN_OFF || 
            state == STEP_MORSE_LEAVE_ON || 
            state == STEP_MORSE_LEAVE_OFF,
            "All states should be valid morse states"
        );
    }
}

void test_morse_has_on_off_transitions(void) {
    // Test that we see both on and off states
    morse->start_morse("E", 20, false, 0);
    
    bool saw_turn_on = false;
    bool saw_turn_off = false;
    bool saw_leave_on = false;
    bool saw_leave_off = false;
    
    for (unsigned long time = 0; time <= 200; time += 5) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON) saw_turn_on = true;
        if (state == STEP_MORSE_TURN_OFF) saw_turn_off = true;
        if (state == STEP_MORSE_LEAVE_ON) saw_leave_on = true;
        if (state == STEP_MORSE_LEAVE_OFF) saw_leave_off = true;
    }
    
    TEST_ASSERT_TRUE_MESSAGE(saw_turn_on, "Should see TURN_ON");
    TEST_ASSERT_TRUE_MESSAGE(saw_turn_off, "Should see TURN_OFF");
    TEST_ASSERT_TRUE_MESSAGE(saw_leave_on, "Should see LEAVE_ON");
    TEST_ASSERT_TRUE_MESSAGE(saw_leave_off, "Should see LEAVE_OFF");
}

void test_morse_different_characters_different_patterns(void) {
    // Test that different characters produce different numbers of turn-ons
    
    // Count turn-ons for E (1 dot)
    morse->start_morse("E", 20, false, 0);
    int e_turn_ons = 0;
    for (unsigned long time = 0; time <= 500; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            e_turn_ons++;
        }
    }
    
    // Count turn-ons for A (dot-dash = 2 elements)
    morse->start_morse("A", 20, false, 0);
    int a_turn_ons = 0;
    for (unsigned long time = 0; time <= 500; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            a_turn_ons++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, e_turn_ons, "E should have 1 element");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, a_turn_ons, "A should have 2 elements");
}

void test_morse_number_5_has_5_elements(void) {
    // Test that number 5 has 5 turn-ons (5 dots)
    morse->start_morse("5", 20, false, 0);
    
    int turn_on_count = 0;
    for (unsigned long time = 0; time <= 2000; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(5, turn_on_count, "Number 5 should have 5 dots");
}

void test_morse_wpm_affects_timing(void) {
    // Test that different WPM values affect when turn-off occurs
    
    // Find turn-off time for 10 WPM
    morse->start_morse("E", 10, false, 0);
    unsigned long turn_off_time_10wpm = 0;
    for (unsigned long time = 0; time <= 300; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_OFF) {
            turn_off_time_10wpm = time;
            break;
        }
    }
    
    // Find turn-off time for 40 WPM
    morse->start_morse("E", 40, false, 0);
    unsigned long turn_off_time_40wpm = 0;
    for (unsigned long time = 0; time <= 100; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_OFF) {
            turn_off_time_40wpm = time;
            break;
        }
    }
    
    // 10 WPM should be slower than 40 WPM
    TEST_ASSERT_GREATER_THAN_MESSAGE(turn_off_time_40wpm, turn_off_time_10wpm, 
                                      "10 WPM should be slower than 40 WPM");
}

// =====================================================
// EDGE CASE TESTS
// =====================================================

void test_morse_empty_string(void) {
    morse->start_morse("", 20, false, 0);
    int state = morse->step_morse(0);
    TEST_ASSERT_TRUE(state >= 0 && state <= 4);
    TEST_PASS_MESSAGE("Empty string handled gracefully");
}

void test_morse_case_insensitive(void) {
    // Test that 'a' and 'A' produce same pattern
    morse->start_morse("a", 20, false, 0);
    int a_turn_ons = 0;
    for (unsigned long time = 0; time <= 500; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            a_turn_ons++;
        }
    }
    
    morse->start_morse("A", 20, false, 0);
    int A_turn_ons = 0;
    for (unsigned long time = 0; time <= 500; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            A_turn_ons++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(A_turn_ons, a_turn_ons, "Upper and lower case should be the same");
}

void test_morse_invalid_characters(void) {
    morse->start_morse("@#$", 20, false, 0);
    
    // Should not crash and should produce valid states
    for (unsigned long time = 0; time <= 200; time += 10) {
        int state = morse->step_morse(time);
        TEST_ASSERT_TRUE(state >= 0 && state <= 4);
    }
    TEST_PASS_MESSAGE("Invalid characters handled gracefully");
}

void test_morse_repeat_vs_non_repeat(void) {
    // Test that repeat produces more activity than non-repeat
    
    // Non-repeat: count turn-ons over 3 seconds
    morse->start_morse("E", 20, false, 0);
    int non_repeat_turn_ons = 0;
    for (unsigned long time = 0; time <= 3000; time += 10) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            non_repeat_turn_ons++;
        }
    }
    
    // Repeat: count turn-ons over 3 seconds
    morse->start_morse("E", 20, true, 1); // Repeat every 1 second
    int repeat_turn_ons = 0;
    for (unsigned long time = 0; time <= 3000; time += 10) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            repeat_turn_ons++;
        }
    }
    
    // Repeat should have more turn-ons
    TEST_ASSERT_GREATER_THAN_MESSAGE(non_repeat_turn_ons, repeat_turn_ons, 
                                      "Repeat should have more activity than non-repeat");
}

// Main test runner
int main() {
    UNITY_BEGIN();
    
    // Basic functionality
    RUN_TEST(test_morse_basic_initialization);
    RUN_TEST(test_morse_timing_calculation);
    RUN_TEST(test_morse_state_constants);
    
    // Behavioral verification
    RUN_TEST(test_morse_starts_with_turn_on);
    RUN_TEST(test_morse_produces_valid_states);
    RUN_TEST(test_morse_has_on_off_transitions);
    RUN_TEST(test_morse_different_characters_different_patterns);
    RUN_TEST(test_morse_number_5_has_5_elements);
    RUN_TEST(test_morse_wpm_affects_timing);
    
    // Edge cases
    RUN_TEST(test_morse_empty_string);
    RUN_TEST(test_morse_case_insensitive);
    RUN_TEST(test_morse_invalid_characters);
    RUN_TEST(test_morse_repeat_vs_non_repeat);
    
    return UNITY_END();
}
