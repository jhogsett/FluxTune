#ifndef __ASYNC_JAMMER_H__
#define __ASYNC_JAMMER_H__

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif

// Jammer behavior constants (in milliseconds and Hz)
#define JAMMER_STEP_INTERVAL 50       // 50ms between frequency changes
#define JAMMER_MAX_DRIFT 2000.0       // ±2kHz drift range from base frequency
#define JAMMER_STEP_SIZE 25.0         // 25Hz brownian motion step size
#define JAMMER_MUTE_PROBABILITY 15    // 15% chance to mute per step (0-100)
#define JAMMER_MUTE_DURATION_MIN 20   // 20ms minimum mute duration
#define JAMMER_MUTE_DURATION_MAX 200  // 200ms maximum mute duration

// Jammer step return values
#define STEP_JAMMER_TURN_ON      1    // Start transmitting
#define STEP_JAMMER_TURN_OFF     2    // Stop transmitting (muted)
#define STEP_JAMMER_LEAVE_ON     3    // Continue transmitting
#define STEP_JAMMER_LEAVE_OFF    4    // Continue muted
#define STEP_JAMMER_CHANGE_FREQ  5    // Continue transmitting but change frequency

// Jammer transmission states
#define JAMMER_STATE_TRANSMITTING 0   // Actively jamming with frequency drift
#define JAMMER_STATE_MUTED       1    // Temporarily muted

class AsyncJammer
{
public:
    AsyncJammer();    void start_jammer_transmission(bool repeat);
    int step_jammer(unsigned long time);
    int get_current_state() const { return _current_state; }
    float get_frequency_offset() const { return _current_frequency_offset; }
    
private:
    void update_frequency_drift();
    void start_next_phase(unsigned long time);
    unsigned long get_random_mute_duration();
    bool should_mute();

    bool _active;                     // True when jammer is active
    bool _repeat;                     // True to repeat indefinitely
    int _current_state;               // Current jammer state
    unsigned long _next_event_time;   // When next state change should occur
    bool _initialized;                // True after first start_jammer_transmission call
    
    // Frequency drift tracking
    float _current_frequency_offset;  // Current offset from base frequency (Hz)
    float _drift_velocity;            // Current brownian motion velocity
};

#endif
