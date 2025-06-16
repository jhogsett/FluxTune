#include <Arduino.h>
#include <unity.h>
#include "async_morse.h"

static AsyncMorse morse;  // Make it static to avoid multiple definition

void test_single_letter_A(void) {
    morse.start_morse("A", 20, false, 0);
    unsigned long time = 0;
    int onCount = 0, offCount = 0;
    
    // Step through enough time to generate the letter
    for (int i = 0; i < 100; i++) {
        int state = morse.step_morse(time);
        if (state == STEP_MORSE_TURN_ON) onCount++;
        if (state == STEP_MORSE_TURN_OFF) offCount++;
        time += 50;  // 50ms steps
    }
    
    // A (.-) should have 2 ON transitions (dot and dash) and 2 OFF transitions
    TEST_ASSERT_EQUAL(2, onCount);
    TEST_ASSERT_EQUAL(2, offCount);
}

void test_sos(void) {
    morse.start_morse("SOS", 20, false, 0);
    unsigned long time = 0;
    int onCount = 0, offCount = 0;
    
    // Step through enough time to generate "SOS"
    for (int i = 0; i < 300; i++) {
        int state = morse.step_morse(time);
        if (state == STEP_MORSE_TURN_ON) onCount++;
        if (state == STEP_MORSE_TURN_OFF) offCount++;
        time += 50;
    }
    
    // SOS (...---...) should have 9 ON transitions and 9 OFF transitions
    TEST_ASSERT_EQUAL(9, onCount);
    TEST_ASSERT_EQUAL(9, offCount);
}

void setUp(void) {
    // Reset before each test
}

void tearDown(void) {
    // Clean up after each test
}

void setup() {
#ifdef ARDUINO
    delay(2000);
#endif
    UNITY_BEGIN();
    RUN_TEST(test_single_letter_A);
    RUN_TEST(test_sos);
    UNITY_END();
}

void loop() {
}
