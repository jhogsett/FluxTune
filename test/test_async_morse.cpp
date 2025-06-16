#ifdef UNIT_TEST_NATIVE
#include "mock_arduino_for_native.h" // Must be first for native mocks
#else
#include <Arduino.h> // For device compilation
#endif

#include <unity.h>
#include "async_morse.h" // Should be found by PlatformIO's build system

// Mock for millis()
static unsigned long mock_current_time_ms = 0;

unsigned long millis() {
    return mock_current_time_ms;
}

void set_mock_millis_time(unsigned long time_ms) {
    mock_current_time_ms = time_ms;
}

// Mock for yield() - can be a no-op for many tests
void yield() {}

// Optional setUp and tearDown, can be empty if not needed for these tests
void setUp(void) {
    // This function is called before each test
}

void tearDown(void) {
    // This function is called after each test
}

void test_char_E_produces_correct_sequence(void) {
    AsyncMorse morse_gen;
    unsigned long current_time = 0;
    int wpm_arg = 20;
    // Hypothesis: the internal unit time is behaving as if it's 53ms.
    unsigned long internal_unit_time = 53;

    morse_gen.start_morse("E", wpm_arg, false, 0); // "E" is one DIT: .

    // 1. Initial call at t=0. Expect pre-element space.
    //    step_morse(0) should return LEAVE_OFF.
    //    (Assumes 0 < internal_unit_time for pre-element space)
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_OFF, morse_gen.step_morse(current_time));

    // 2. Advance time to hypothesized start of DIT (t = internal_unit_time = 53ms).
    //    step_morse(53) should turn ON.
    current_time = internal_unit_time;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_ON, morse_gen.step_morse(current_time)); // Line 29 if this is the version

    // 3. Advance time to just before DIT ends (DIT duration is also internal_unit_time).
    current_time = internal_unit_time + internal_unit_time - 1;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_ON, morse_gen.step_morse(current_time));

    // 4. Advance time to exactly when DIT ends.
    current_time = internal_unit_time + internal_unit_time;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_OFF, morse_gen.step_morse(current_time));

    // 5. Advance time to just before inter-element space ends.
    current_time = internal_unit_time * 3 - 1;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_OFF, morse_gen.step_morse(current_time));

    // 6. Advance time to when inter-element space ends (char "E" is complete).
    current_time = internal_unit_time * 3;
    TEST_ASSERT_EQUAL_INT(PHASE_DONE, morse_gen.step_morse(current_time));
}

void test_char_T_produces_correct_sequence(void) {
    AsyncMorse morse_gen;
    unsigned long current_time = 0;
    int wpm_arg = 20;
    // Hypothesis: the internal unit time is behaving as if it's 53ms.
    unsigned long internal_unit_time = 53;
    unsigned long internal_dah_duration = 3 * internal_unit_time;

    morse_gen.start_morse("T", wpm_arg, false, 0); // "T" is one DAH: -

    // 1. Initial call at t=0.
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_OFF, morse_gen.step_morse(current_time));

    // 2. Advance time to hypothesized start of DAH (t = internal_unit_time = 53ms).
    current_time = internal_unit_time;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_ON, morse_gen.step_morse(current_time)); // Line 68 if this is the version

    // 3. Advance time to just before DAH ends.
    current_time = internal_unit_time + internal_dah_duration - 1;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_ON, morse_gen.step_morse(current_time));

    // 4. Advance time to exactly when DAH ends.
    current_time = internal_unit_time + internal_dah_duration;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_TURN_OFF, morse_gen.step_morse(current_time));

    // 5. Advance time to just before inter-element space ends.
    current_time = internal_unit_time + internal_dah_duration + internal_unit_time - 1;
    TEST_ASSERT_EQUAL_INT(STEP_MORSE_LEAVE_OFF, morse_gen.step_morse(current_time));

    // 6. Advance time to when inter-element space ends.
    current_time = internal_unit_time + internal_dah_duration + internal_unit_time;
    TEST_ASSERT_EQUAL_INT(PHASE_DONE, morse_gen.step_morse(current_time));
}

// Conditional compilation for test runner entry point
#ifdef UNIT_TEST_NATIVE
// Native test runner main function
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_char_E_produces_correct_sequence);
    RUN_TEST(test_char_T_produces_correct_sequence);
    // Add any new tests here as well
    return UNITY_END();
}

// We need to ensure Unity's own main function is not used when we provide our own.
// This might require a build flag for Unity, or ensuring Unity.c is compiled appropriately.
// For now, this structure is typical for many native Unity setups.
// If pio test -e native complains about multiple definitions of main, 
// we may need to add something like -D UNITY_MAIN_FUNC=main_unity (and define main_unity elsewhere)
// or more simply, ensure that if Unity.c is compiled, its main is excluded.
// Often, just including unity.h and linking unity.c (or libunity.a) is enough
// and the test runner (like this main()) calls the Unity functions.

#else
// Arduino device test runner (using setup and loop)
void setup() {
    // IMPORTANT: Wait for PlatformIO to connect serial monitor,
    // otherwise, the first output may be missed.
    delay(2000); // delay is Arduino specific, fine here as it's not UNIT_TEST_NATIVE

    UNITY_BEGIN();
    RUN_TEST(test_char_E_produces_correct_sequence);
    RUN_TEST(test_char_T_produces_correct_sequence);
    // Add any new tests here as well
    UNITY_END();
}

void loop() {
    // Nothing to do here, tests run once in setup.
}
#endif
