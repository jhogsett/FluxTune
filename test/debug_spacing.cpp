#include "../native/platform.h"
#include "async_morse.h"
#include <stdio.h>

int main() {
    AsyncMorse morse;
    
    printf("=== Element Spacing Analysis ===\n");
    
    // Test A (dot-dash) at 20 WPM
    printf("\n--- A at 20 WPM ---\n");
    morse.start_morse("A", 20, false, 0);
    
    unsigned long first_turn_off_20wpm = 0;
    unsigned long second_turn_on_20wpm = 0;
    bool found_first_off = false;
    
    for (unsigned long time = 0; time <= 500; time += 1) {
        int state = morse.step_morse(time);
        if (time <= 200) printf("t=%lu: state=%d\n", time, state);
        
        if (state == STEP_MORSE_TURN_OFF && !found_first_off) {
            first_turn_off_20wpm = time;
            found_first_off = true;
            printf("First TURN_OFF at t=%lu\n", time);
        }
        if (found_first_off && state == STEP_MORSE_TURN_ON && second_turn_on_20wpm == 0) {
            second_turn_on_20wpm = time;
            printf("Second TURN_ON at t=%lu\n", time);
            break;
        }
    }
    
    unsigned long element_spacing_20wpm = second_turn_on_20wpm - first_turn_off_20wpm;
    printf("Element spacing at 20 WPM: %lu ms\n", element_spacing_20wpm);
    
    // Test A at 40 WPM
    printf("\n--- A at 40 WPM ---\n");
    morse.start_morse("A", 40, false, 0);
    
    unsigned long first_turn_off_40wpm = 0;
    unsigned long second_turn_on_40wpm = 0;
    found_first_off = false;
    
    for (unsigned long time = 0; time <= 250; time += 1) {
        int state = morse.step_morse(time);
        if (time <= 100) printf("t=%lu: state=%d\n", time, state);
        
        if (state == STEP_MORSE_TURN_OFF && !found_first_off) {
            first_turn_off_40wpm = time;
            found_first_off = true;
            printf("First TURN_OFF at t=%lu\n", time);
        }
        if (found_first_off && state == STEP_MORSE_TURN_ON && second_turn_on_40wpm == 0) {
            second_turn_on_40wpm = time;
            printf("Second TURN_ON at t=%lu\n", time);
            break;
        }
    }
    
    unsigned long element_spacing_40wpm = second_turn_on_40wpm - first_turn_off_40wpm;
    printf("Element spacing at 40 WPM: %lu ms\n", element_spacing_40wpm);
    
    printf("\n=== Analysis ===\n");
    printf("20 WPM unit time: %d ms\n", MORSE_TIME_FROM_WPM(20));
    printf("40 WPM unit time: %d ms\n", MORSE_TIME_FROM_WPM(40));
    printf("Expected ratio (20/40): %.2f\n", (float)MORSE_TIME_FROM_WPM(20) / MORSE_TIME_FROM_WPM(40));
    
    if (element_spacing_20wpm > 0 && element_spacing_40wpm > 0) {
        printf("Actual spacing ratio (20/40): %.2f\n", (float)element_spacing_20wpm / element_spacing_40wpm);
    }
    
    return 0;
}
