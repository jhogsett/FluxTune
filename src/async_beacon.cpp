#include "../native/platform.h"
#include "../include/async_beacon.h"

// ========================================
// CONSTRUCTOR
// ========================================
AsyncBeacon::AsyncBeacon() {
    _pattern_type = BEACON_PATTERN_SIMPLE;
    _on_time_ms = 1000;
    _off_time_ms = 1000;
    _repeat = true;
    _active = false;
    _switched_on = false;
    _next_event = 0;
    _running = false;
    _pattern_step = 0;
    _pattern_cycles = 0;
}

// ========================================
// PUBLIC INTERFACE
// ========================================
void AsyncBeacon::start_beacon(uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms, bool repeat) {
    _pattern_type = pattern_type;
    _on_time_ms = on_time_ms;
    _off_time_ms = off_time_ms;
    _repeat = repeat;
    
    _active = false;
    _switched_on = false;
    _next_event = 0;
    _running = true;
    _pattern_step = 0;
    _pattern_cycles = 0;
    
    // Start the appropriate pattern
    switch (_pattern_type) {
        case BEACON_PATTERN_SIMPLE:
            start_simple_pattern();
            break;
        case BEACON_PATTERN_FAST_KEYER:
            start_fast_keyer_pattern();
            break;
        case BEACON_PATTERN_HEARTBEAT:
            start_heartbeat_pattern();
            break;
        case BEACON_PATTERN_LIGHTHOUSE:
            start_lighthouse_pattern();
            break;
        default:
            start_simple_pattern();
            break;
    }
}

int AsyncBeacon::step_beacon(unsigned long time) {
    if (!_running) {
        return _active ? STEP_BEACON_LEAVE_ON : STEP_BEACON_LEAVE_OFF;
    }
    
    // Step the appropriate pattern
    bool pattern_active = false;
    switch (_pattern_type) {
        case BEACON_PATTERN_SIMPLE:
            pattern_active = step_simple_pattern(time);
            break;
        case BEACON_PATTERN_FAST_KEYER:
            pattern_active = step_fast_keyer_pattern(time);
            break;
        case BEACON_PATTERN_HEARTBEAT:
            pattern_active = step_heartbeat_pattern(time);
            break;
        case BEACON_PATTERN_LIGHTHOUSE:
            pattern_active = step_lighthouse_pattern(time);
            break;
        default:
            pattern_active = step_simple_pattern(time);
            break;
    }
    
    if (!pattern_active && !_repeat) {
        _running = false;
        _active = false;  // Ensure beacon turns off when stopped
    }
    
    // Generate output signal based on current active state
    if (_active != _switched_on) {
        _switched_on = _active;
        return _active ? STEP_BEACON_TURN_ON : STEP_BEACON_TURN_OFF;
    } else {
        return _active ? STEP_BEACON_LEAVE_ON : STEP_BEACON_LEAVE_OFF;
    }
}

// ========================================
// PATTERN IMPLEMENTATIONS
// ========================================

void AsyncBeacon::start_simple_pattern() {
    _pattern_step = 0;
    _active = true;  // Start with beacon on
    schedule_next_event(0, _on_time_ms);
}

bool AsyncBeacon::step_simple_pattern(unsigned long time) {
    if (!is_time_ready(time)) {
        return true;  // Pattern continues
    }
    
    if (_active) {
        // Currently on, switch to off
        _active = false;
        schedule_next_event(time, _off_time_ms);
    } else {
        // Currently off, switch to on (new cycle)
        _active = true;
        schedule_next_event(time, _on_time_ms);
        _pattern_cycles++;
        
        // For non-repeat mode, complete after one full cycle
        if (!_repeat && _pattern_cycles >= 1) {
            return false;  // Pattern complete
        }
    }
    
    return true;  // Pattern continues
}

void AsyncBeacon::start_fast_keyer_pattern() {
    _pattern_step = 0;
    _pattern_cycles = 0;
    _active = true;
    schedule_next_event(0, _on_time_ms / 4);  // Short pulses
}

bool AsyncBeacon::step_fast_keyer_pattern(unsigned long time) {
    if (!is_time_ready(time)) {
        return true;
    }
    
    // Fast keyer: 3 short pulses, then longer pause
    if (_pattern_step < 6) {  // 3 on/off cycles
        if (_active) {
            _active = false;
            schedule_next_event(time, _on_time_ms / 4);  // Short gap
        } else {
            _active = true;
            schedule_next_event(time, _on_time_ms / 4);  // Short pulse
        }
        _pattern_step++;
    } else {
        // End of cycle, longer pause
        _active = false;
        schedule_next_event(time, _off_time_ms);
        _pattern_step = 0;
        _pattern_cycles++;
    }
    
    return true;
}

void AsyncBeacon::start_heartbeat_pattern() {
    _pattern_step = 0;
    _pattern_cycles = 0;
    _active = true;
    schedule_next_event(0, _on_time_ms / 3);  // First short pulse
}

bool AsyncBeacon::step_heartbeat_pattern(unsigned long time) {
    if (!is_time_ready(time)) {
        return true;
    }
    
    // Heartbeat: short-short-long pause (like lub-dub)
    switch (_pattern_step) {
        case 0:  // First pulse on -> off
            _active = false;
            schedule_next_event(time, _on_time_ms / 6);  // Short gap
            break;
        case 1:  // Short gap -> second pulse
            _active = true;
            schedule_next_event(time, _on_time_ms / 3);  // Second pulse
            break;
        case 2:  // Second pulse -> off
            _active = false;
            schedule_next_event(time, _off_time_ms);  // Long pause
            _pattern_cycles++;
            break;
        default:  // Start new cycle
            _active = true;
            schedule_next_event(time, _on_time_ms / 3);  // First pulse
            _pattern_step = -1;  // Will be incremented to 0
            break;
    }
    
    _pattern_step++;
    return true;
}

void AsyncBeacon::start_lighthouse_pattern() {
    _pattern_step = 0;
    _pattern_cycles = 0;
    _active = false;  // Start with beam away
    schedule_next_event(0, _off_time_ms);
}

bool AsyncBeacon::step_lighthouse_pattern(unsigned long time) {
    if (!is_time_ready(time)) {
        return true;
    }
    
    // Lighthouse: mostly off with periodic sweeps
    if (_pattern_step == 0) {
        // Beam sweep: quick flash
        _active = true;
        schedule_next_event(time, _on_time_ms / 2);  // Brief flash
        _pattern_step = 1;
    } else {
        // Back to darkness
        _active = false;
        schedule_next_event(time, _off_time_ms * 3);  // Long dark period
        _pattern_step = 0;
        _pattern_cycles++;
    }
    
    return true;
}

// ========================================
// HELPER METHODS
// ========================================

void AsyncBeacon::schedule_next_event(unsigned long time, unsigned long delay_ms) {
    _next_event = time + delay_ms;
}

bool AsyncBeacon::is_time_ready(unsigned long current_time) {
    return current_time >= _next_event;
}
