#include "../native/platform.h"
#include "../include/async_pager.h"

AsyncPager::AsyncPager()
{
    // Initialize all state variables to safe defaults
    _active = false;
    _repeat = false;
    _transmitting = false;
    _current_state = PAGER_STATE_SILENCE;
    _next_event_time = 0;
    _initialized = false;
}

void AsyncPager::start_pager_transmission(bool repeat)
{
    _repeat = repeat;
    _active = true;
    _initialized = true;
    
    // Start with first tone immediately
    _current_state = PAGER_STATE_TONE_A;
    _transmitting = true;
    _next_event_time = millis() + PAGER_TONE_DURATION;
}

int AsyncPager::step_pager(unsigned long time)
{
    if (!_active || !_initialized) {
        return STEP_PAGER_LEAVE_OFF;
    }
    
    // Check if it's time for a state change
    if (time < _next_event_time) {
        // Not time to change state yet
        return _transmitting ? STEP_PAGER_LEAVE_ON : STEP_PAGER_LEAVE_OFF;
    }
    
    // Time to change state
    start_next_phase(time);
    
    // Return appropriate step based on new state
    return _transmitting ? STEP_PAGER_TURN_ON : STEP_PAGER_TURN_OFF;
}

void AsyncPager::start_next_phase(unsigned long time)
{
    switch (_current_state) {
        case PAGER_STATE_TONE_A:
            // Tone A finished, start Tone B immediately
            _current_state = PAGER_STATE_TONE_B;
            _transmitting = true;
            _next_event_time = time + PAGER_TONE_DURATION;
            break;
            
        case PAGER_STATE_TONE_B:
            // Tone B finished, start silence period
            _current_state = PAGER_STATE_SILENCE;
            _transmitting = false;
            _next_event_time = time + get_random_silence_duration();
            break;
            
        case PAGER_STATE_SILENCE:
            // Silence finished
            if (_repeat) {
                // Start next transmission cycle with Tone A
                _current_state = PAGER_STATE_TONE_A;
                _transmitting = true;
                _next_event_time = time + PAGER_TONE_DURATION;
            } else {
                // No repeat, stay silent
                _active = false;
                _transmitting = false;
            }
            break;
    }
}

unsigned long AsyncPager::get_random_silence_duration()
{
    // Generate random silence duration between min and max
    // For native builds, use simple modulo; for Arduino, use Arduino's random()
#ifdef PLATFORM_NATIVE
    // Simple pseudo-random for native testing
    static unsigned long seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return PAGER_SILENCE_MIN + (seed % (PAGER_SILENCE_MAX - PAGER_SILENCE_MIN));
#else
    return PAGER_SILENCE_MIN + random(PAGER_SILENCE_MAX - PAGER_SILENCE_MIN);
#endif
}
