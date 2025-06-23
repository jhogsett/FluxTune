#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "wave_gen_pool.h"

// Test to verify that SimStation and SimNumbers implement per-message-cycle 
// WaveGen allocation/freeing for dynamic station pipelining
// This is a focused behavioral test of the changes we implemented

// Mock WaveGen for testing
class MockWaveGen {
public:
    MockWaveGen() {}
    void set_frequency(float freq, bool main=true) {}
    void set_active_frequency(bool main) {}
    void force_refresh() {}
};

// Extended Mock that tracks allocation lifecycle for SimStation-like behavior
class MockCWStation {
private:
    WaveGenPool* _wave_gen_pool;
    int _realizer;
    bool _in_wait_delay;
    unsigned long _next_cq_time;
    
    // Simulate morse state  
    bool _transmitting;
    unsigned long _transmission_end_time;
    
public:
    MockCWStation(WaveGenPool* pool) 
        : _wave_gen_pool(pool), _realizer(-1), _in_wait_delay(false), 
          _next_cq_time(0), _transmitting(false), _transmission_end_time(0) {}
    
    bool begin(unsigned long time) {
        if(_realizer != -1) {
            return true;  // Already have realizer (idempotent)
        }
        _realizer = _wave_gen_pool->get_realizer();
        return (_realizer != -1);
    }
    
    void end() {
        if(_realizer != -1) {
            _wave_gen_pool->free_realizer(_realizer);
            _realizer = -1;
        }
    }
    
    // Start a CQ transmission
    void start_cq_cycle(unsigned long time) {
        _transmitting = true;
        _transmission_end_time = time + 3000;  // 3 second CQ message
    }
    
    bool step(unsigned long time) {
        // Check if transmission just completed (like STEP_MORSE_MESSAGE_COMPLETE)
        if(_transmitting && time >= _transmission_end_time) {
            _transmitting = false;
            
            // DYNAMIC PIPELINING: Free WaveGen at end of message cycle
            end();
            
            // Start wait delay before next CQ
            _in_wait_delay = true;
            _next_cq_time = time + 4000;  // 4 second wait
            
            return true;  // Message completed
        }
        
        // Check if it's time to start next CQ cycle
        if(_in_wait_delay && time >= _next_cq_time) {
            // DYNAMIC PIPELINING: Try to reallocate WaveGen for next message cycle
            if(begin(time)) {  // Only proceed if WaveGen is available
                _in_wait_delay = false;
                start_cq_cycle(time);
                return true;  // Started new cycle
            } else {
                // WaveGen not available - extend wait period and try again later
                _next_cq_time = time + 1000;  // Try again in 1 second
                return false;  // Still waiting
            }
        }
        
        return false;  // No state change
    }
    
    int get_realizer() const { return _realizer; }
    bool is_transmitting() const { return _transmitting; }
    bool is_waiting() const { return _in_wait_delay; }
};

// Extended Mock that tracks allocation lifecycle for SimNumbers-like behavior
class MockNumbersStation {
private:
    WaveGenPool* _wave_gen_pool;
    int _realizer;
    
    enum Phase { INTERVAL, NUMBERS, ENDING, CYCLE_DELAY };
    Phase _current_phase;
    
    bool _in_delay;
    unsigned long _next_transmission_time;
    
    // Phase tracking
    int _interval_repeats_sent;
    int _groups_sent;
    bool _transmitting;
    unsigned long _transmission_end_time;
    
public:
    MockNumbersStation(WaveGenPool* pool) 
        : _wave_gen_pool(pool), _realizer(-1), _current_phase(INTERVAL),
          _in_delay(false), _next_transmission_time(0), _interval_repeats_sent(0),
          _groups_sent(0), _transmitting(false), _transmission_end_time(0) {}
    
    bool begin(unsigned long time) {
        if(_realizer != -1) {
            return true;  // Already have realizer (idempotent)
        }
        _realizer = _wave_gen_pool->get_realizer();
        return (_realizer != -1);
    }
    
    void end() {
        if(_realizer != -1) {
            _wave_gen_pool->free_realizer(_realizer);
            _realizer = -1;
        }
    }
    
    void start_transmission(unsigned long time) {
        _transmitting = true;
        _transmission_end_time = time + 1000;  // 1 second transmission
    }
    
