#include "unity/src/unity.h"
#include "../native/platform.h"
#include "../include/async_morse.h"

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
        int state = morse->step_morse(time);        TEST_ASSERT_TRUE_MESSAGE(
            state == STEP_MORSE_TURN_ON || 
            state == STEP_MORSE_TURN_OFF || 
            state == STEP_MORSE_LEAVE_ON || 
            state == STEP_MORSE_LEAVE_OFF ||
            state == STEP_MORSE_MESSAGE_COMPLETE,
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

void test_morse_complete_cycle_phases(void) {
    // Test that a complete morse sequence goes through expected phases
    morse->start_morse("A", 20, false, 0);
    
    bool completed = false;
    for (unsigned long time = 0; time <= 1000; time += 5) {
        int state = morse->step_morse(time);
        // After sufficient time, should return to inactive state
        if (time > 500 && state == STEP_MORSE_LEAVE_OFF) {
            completed = true;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(completed, "Morse should complete and go inactive");
}

void test_morse_wait_parameter(void) {
    // Test different wait times in repeat mode
    morse->start_morse("E", 20, true, 2); // 2 second wait
    
    int first_cycle_turn_ons = 0;
    int second_cycle_turn_ons = 0;
    
    // Count first cycle (0-1000ms)
    for (unsigned long time = 0; time <= 1000; time += 10) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            first_cycle_turn_ons++;
        }
    }
    
    // Count around second cycle (2000-3000ms, accounting for 2s wait)
    for (unsigned long time = 2000; time <= 3000; time += 10) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            second_cycle_turn_ons++;
        }
    }
    
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, first_cycle_turn_ons, "Should have activity in first cycle");
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, second_cycle_turn_ons, "Should repeat after wait period");
}

void test_morse_extreme_wpm_values(void) {
    // Test very slow WPM
    morse->start_morse("E", 1, false, 0); // 1 WPM
    bool found_turn_on = false;
    for (unsigned long time = 0; time <= 2000; time += 50) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            found_turn_on = true;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on, "Should work with very slow WPM");
    
    // Test very fast WPM  
    morse->start_morse("E", 100, false, 0); // 100 WPM
    found_turn_on = false;
    for (unsigned long time = 0; time <= 100; time += 1) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            found_turn_on = true;
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on, "Should work with very fast WPM");
}

void test_morse_multi_character_string(void) {
    // Test "SOS" - should have 9 elements total (3+3+3)
    morse->start_morse("SOS", 20, false, 0);
    
    int turn_on_count = 0;
    for (unsigned long time = 0; time <= 3000; time += 5) {
        if (morse->step_morse(time) == STEP_MORSE_TURN_ON) {
            turn_on_count++;
        }
    }
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(9, turn_on_count, "SOS should have 9 elements (3+3+3)");
}

