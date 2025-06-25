#include "unity/src/unity.h"
#include "../native/platform.h"
#include "../include/async_morse.h"
#include <iostream>

// Test fixture (exact copy from test_async_morse.cpp)
AsyncMorse* morse;

void setUp(void) {
    morse = new AsyncMorse();
}

void tearDown(void) {
    delete morse;
    morse = nullptr;
}

void test_morse_extreme_wpm_values_debug(void) {
    std::cout << "\n=== DEBUG: Starting extreme WPM test ===\n";
    
    // Test very slow WPM
    std::cout << "Starting slow WPM test...\n";
    morse->start_morse("E", 1); // 1 WPM
    bool found_turn_on = false;
    for (unsigned long time = 0; time <= 2000; time += 50) {
        int result = morse->step_morse(time);
        if (result == STEP_MORSE_TURN_ON) {
            found_turn_on = true;
            std::cout << "Found TURN_ON for slow WPM at time " << time << "\n";
            std::cout << "After slow WPM TURN_ON: switched_on = " << morse->is_switched_on() << "\n";
            break;
        }
    }
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on, "Should work with very slow WPM");
    
    std::cout << "Starting fast WPM test...\n";
    std::cout << "Before fast WPM start: switched_on = " << morse->is_switched_on() << "\n";
    
    // Test very fast WPM  
    morse->start_morse("E", 100); // 100 WPM
    std::cout << "After fast WPM start: switched_on = " << morse->is_switched_on() << "\n";
    
    found_turn_on = false;
    for (unsigned long time = 0; time <= 100; time += 1) {
        int result = morse->step_morse(time);
        if (result == STEP_MORSE_TURN_ON) {
            found_turn_on = true;
            std::cout << "Found TURN_ON for fast WPM at time " << time << "\n";
            break;
        }
        if (time <= 5) {  // Debug first few steps
            std::cout << "Fast WPM time " << time << ": result = " << result << "\n";
        }
    }
    
    if (!found_turn_on) {
        std::cout << "FAILED: No TURN_ON found for fast WPM\n";
    }
    
    TEST_ASSERT_TRUE_MESSAGE(found_turn_on, "Should work with very fast WPM");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_morse_extreme_wpm_values_debug);
    return UNITY_END();
}