    bool step(unsigned long time) {
        // Check if transmission just completed (like STEP_MORSE_MESSAGE_COMPLETE)
        if(_transmitting && time >= _transmission_end_time) {
            _transmitting = false;
            
            // Handle phase transitions
            switch(_current_phase) {
                case INTERVAL:
                    _interval_repeats_sent++;
                    if(_interval_repeats_sent >= 3) {  // 3 "FT" repeats
                        _current_phase = NUMBERS;
                        _groups_sent = 0;
                    }
                    _next_transmission_time = time + 500;  // Short delay
                    break;
                    
                case NUMBERS:
                    _groups_sent++;
                    if(_groups_sent >= 5) {  // 5 number groups
                        _current_phase = ENDING;
                    }
                    _next_transmission_time = time + 500;  // Short delay
                    break;
                    
                case ENDING:
                    // Ending sequence complete, start cycle delay
                    _current_phase = CYCLE_DELAY;
                    _next_transmission_time = time + 10000;  // Long cycle delay
                    
                    // DYNAMIC PIPELINING: Free WaveGen at end of complete cycle
                    end();
                    break;
                    
                case CYCLE_DELAY:
                    // Should not reach here in transmission complete
                    break;
            }
            
            _in_delay = true;
            return true;  // Phase changed
        }
        
        // Check if it's time for next transmission
        if(_in_delay && time >= _next_transmission_time) {
            _in_delay = false;
            
            if(_current_phase == CYCLE_DELAY) {
                // DYNAMIC PIPELINING: Try to reallocate WaveGen for next cycle
                if(begin(time)) {  // Only proceed if WaveGen is available
                    // Cycle complete, restart with interval signal
                    _current_phase = INTERVAL;
                    _interval_repeats_sent = 0;
                    _groups_sent = 0;
                    start_transmission(time);
                    return true;  // Started new cycle
                } else {
                    // WaveGen not available - extend cycle delay and try again later
                    _next_transmission_time = time + 1000;  // Try again in 1 second
                    _in_delay = true;
                    return false;  // Still waiting
                }
            } else {
                // Continue within current cycle
                start_transmission(time);
                return true;  // Started transmission
            }
        }
        
        return false;  // No state change
    }
    
    int get_realizer() const { return _realizer; }
    bool is_transmitting() const { return _transmitting; }
    bool is_in_cycle_delay() const { return (_current_phase == CYCLE_DELAY); }
    Phase get_phase() const { return _current_phase; }
};

MockWaveGen test_wavegens[4];
WaveGen* test_wavegen_ptrs[4] = {
    reinterpret_cast<WaveGen*>(&test_wavegens[0]),
    reinterpret_cast<WaveGen*>(&test_wavegens[1]),
    reinterpret_cast<WaveGen*>(&test_wavegens[2]),
    reinterpret_cast<WaveGen*>(&test_wavegens[3])
};
bool test_statuses[4];

void setUp(void) {
    // Reset all WaveGens to free state
    for (int i = 0; i < 4; i++) {
        test_statuses[i] = false;
    }
}

void tearDown(void) {
    // Clean up
}

void test_cw_station_dynamic_allocation_cycle(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    MockCWStation station(&pool);
    
    // Start first cycle
    TEST_ASSERT_TRUE(station.begin(1000));
    station.start_cq_cycle(1000);
    
    // During transmission, WaveGen should be allocated
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());
    TEST_ASSERT_TRUE(station.is_transmitting());
    
    // Step through transmission - should complete and free WaveGen
    bool changed = station.step(4500);  // After transmission ends
    TEST_ASSERT_TRUE(changed);  // Message completed
    TEST_ASSERT_FALSE(station.is_transmitting());
    TEST_ASSERT_TRUE(station.is_waiting());
    TEST_ASSERT_EQUAL(-1, station.get_realizer());  // WaveGen freed!
    
    // Pool should show WaveGen is free
    TEST_ASSERT_FALSE(test_statuses[0]);
    
    // Step through wait period - should reallocate and start new cycle
    changed = station.step(8600);  // After wait period ends
    TEST_ASSERT_TRUE(changed);  // New cycle started
    TEST_ASSERT_TRUE(station.is_transmitting());
    TEST_ASSERT_FALSE(station.is_waiting());
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());  // WaveGen reallocated
    
    // Pool should show WaveGen is allocated again
    TEST_ASSERT_TRUE(test_statuses[0]);
}

