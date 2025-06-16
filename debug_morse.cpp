#include "unity/src/unity.h"
#include "../native/platform.h"
#include "async_morse.h"
#include <stdio.h>

// Simple debug test to understand the async morse behavior
AsyncMorse morse;
unsigned long mock_time = 0;

void debug_morse_E() {
    printf("\\nDebugging morse code for letter 'E' (single dot)\\n");
    printf("Time\\tState\\tDescription\\n");
    printf("----\\t-----\\t-----------\\n");
    
    morse.start_morse("E", 20, false, 0);
    
    // Step through time and see what happens
    for (int i = 0; i < 100; i++) {
        int state = morse.step_morse(mock_time);
        
        const char* state_name;
        switch(state) {
            case 1: state_name = "TURN_ON"; break;
            case 2: state_name = "TURN_OFF"; break;
            case 3: state_name = "LEAVE_ON"; break;
            case 4: state_name = "LEAVE_OFF"; break;
            default: state_name = "UNKNOWN"; break;
        }
        
        static int last_state = -1;
        if (state != last_state) {
            printf("%lu\\t%d\\t%s\\n", mock_time, state, state_name);
            last_state = state;
        }
        
        mock_time += 10; // Advance by 10ms each step
        
        // Stop if we've been in LEAVE_OFF for a while
        if (state == 4 && i > 20) break;
    }
}

int main() {
    debug_morse_E();
    return 0;
}
