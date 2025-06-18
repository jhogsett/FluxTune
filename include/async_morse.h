#ifndef __ASYNC_MORSE_H__
#define __ASYNC_MORSE_H__

#define MORSE_TIME_FROM_WPM(w) (1000 / w)

#define PHASE_DONE 0
#define PHASE_CHAR 1
#define PHASE_SPACE 2
#define PHASE_WAIT 3

#define MAX_ELEMENT 6

#define STEP_MORSE_TURN_ON   1
#define STEP_MORSE_TURN_OFF  2
#define STEP_MORSE_LEAVE_ON  3
#define STEP_MORSE_LEAVE_OFF 4

#define STEP_ELEMENT_EARLY 0
#define STEP_ELEMENT_ACTIVE 1
#define STEP_ELEMENT_DONE 2

class AsyncMorse
{
public:
    AsyncMorse();    void start_morse(const char *s, int wpm, bool repeat, int wait_seconds);
    int step_morse(unsigned long time);
    
private:
    // ========================================
    // INTERNAL HELPER METHODS
    // ========================================
    char lookup_morse_char(char c);
    bool start_step_element(unsigned long time);
    void restart_morse();
    unsigned long compute_element_time(unsigned long time, byte element_count, bool is_space);
    int step_element(unsigned long time);
    bool step_position(unsigned long time);
    void step_space(unsigned long time);
    void start_wait(unsigned long time);
    void step_wait(unsigned long time);
    void handle_transmission_complete(unsigned long time);
    bool is_time_ready(unsigned long current_time);

    // ========================================
    // STATE VARIABLES
    // ========================================
    
    // Text configuration
    const char *async_str = NULL;          // String being transmitted
    int async_length;                      // Length of text string
    int async_element_del;                 // Base timing unit in milliseconds (from WPM)
    bool async_repeat;                     // Whether to repeat transmission
    int async_wait_seconds;                // Seconds to wait between repeats
    
    // Current transmission state  
    byte async_phase;                      // Current phase: PHASE_DONE, PHASE_CHAR, PHASE_SPACE, PHASE_WAIT
    byte async_position;                   // Current position in text string
    char async_char;                       // Morse table index for current character
    byte async_morse;                      // Bit pattern for current character
    byte async_element;                    // Current element within character (0-6)
    bool async_element_done;               // True when current element is finished    // Timing and output state
    bool async_active;                     // True when transmitter should be ON
    unsigned long async_next_event;        // Time when next state change occurs
    bool async_space;                      // True when in gap between elements
    bool async_switched_on;                // Tracks output transitions for TURN_ON/TURN_OFF

};

extern void start_morse(const char *s, int wpm, bool repeat);
extern int step_morse(unsigned long time);

#endif
