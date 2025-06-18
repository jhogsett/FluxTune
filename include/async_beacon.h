#ifndef __ASYNC_BEACON_H__
#define __ASYNC_BEACON_H__

#include "basic_types.h"

// Return values for step_beacon()
#define STEP_BEACON_TURN_ON   1
#define STEP_BEACON_TURN_OFF  2  
#define STEP_BEACON_LEAVE_ON  3
#define STEP_BEACON_LEAVE_OFF 4

// Beacon pattern types
#define BEACON_PATTERN_SIMPLE     0  // Simple on/off toggle
#define BEACON_PATTERN_FAST_KEYER 1  // Fast keyer pattern (multiple short pulses)
#define BEACON_PATTERN_HEARTBEAT  2  // Heartbeat pattern (short-short-long pause)
#define BEACON_PATTERN_LIGHTHOUSE 3  // Lighthouse pattern (rotating beam simulation)

class AsyncBeacon
{
public:
    AsyncBeacon();
    
    void start_beacon(uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms, bool repeat);
    int step_beacon(unsigned long time);
    
private:
    // Pattern configuration
    uint8_t _pattern_type;
    unsigned long _on_time_ms;
    unsigned long _off_time_ms;
    bool _repeat;
    
    // Current state
    bool _active;              // Whether beacon should be transmitting
    bool _switched_on;         // For detecting state changes
    unsigned long _next_event; // Time for next state change
    bool _running;             // Whether beacon is currently running
    
    // Pattern-specific state
    uint8_t _pattern_step;     // Current step in pattern sequence
    uint8_t _pattern_cycles;   // Cycles completed in current pattern
    
    // Pattern implementations
    void start_simple_pattern();
    void start_fast_keyer_pattern();
    void start_heartbeat_pattern(); 
    void start_lighthouse_pattern();
    
    bool step_simple_pattern(unsigned long time);
    bool step_fast_keyer_pattern(unsigned long time);
    bool step_heartbeat_pattern(unsigned long time);
    bool step_lighthouse_pattern(unsigned long time);
    
    void schedule_next_event(unsigned long time, unsigned long delay_ms);
    bool is_time_ready(unsigned long current_time);
};

#endif