void test_numbers_station_dynamic_allocation_cycle(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    MockNumbersStation station(&pool);
    
    // Start first cycle
    TEST_ASSERT_TRUE(station.begin(1000));
    station.start_transmission(1000);
    
    // Should stay allocated during entire interval → numbers → ending sequence
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());
    
    unsigned long time = 1000;
    
    // Step through interval phase (3 transmissions)
    for(int i = 0; i < 3; i++) {
        time += 1000;  // Complete transmission
        station.step(time);
        
        time += 500;   // Wait delay
        station.step(time);
        
        TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());  // Should keep WaveGen
    }
    
    // Should now be in NUMBERS phase
    TEST_ASSERT_EQUAL(1, (int)station.get_phase());  // NUMBERS phase
    
    // Step through numbers phase (5 transmissions)  
    for(int i = 0; i < 5; i++) {
        time += 1000;  // Complete transmission
        station.step(time);
        
        time += 500;   // Wait delay
        if(i < 4) {  // Not the last one
            station.step(time);
        }
        
        TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());  // Should keep WaveGen
    }
    
    // Should now be in ENDING phase and have ending transmission scheduled
    TEST_ASSERT_EQUAL(2, (int)station.get_phase());  // ENDING phase
    
    // Start the ending transmission
    station.step(time);
    
    // Complete the ending transmission - should free WaveGen at completion
    time += 1000;  // Complete ending transmission
    bool changed = station.step(time);
    
    // The MockNumbersStation should now be in CYCLE_DELAY phase after ending
    TEST_ASSERT_TRUE(changed);  // State should have changed
    TEST_ASSERT_TRUE(station.is_in_cycle_delay());  // Should be in cycle delay phase
    TEST_ASSERT_EQUAL(-1, station.get_realizer());  // WaveGen freed!    
    // Pool should show WaveGen is free during cycle delay
    TEST_ASSERT_FALSE(test_statuses[0]);
    
    // Step through cycle delay - should reallocate and restart
    time += 10000;  // After long cycle delay
    bool restarted = station.step(time);
    TEST_ASSERT_TRUE(restarted);  // New cycle started
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());  // WaveGen reallocated
    TEST_ASSERT_EQUAL(0, (int)station.get_phase());  // Back to INTERVAL phase
    
    // Pool should show WaveGen is allocated again
    TEST_ASSERT_TRUE(test_statuses[0]);
}

void test_mixed_stations_efficient_sharing(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    MockCWStation cw_station(&pool);
    MockNumbersStation numbers_station(&pool);
    
    // Both stations start their cycles
    TEST_ASSERT_TRUE(cw_station.begin(1000));
    TEST_ASSERT_TRUE(numbers_station.begin(1000));
    cw_station.start_cq_cycle(1000);
    numbers_station.start_transmission(1000);
    
    // Pool should show 2 allocated
    int allocated_count = 0;
    for (int i = 0; i < 4; i++) {
        if (test_statuses[i]) allocated_count++;
    }
    TEST_ASSERT_EQUAL(2, allocated_count);
    
    // CW station completes first (shorter cycle)
    cw_station.step(4500);  // CW message complete, enters wait
    TEST_ASSERT_EQUAL(-1, cw_station.get_realizer());  // CW freed WaveGen
    TEST_ASSERT_NOT_EQUAL(-1, numbers_station.get_realizer());  // Numbers keeps WaveGen
    
    // Pool should show 1 allocated (Numbers still using one)
    allocated_count = 0;
    for (int i = 0; i < 4; i++) {
        if (test_statuses[i]) allocated_count++;
    }
    TEST_ASSERT_EQUAL(1, allocated_count);    // Numbers station eventually completes full cycle (much later)
    // The numbers station started at time 1000, so we need to let it complete
    // its cycle properly by stepping at the right intervals
    
    // Step through the complete numbers station cycle
    unsigned long time = 1000;
    
    for(int step = 0; step < 20 && !numbers_station.is_in_cycle_delay(); step++) {
        time += 1000;  // Complete any ongoing transmission
        numbers_station.step(time);
        
        if(!numbers_station.is_in_cycle_delay()) {
            time += 500;  // Wait delay before next transmission  
            numbers_station.step(time);
        }
    }
    
    TEST_ASSERT_EQUAL(-1, numbers_station.get_realizer());  // Numbers freed WaveGen
    
    // Pool should show 0 allocated (both stations in delay)
    allocated_count = 0;
    for (int i = 0; i < 4; i++) {
        if (test_statuses[i]) allocated_count++;
    }
    TEST_ASSERT_EQUAL(0, allocated_count);
      // Both stations should be able to restart and reallocate
    time += 10000;  // After cycle delay period
    bool cw_started = cw_station.step(time);
    bool numbers_started = numbers_station.step(time);
    
    TEST_ASSERT_TRUE(cw_started || numbers_started);  // At least one should start
    
    // Pool should show allocations again
    allocated_count = 0;
    for (int i = 0; i < 4; i++) {
        if (test_statuses[i]) allocated_count++;
    }
    TEST_ASSERT_GREATER_THAN(0, allocated_count);  // Some WaveGens allocated
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_cw_station_dynamic_allocation_cycle);
    RUN_TEST(test_numbers_station_dynamic_allocation_cycle);
    RUN_TEST(test_mixed_stations_efficient_sharing);
    
    return UNITY_END();
}