void test_morse_character_spacing(void) {
    // Test that there are gaps between characters
    morse->start_morse("EE", 20, false, 0); // Two E's
    
    bool found_gap = false;
    bool found_first_turn_off = false;
    
    for (unsigned long time = 0; time <= 1000; time += 5) {
        int state = morse->step_morse(time);
        
        if (state == STEP_MORSE_TURN_OFF) {
            found_first_turn_off = true;
        }
        
        // Look for extended LEAVE_OFF period (character spacing)
        if (found_first_turn_off && state == STEP_MORSE_LEAVE_OFF) {
            // Check if we stay in LEAVE_OFF for multiple time steps
            int leave_off_count = 0;
            for (unsigned long t2 = time; t2 <= time + 50 && t2 <= 1000; t2 += 5) {
                if (morse->step_morse(t2) == STEP_MORSE_LEAVE_OFF) {
                    leave_off_count++;
                } else {
                    break;
                }
            }
            if (leave_off_count > 3) { // Extended gap indicates character spacing
                found_gap = true;
                break;
            }
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(found_gap, "Should have spacing between characters");
}

// =====================================================
// TIMING PRECISION TESTS
// =====================================================

void test_morse_timing_precision(void) {
    // Test that dot and dash have correct 1:3 ratio
    
    // Measure dot duration (E = single dot)
    morse->start_morse("E", 20, false, 0);
    unsigned long dot_start = 0, dot_end = 0;
    bool found_turn_on = false;
    
    for (unsigned long time = 0; time <= 200; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON && !found_turn_on) {
            dot_start = time;
            found_turn_on = true;
        }
        if (found_turn_on && state == STEP_MORSE_TURN_OFF) {
            dot_end = time;
            break;
        }
    }
    
    unsigned long dot_duration = dot_end - dot_start;
    
    // Measure dash duration (T = single dash)
    morse->start_morse("T", 20, false, 0);
    unsigned long dash_start = 0, dash_end = 0;
    found_turn_on = false;
    
    for (unsigned long time = 0; time <= 200; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON && !found_turn_on) {
            dash_start = time;
            found_turn_on = true;
        }
        if (found_turn_on && state == STEP_MORSE_TURN_OFF) {
            dash_end = time;
            break;
        }
    }
    
    unsigned long dash_duration = dash_end - dash_start;
    
    // Dash should be approximately 3x dot duration
    TEST_ASSERT_GREATER_THAN_MESSAGE(dot_duration * 2, dash_duration, "Dash should be longer than dot");
    TEST_ASSERT_LESS_THAN_MESSAGE(dot_duration * 4, dash_duration, "Dash should not be too long");
}

void test_morse_state_transition_order(void) {
    // Test that TURN_ON is always followed by LEAVE_ON, then TURN_OFF
    morse->start_morse("E", 20, false, 0);
    
    int prev_state = STEP_MORSE_LEAVE_OFF;
    bool valid_transitions = true;
    
    for (unsigned long time = 0; time <= 500; time += 5) {
        int state = morse->step_morse(time);
        
        // Check for invalid transition patterns
        if (prev_state == STEP_MORSE_TURN_ON && state != STEP_MORSE_LEAVE_ON) {
            valid_transitions = false;
            break;
        }
        if (prev_state == STEP_MORSE_TURN_OFF && state != STEP_MORSE_LEAVE_OFF) {
            valid_transitions = false;
            break;
        }
        
        prev_state = state;
    }
    
    TEST_ASSERT_TRUE_MESSAGE(valid_transitions, "State transitions should follow valid patterns");
}

// =====================================================
// BUG DETECTION TESTS
// =====================================================

void test_morse_element_spacing_timing(void) {
    // Test that element spacing scales with WPM
    // Element spacing should be 1 unit, which should scale with WPM
    
    // Test at 20 WPM (50ms per unit)
    morse->start_morse("A", 20, false, 0); // A = dot-dash
    unsigned long first_turn_off_20wpm = 0;
    unsigned long second_turn_on_20wpm = 0;
    bool found_first_off = false;
    
    for (unsigned long time = 0; time <= 500; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_OFF && !found_first_off) {
            first_turn_off_20wpm = time;
            found_first_off = true;
        }
        if (found_first_off && state == STEP_MORSE_TURN_ON && second_turn_on_20wpm == 0) {
            second_turn_on_20wpm = time;
            break;
        }
    }
    
    unsigned long element_spacing_20wpm = second_turn_on_20wpm - first_turn_off_20wpm;
    
    // Test at 40 WPM (25ms per unit) 
    morse->start_morse("A", 40, false, 0);
    unsigned long first_turn_off_40wpm = 0;
    unsigned long second_turn_on_40wpm = 0;
    found_first_off = false;
    
    for (unsigned long time = 0; time <= 250; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_OFF && !found_first_off) {
            first_turn_off_40wpm = time;
            found_first_off = true;
        }
        if (found_first_off && state == STEP_MORSE_TURN_ON && second_turn_on_40wpm == 0) {
            second_turn_on_40wpm = time;
            break;
        }
    }
    
    unsigned long element_spacing_40wpm = second_turn_on_40wpm - first_turn_off_40wpm;
    
    // Element spacing should scale with WPM: 20 WPM should be ~2x slower than 40 WPM
    TEST_ASSERT_GREATER_THAN_MESSAGE(element_spacing_40wpm, element_spacing_20wpm, 
                                      "Element spacing should scale with WPM");
    
    // Check if the ratio is approximately 2:1 (within 25% tolerance)
    unsigned long expected_ratio = element_spacing_20wpm / element_spacing_40wpm;
    TEST_ASSERT_GREATER_THAN_MESSAGE(1, expected_ratio, "WPM ratio should be greater than 1");
    TEST_ASSERT_LESS_THAN_MESSAGE(3, expected_ratio, "WPM ratio should be reasonable");
}

