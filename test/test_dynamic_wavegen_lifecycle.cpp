#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "wave_gen_pool.h"

// Test SimStation WaveGen allocation lifecycle changes
// This test verifies that SimStation now frees WaveGens between message cycles

// Mock WaveGen for testing
class MockWaveGen {
public:
    float current_frequency;
    bool active_main;
    
    MockWaveGen() : current_frequency(0.0), active_main(false) {}
    
    void set_frequency(float freq, bool main=true) { 
        current_frequency = freq; 
    }
    void set_active_frequency(bool main) { 
        active_main = main; 
    }
    void force_refresh() {}
};

// Mock Realization that simulates the key parts of SimStation behavior
class MockSimStation {
private:
    WaveGenPool* _wave_gen_pool;
    int _realizer;
    bool _in_wait_delay;
    unsigned long _next_cq_time;
    
public:
    MockSimStation(WaveGenPool* pool) : _wave_gen_pool(pool), _realizer(-1), 
                                       _in_wait_delay(false), _next_cq_time(0) {}
    
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
    
    // Simulate the key behavior from our SimStation changes
    void simulate_message_complete(unsigned long time) {
        // DYNAMIC PIPELINING: Free WaveGen at end of message cycle
        end();
        
        // Start wait delay before next CQ
        _in_wait_delay = true;
        _next_cq_time = time + 4000;  // 4 second wait like WAIT_SECONDS
    }
      bool step(unsigned long time) {
        // Check if it's time to start next CQ cycle
        if(_in_wait_delay && time >= _next_cq_time) {
            // DYNAMIC PIPELINING: Try to reallocate WaveGen for next message cycle
            if(begin(time)) {  // Only proceed if WaveGen is available
                _in_wait_delay = false;
                return true;  // Started new cycle
            } else {
                // WaveGen not available - extend wait period and try again later
                _next_cq_time = time + 1000;  // Try again in 1 second
                return false;  // Still waiting
            }
        }
        return false;  // Either not waiting or wait time not reached
    }
    
    int get_realizer() const { return _realizer; }
    bool is_waiting() const { return _in_wait_delay; }
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

void test_station_frees_wavegen_after_message_cycle(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    MockSimStation station(&pool);
    
    // Station starts without WaveGen
    TEST_ASSERT_EQUAL(-1, station.get_realizer());
    
    // Station begins first message cycle
    TEST_ASSERT_TRUE(station.begin(1000));
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());
    
    // Pool should show 1 allocated
    TEST_ASSERT_TRUE(test_statuses[0]);   // First WaveGen allocated
    TEST_ASSERT_FALSE(test_statuses[1]);  // Others free
    
    // Simulate message completion (like STEP_MORSE_MESSAGE_COMPLETE)
    station.simulate_message_complete(5000);
    
    // Station should have freed WaveGen and entered wait state
    TEST_ASSERT_EQUAL(-1, station.get_realizer());
    TEST_ASSERT_TRUE(station.is_waiting());
    
    // Pool should show 0 allocated (WaveGen freed!)
    TEST_ASSERT_FALSE(test_statuses[0]);  // WaveGen freed
    TEST_ASSERT_FALSE(test_statuses[1]);
    TEST_ASSERT_FALSE(test_statuses[2]);
    TEST_ASSERT_FALSE(test_statuses[3]);
}

void test_station_reallocates_wavegen_for_next_cycle(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    MockSimStation station(&pool);
    
    // Complete first cycle and enter wait
    station.begin(1000);
    station.simulate_message_complete(5000);
    TEST_ASSERT_EQUAL(-1, station.get_realizer());
    TEST_ASSERT_TRUE(station.is_waiting());
    
    // Step through wait period - should still be waiting
    bool started_new_cycle = station.step(7000);  // Before wait end
    TEST_ASSERT_FALSE(started_new_cycle);
    TEST_ASSERT_TRUE(station.is_waiting());
    TEST_ASSERT_EQUAL(-1, station.get_realizer());
    
    // Step past wait end - should try to reallocate WaveGen
    started_new_cycle = station.step(9500);  // After wait end (5000 + 4000 = 9000)
    TEST_ASSERT_TRUE(started_new_cycle);
    TEST_ASSERT_FALSE(station.is_waiting());
    TEST_ASSERT_NOT_EQUAL(-1, station.get_realizer());  // WaveGen reallocated!
    
    // Pool should show 1 allocated again
    TEST_ASSERT_TRUE(test_statuses[0]);  // WaveGen reallocated
}

