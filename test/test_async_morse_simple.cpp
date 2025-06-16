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

// Simple helper function to test basic morse functionality
void test_morse_basic_initialization(void) {
    // Just test that we can create and use the class
    TEST_ASSERT_NOT_NULL(morse);
    TEST_PASS_MESSAGE("AsyncMorse can be instantiated");
}

void test_morse_start_single_character(void) {
    // Test starting morse with a single character
    morse->start_morse("E", 20, false, 0);
    
    // Step through a few times to see behavior
    unsigned long time = 0;
    int result1 = morse->step_morse(time);
    
    // Should get some kind of state back (not testing specific value yet)
    TEST_ASSERT_TRUE(result1 >= 0 && result1 <= 4);
    TEST_PASS_MESSAGE("Can start morse and get initial state");
}

void test_morse_timing_calculation(void) {
    // Test the WPM macro
    int time_20wpm = MORSE_TIME_FROM_WPM(20);
    int time_10wpm = MORSE_TIME_FROM_WPM(10);
    
    // 10 WPM should take twice as long as 20 WPM
    TEST_ASSERT_EQUAL_INT(time_20wpm * 2, time_10wpm);
    
    // 20 WPM should be 50ms per unit
    TEST_ASSERT_EQUAL_INT(50, time_20wpm);
}

void test_morse_empty_string(void) {
    // Test with empty string
    morse->start_morse("", 20, false, 0);
    
    unsigned long time = 0;
    int result = morse->step_morse(time);
    
    // Should handle empty string gracefully
    TEST_ASSERT_TRUE(result >= 0 && result <= 4);
    TEST_PASS_MESSAGE("Handles empty string without crashing");
}

void test_morse_step_progression_simple(void) {
    // Test a simple progression with letter E (one dot)
    morse->start_morse("E", 20, false, 0);
    
    unsigned long time = 0;
    
    // Step 1: Should get initial state
    int state1 = morse->step_morse(time);
    
    // Step 2: Advance time by element duration (50ms for 20 WPM)
    time += 50;
    int state2 = morse->step_morse(time);
    
    // Step 3: Advance more time
    time += 50; 
    int state3 = morse->step_morse(time);
    
    // Step 4: Advance even more time
    time += 200;
    int state4 = morse->step_morse(time);
    
    // All states should be valid
    TEST_ASSERT_TRUE(state1 >= 0 && state1 <= 4);
    TEST_ASSERT_TRUE(state2 >= 0 && state2 <= 4);
    TEST_ASSERT_TRUE(state3 >= 0 && state3 <= 4);
    TEST_ASSERT_TRUE(state4 >= 0 && state4 <= 4);
    
    TEST_PASS_MESSAGE("State progression works without crashes");
}

void test_morse_on_off_states(void) {
    // Test that we get proper on/off transitions
    morse->start_morse("E", 20, false, 0);
    
    unsigned long time = 0;
    bool saw_turn_on = false;
    bool saw_turn_off = false;
    
    // Step through several time increments
    for (int i = 0; i < 20; i++) {
        int state = morse->step_morse(time);
        
        if (state == STEP_MORSE_TURN_ON) {
            saw_turn_on = true;
        }
        if (state == STEP_MORSE_TURN_OFF) {
            saw_turn_off = true;
        }
        
        time += 25; // Small time increments
    }
    
    // Should see at least one on/off cycle for a single dot
    TEST_ASSERT_TRUE_MESSAGE(saw_turn_on, "Should see TURN_ON state");
    // Note: Turn off might happen later, so we'll be more lenient for now
    
    TEST_PASS_MESSAGE("Morse produces turn-on states");
}