void test_morse_immediate_start(void) {
    // Test that morse starts immediately without unnecessary delay
    morse->start_morse("E", 20, false, 0);
    
    // Should get TURN_ON very quickly (within first few milliseconds)
    bool found_turn_on_early = false;
    for (unsigned long time = 0; time <= 10; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON) {
            found_turn_on_early = true;
            break;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on_early, 
                            "Morse should start immediately without unnecessary startup delay");
}

void test_morse_character_spacing_wpm_dependency(void) {
    // Test that character spacing (between letters) scales with WPM
    
    // Test "AB" at 20 WPM - find gap between A and B
    morse->start_morse("AB", 20, false, 0);
    unsigned long char_gap_start_20wpm = 0;
    unsigned long char_gap_end_20wpm = 0;
    int turn_on_count = 0;
    bool in_gap = false;
    
    for (unsigned long time = 0; time <= 1000; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
            if (turn_on_count == 2 && !in_gap) {
                // End of first character (A has 2 elements)
                in_gap = true;
            } else if (turn_on_count == 3 && in_gap) {
                // Start of second character (B)
                char_gap_end_20wpm = time;
                break;
            }
        }
        if (in_gap && char_gap_start_20wpm == 0 && state == STEP_MORSE_LEAVE_OFF) {
            char_gap_start_20wpm = time;
        }
    }
    
    unsigned long char_spacing_20wpm = char_gap_end_20wpm - char_gap_start_20wpm;
    
    // Test same at 40 WPM
    morse->start_morse("AB", 40, false, 0);
    unsigned long char_gap_start_40wpm = 0;
    unsigned long char_gap_end_40wpm = 0;
    turn_on_count = 0;
    in_gap = false;
    
    for (unsigned long time = 0; time <= 500; time += 1) {
        int state = morse->step_morse(time);
        if (state == STEP_MORSE_TURN_ON) {
            turn_on_count++;
            if (turn_on_count == 2 && !in_gap) {
                in_gap = true;
            } else if (turn_on_count == 3 && in_gap) {
                char_gap_end_40wpm = time;
                break;
            }
        }
        if (in_gap && char_gap_start_40wpm == 0 && state == STEP_MORSE_LEAVE_OFF) {
            char_gap_start_40wpm = time;
        }
    }
    
    unsigned long char_spacing_40wpm = char_gap_end_40wpm - char_gap_start_40wpm;
    
    // Character spacing should also scale with WPM
    TEST_ASSERT_GREATER_THAN_MESSAGE(char_spacing_40wpm, char_spacing_20wpm,
                                      "Character spacing should scale with WPM");
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
    RUN_TEST(test_morse_complete_cycle_phases);
    RUN_TEST(test_morse_wait_parameter);
    RUN_TEST(test_morse_extreme_wpm_values);
    RUN_TEST(test_morse_multi_character_string);
    RUN_TEST(test_morse_character_spacing);
    
    // Timing precision
    RUN_TEST(test_morse_timing_precision);
    RUN_TEST(test_morse_state_transition_order);
    
    // Bug detection
    RUN_TEST(test_morse_element_spacing_timing);
    RUN_TEST(test_morse_immediate_start);
    RUN_TEST(test_morse_character_spacing_wpm_dependency);
    
    return UNITY_END();
}