void test_station_waits_when_pool_exhausted(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    // Create 4 stations and exhaust the pool
    MockSimStation station1(&pool);
    MockSimStation station2(&pool);
    MockSimStation station3(&pool);
    MockSimStation station4(&pool);
    MockSimStation station5(&pool);  // This one will have to wait
    
    // Allocate all WaveGens to first 4 stations
    TEST_ASSERT_TRUE(station1.begin(1000));
    TEST_ASSERT_TRUE(station2.begin(1000));
    TEST_ASSERT_TRUE(station3.begin(1000));
    TEST_ASSERT_TRUE(station4.begin(1000));
    
    // Station 5 completes message and tries to start next cycle
    station5.simulate_message_complete(5000);
    
    // Pool is exhausted - station5 should wait longer
    bool started = station5.step(9500);  // Try to start at normal time
    TEST_ASSERT_FALSE(started);  // Should fail - no WaveGen available
    TEST_ASSERT_TRUE(station5.is_waiting());  // Should still be waiting
    
    // Station1 completes its cycle, freeing a WaveGen
    station1.simulate_message_complete(10000);
    
    // Now station5 should be able to start on its next retry
    started = station5.step(10500);  // Retry 1 second later
    TEST_ASSERT_TRUE(started);  // Should succeed now
    TEST_ASSERT_NOT_EQUAL(-1, station5.get_realizer());
}

void test_multiple_stations_sharing_wavegens_efficiently(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    MockSimStation stations[6] = {
        MockSimStation(&pool), MockSimStation(&pool), MockSimStation(&pool),
        MockSimStation(&pool), MockSimStation(&pool), MockSimStation(&pool)
    };
    
    // Start 4 stations (exhaust pool)
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(stations[i].begin(1000));
    }
    
    // Station 0 completes cycle at time 5000
    stations[0].simulate_message_complete(5000);
    
    // Station 1 completes cycle at time 6000  
    stations[1].simulate_message_complete(6000);
    
    // Now 2 WaveGens should be free, stations 2&3 still using theirs
    int free_count = 0;
    for (int i = 0; i < 4; i++) {
        if (!test_statuses[i]) free_count++;
    }
    TEST_ASSERT_EQUAL(2, free_count);
    
    // Stations 4&5 can now start their cycles
    TEST_ASSERT_TRUE(stations[4].begin(7000));
    TEST_ASSERT_TRUE(stations[5].begin(7000));
    
    // Pool should be full again
    free_count = 0;
    for (int i = 0; i < 4; i++) {
        if (!test_statuses[i]) free_count++;
    }
    TEST_ASSERT_EQUAL(0, free_count);  // All WaveGens in use
    
    // When station 0 tries to restart (time 9000), pool is full
    bool started = stations[0].step(9500);
    TEST_ASSERT_FALSE(started);  // Should wait
    
    // When station 2 completes (freeing a WaveGen), station 0 can start
    stations[2].simulate_message_complete(10000);
    started = stations[0].step(11000);  // Retry after station 2 freed WaveGen
    TEST_ASSERT_TRUE(started);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_station_frees_wavegen_after_message_cycle);
    RUN_TEST(test_station_reallocates_wavegen_for_next_cycle);
    RUN_TEST(test_station_waits_when_pool_exhausted);
    RUN_TEST(test_multiple_stations_sharing_wavegens_efficiently);
    
    return UNITY_END();
}
