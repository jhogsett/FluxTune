#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "signal_meter.h"
#include "sim_station.h"
#include "../native/mock_realization_pool.h"

void setUp(void) {
    // This is run before EACH test
}

void tearDown(void) {
    // This is run after EACH test
}

void test_station_sends_charge_pulse_on_carrier_turn_on(void) {
    // Setup signal meter
    SignalMeter signal_meter;
    signal_meter.init();
    
    // Setup mock realizer pool
    WaveGenPool wave_gen_pool(nullptr, nullptr, 0);
    
    // Create station with signal meter
    SimStation station(&wave_gen_pool, &signal_meter);
    
    // Initialize station
    station.begin(0, 7002000.0, "E", 20); // Short message, 20 WPM
    // Set VFO frequency close to station for maximum charge
    station.set_vfo_frequency_for_test(7002000.0); // Exact match
      // Initial signal meter should be at 0
    signal_meter.update(0);
    TEST_ASSERT_EQUAL(0, signal_meter.get_current_strength());
    
    // Step the station - should eventually get TURN_ON event
    bool saw_turn_on = false;
    int max_steps = 1000;
    
    for (int i = 0; i < max_steps && !saw_turn_on; i++) {
        unsigned long time = i * 10; // 10ms steps
        
        // Step signal meter to handle decay
        signal_meter.update(time);
        
        // Step station
        station.step(time);
        
        // Check if signal meter received charge
        if (signal_meter.get_current_strength() > 0) {
            saw_turn_on = true;
        }
    }
    
    TEST_ASSERT_TRUE_MESSAGE(saw_turn_on, "Station should send charge pulse when carrier turns on");
}

void test_station_no_charge_when_vfo_far_away(void) {
    // Setup signal meter
    SignalMeter signal_meter;
    signal_meter.init();
    
    // Setup mock realizer pool
    WaveGenPool wave_gen_pool(nullptr, nullptr, 0);
    
    // Create station with signal meter
    SimStation station(&wave_gen_pool, &signal_meter);
    
    // Initialize station
    station.begin(0, 7002000.0, "E", 20); // Short message, 20 WPM
    // Set VFO frequency very far from station (no charge expected)
    station.set_vfo_frequency_for_test(3500000.0); // Way off frequency
    
    // Step the station and signal meter
    for (int i = 0; i < 1000; i++) {
        unsigned long time = i * 10;
        signal_meter.update(time);
        station.step(time);
    }
    
    // Signal meter should remain at 0 (no charge from distant station)
    TEST_ASSERT_EQUAL(0, signal_meter.get_current_strength());
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_station_sends_charge_pulse_on_carrier_turn_on);
    RUN_TEST(test_station_no_charge_when_vfo_far_away);
    
    return UNITY_END();
}