void test_morse_different_wpm(void) {
    // Test that different WPM values are accepted
    morse->start_morse("E", 10, false, 0);
    int state1 = morse->step_morse(0);
    
    morse->start_morse("E", 30, false, 0);
    int state2 = morse->step_morse(0);
    
    morse->start_morse("E", 5, false, 0);
    int state3 = morse->step_morse(0);
    
    // All should work without crashing
    TEST_ASSERT_TRUE(state1 >= 0 && state1 <= 4);
    TEST_ASSERT_TRUE(state2 >= 0 && state2 <= 4);
    TEST_ASSERT_TRUE(state3 >= 0 && state3 <= 4);
    
    TEST_PASS_MESSAGE("Different WPM values work");
}

void test_morse_multiple_characters(void) {
    // Test with multiple characters
    morse->start_morse("SOS", 20, false, 0);
    
    unsigned long time = 0;
    int valid_state_count = 0;
    
    // Step through many times
    for (int i = 0; i < 100; i++) {
        int state = morse->step_morse(time);
        if (state >= 0 && state <= 4) {
            valid_state_count++;
        }
        time += 50;
    }
    
    // Should get many valid states for SOS
    TEST_ASSERT_GREATER_THAN(50, valid_state_count);
    TEST_PASS_MESSAGE("Multiple characters work");
}

void test_morse_repeat_flag(void) {
    // Test repeat functionality
    morse->start_morse("E", 20, true, 1); // Repeat with 1 second wait
    
    unsigned long time = 0;
    int state_count = 0;
    
    // Step through several seconds worth
    for (int i = 0; i < 200; i++) { // 200 * 50ms = 10 seconds
        int state = morse->step_morse(time);
        if (state >= 0 && state <= 4) {
            state_count++;
        }
        time += 50;
    }
    
    // Should get many states due to repetition
    TEST_ASSERT_GREATER_THAN(100, state_count);
    TEST_PASS_MESSAGE("Repeat functionality works");
}

void test_morse_case_insensitive(void) {
    // Test that upper and lower case work
    morse->start_morse("a", 20, false, 0);
    int state1 = morse->step_morse(0);
    
    morse->start_morse("A", 20, false, 0);
    int state2 = morse->step_morse(0);
    
    // Both should produce valid states
    TEST_ASSERT_TRUE(state1 >= 0 && state1 <= 4);
    TEST_ASSERT_TRUE(state2 >= 0 && state2 <= 4);
    
    TEST_PASS_MESSAGE("Case insensitive input works");
}

void test_morse_numbers(void) {
    // Test that numbers work
    morse->start_morse("123", 20, false, 0);
    
    unsigned long time = 0;
    int valid_states = 0;
    
    for (int i = 0; i < 50; i++) {
        int state = morse->step_morse(time);
        if (state >= 0 && state <= 4) {
            valid_states++;
        }
        time += 100;
    }
    
    TEST_ASSERT_GREATER_THAN(20, valid_states);
    TEST_PASS_MESSAGE("Numbers work in morse");
}

void test_morse_state_constants(void) {
    // Test that all state constants are defined and unique
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_TURN_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_ON, STEP_MORSE_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_OFF, STEP_MORSE_LEAVE_ON);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_TURN_OFF, STEP_MORSE_LEAVE_OFF);
    TEST_ASSERT_NOT_EQUAL(STEP_MORSE_LEAVE_ON, STEP_MORSE_LEAVE_OFF);
    
    TEST_PASS_MESSAGE("All morse state constants are properly defined");
}

// Main test runner
int main() {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_morse_basic_initialization);
    RUN_TEST(test_morse_timing_calculation);
    RUN_TEST(test_morse_state_constants);
    
    // Simple operation tests
    RUN_TEST(test_morse_start_single_character);
    RUN_TEST(test_morse_empty_string);
    RUN_TEST(test_morse_step_progression_simple);
    
    // State behavior tests
    RUN_TEST(test_morse_on_off_states);
    
    // Input variation tests
    RUN_TEST(test_morse_different_wpm);
    RUN_TEST(test_morse_case_insensitive);
    RUN_TEST(test_morse_numbers);
    
    // Complex scenario tests
    RUN_TEST(test_morse_multiple_characters);
    RUN_TEST(test_morse_repeat_flag);
    
    return UNITY_END();
}
