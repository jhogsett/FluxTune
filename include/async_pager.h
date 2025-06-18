#ifndef __ASYNC_PAGER_H__
#define __ASYNC_PAGER_H__

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif

// Pager timing constants (in milliseconds) - authentic two-tone sequential timing
#define PAGER_TONE_A_DURATION 1500    // First tone: 1.5 seconds (longer for unsquelch)
#define PAGER_TONE_B_DURATION 800     // Second tone: 0.8 seconds (shorter identification)
#define PAGER_INTER_TONE_GAP 100      // Gap between tones: 100ms
#define PAGER_SILENCE_MIN 3000        // Minimum 3 seconds silence (shortened for testing)
#define PAGER_SILENCE_MAX 8000        // Maximum 8 seconds silence (shortened for testing)

// Pager step return values
#define STEP_PAGER_TURN_ON   1        // Start transmitting (tone A or B)
#define STEP_PAGER_TURN_OFF  2        // Stop transmitting (enter silence)
#define STEP_PAGER_LEAVE_ON  3        // Continue transmitting (no change)
#define STEP_PAGER_LEAVE_OFF 4        // Continue silence
#define STEP_PAGER_CHANGE_FREQ 5      // Continue transmitting but change frequency

// Pager transmission states
#define PAGER_STATE_TONE_A   0        // Transmitting first tone (longer)
#define PAGER_STATE_GAP      1        // Brief gap between tones
#define PAGER_STATE_TONE_B   2        // Transmitting second tone (shorter)
#define PAGER_STATE_SILENCE  3        // Silent period between transmissions

class AsyncPager
{
public:
    AsyncPager();

    void start_pager_transmission(bool repeat);
    int step_pager(unsigned long time);
    int get_current_state() { return _current_state; }
    
private:
    void start_next_phase(unsigned long time);
    unsigned long get_random_silence_duration();

    bool _active;                     // True when pager is active
    bool _repeat;                     // True to repeat transmissions
    bool _transmitting;               // True during tone transmission
    int _current_state;               // Current pager state (TONE_A, TONE_B, SILENCE)
    unsigned long _next_event_time;   // When next state change should occur
    bool _initialized;                // True after first start_pager_transmission call
};

#endif
