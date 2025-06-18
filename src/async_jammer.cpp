#include "../native/platform.h"
#include "../include/async_jammer.h"

AsyncJammer::AsyncJammer()
{
    // Initialize all state variables to safe defaults
    _active = false;
    _repeat = false;
    _current_state = JAMMER_STATE_MUTED;
    _next_event_time = 0;
    _initialized = false;
    
    // Initialize frequency drift parameters
    _current_frequency_offset = 0.0;
    _drift_velocity = 0.0;
}

void AsyncJammer::start_jammer_transmission(bool repeat)
{
    _repeat = repeat;
    _active = true;
    _initialized = true;
    
    // Start transmitting immediately
    _current_state = JAMMER_STATE_TRANSMITTING;
    
    // Initialize frequency drift to center
    _current_frequency_offset = 0.0;
    _drift_velocity = 0.0;
    
    // Initialize to 0 like other async classes - timing will be set on first step_jammer call
    _next_event_time = 0;
}

int AsyncJammer::step_jammer(unsigned long time)
{
    if (!_active || !_initialized) {
        return STEP_JAMMER_LEAVE_OFF;
    }
    
    // If this is the first call (next_event_time is 0), set up initial timing
    if (_next_event_time == 0) {
        _next_event_time = time + JAMMER_STEP_INTERVAL;
        return STEP_JAMMER_TURN_ON;  // Start transmitting
    }
    
    // Check if it's time for a state change
    if (time < _next_event_time) {
        // Not time to change state yet
        return (_current_state == JAMMER_STATE_TRANSMITTING) ? STEP_JAMMER_LEAVE_ON : STEP_JAMMER_LEAVE_OFF;
    }
    
    // Time to change state
    bool was_transmitting = (_current_state == JAMMER_STATE_TRANSMITTING);
    float old_frequency_offset = _current_frequency_offset;
    start_next_phase(time);
    
    // Check if jammer became inactive (no-repeat case)
    if (!_active) {
        return STEP_JAMMER_LEAVE_OFF;
    }
    
    // Return appropriate step based on transition
    bool is_transmitting = (_current_state == JAMMER_STATE_TRANSMITTING);
    
    if (is_transmitting && !was_transmitting) {
        return STEP_JAMMER_TURN_ON;     // MUTED → TRANSMITTING
    } else if (!is_transmitting && was_transmitting) {
        return STEP_JAMMER_TURN_OFF;    // TRANSMITTING → MUTED
    } else if (is_transmitting && was_transmitting) {
        // Check if frequency changed significantly
        if (old_frequency_offset != _current_frequency_offset) {
            return STEP_JAMMER_CHANGE_FREQ; // TRANSMITTING → TRANSMITTING with new frequency
        } else {
            return STEP_JAMMER_LEAVE_ON;    // TRANSMITTING → TRANSMITTING (no change)
        }
    } else {
        return STEP_JAMMER_LEAVE_OFF;   // MUTED → MUTED
    }
}

void AsyncJammer::start_next_phase(unsigned long time)
{
    switch (_current_state) {
        case JAMMER_STATE_TRANSMITTING:
            // Update frequency drift with brownian motion
            update_frequency_drift();
            
            // Check if we should randomly mute
            if (should_mute()) {
                _current_state = JAMMER_STATE_MUTED;
                _next_event_time = time + get_random_mute_duration();
            } else {
                // Continue transmitting, schedule next frequency update
                _next_event_time = time + JAMMER_STEP_INTERVAL;
            }
            break;
            
        case JAMMER_STATE_MUTED:
            // Mute period finished, resume transmitting
            _current_state = JAMMER_STATE_TRANSMITTING;
            update_frequency_drift(); // Update frequency when resuming
            _next_event_time = time + JAMMER_STEP_INTERVAL;
            break;
    }
    
    // If not repeating and we've been running for a while, eventually stop
    // For now, jammers run indefinitely when active
}

void AsyncJammer::update_frequency_drift()
{
    // Brownian motion: add random acceleration to velocity, then apply velocity to position
    
#ifdef PLATFORM_NATIVE
    // Simple pseudo-random for native testing
    static unsigned long seed = 98765;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    float random_acceleration = ((float)(seed % 2001) - 1000.0) / 1000.0; // -1.0 to +1.0
#else
    // Arduino random() function
    float random_acceleration = ((float)(random(2001)) - 1000.0) / 1000.0; // -1.0 to +1.0
#endif
    
    // Update velocity with damping and random acceleration
    _drift_velocity = _drift_velocity * 0.95 + random_acceleration * (JAMMER_STEP_SIZE * 0.5);
    
    // Apply velocity to frequency offset
    _current_frequency_offset += _drift_velocity;
    
    // Constrain to drift range with soft bouncing
    if (_current_frequency_offset > JAMMER_MAX_DRIFT) {
        _current_frequency_offset = JAMMER_MAX_DRIFT;
        _drift_velocity *= -0.8; // Bounce back with damping
    } else if (_current_frequency_offset < -JAMMER_MAX_DRIFT) {
        _current_frequency_offset = -JAMMER_MAX_DRIFT;
        _drift_velocity *= -0.8; // Bounce back with damping
    }
}

bool AsyncJammer::should_mute()
{
    // Return true with JAMMER_MUTE_PROBABILITY chance
#ifdef PLATFORM_NATIVE
    static unsigned long seed = 13579;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return (seed % 100) < JAMMER_MUTE_PROBABILITY;
#else
    return random(100) < JAMMER_MUTE_PROBABILITY;
#endif
}

unsigned long AsyncJammer::get_random_mute_duration()
{
    // Return random duration between min and max
#ifdef PLATFORM_NATIVE
    static unsigned long seed = 24680;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return JAMMER_MUTE_DURATION_MIN + (seed % (JAMMER_MUTE_DURATION_MAX - JAMMER_MUTE_DURATION_MIN));
#else
    return JAMMER_MUTE_DURATION_MIN + random(JAMMER_MUTE_DURATION_MAX - JAMMER_MUTE_DURATION_MIN);
#endif
}
