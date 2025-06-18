#ifndef __ASYNC_PAGER_H__
#define __ASYNC_PAGER_H__

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif

// Pager timing constants (in milliseconds)
#define PAGER_TONE_DURATION 1000      // 1 second per tone
#define PAGER_SILENCE_MIN 15000       // Minimum 15 seconds silence
#define PAGER_SILENCE_MAX 30000       // Maximum 30 seconds silence

// Pager step return values
#define STEP_PAGER_TURN_ON   1        // Start transmitting (tone A or B)
#define STEP_PAGER_TURN_OFF  2        // Stop transmitting (enter silence)
#define STEP_PAGER_LEAVE_ON  3        // Continue transmitting
#define STEP_PAGER_LEAVE_OFF 4        // Continue silence

// Pager transmission states
#define PAGER_STATE_TONE_A   0        // Transmitting first tone
#define PAGER_STATE_TONE_B   1        // Transmitting second tone  
#define PAGER_STATE_SILENCE  2        // Silent period between transmissions

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
