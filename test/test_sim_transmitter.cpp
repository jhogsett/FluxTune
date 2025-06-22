#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "sim_transmitter.h"
#include "sim_station.h"
#include "sim_numbers.h"

// Mock components for testing
SignalMeter mock_signal_meter;
WaveGenPool mock_realizer_pool(nullptr, nullptr, 0);

void setUp(void) {
    mock_signal_meter.init();
}

void tearDown(void) {
    // Clean up
}

void test_sim_transmitter_initialization(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test initial state
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, station.get_fixed_frequency());
    TEST_ASSERT_EQUAL(DORMANT, station.get_station_state());
    TEST_ASSERT_FALSE(station.isActive());
    TEST_ASSERT_FALSE(station.is_audible());
}

void test_sim_transmitter_state_transitions(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test state transitions
    station.set_station_state(ACTIVE);
    TEST_ASSERT_EQUAL(ACTIVE, station.get_station_state());
    TEST_ASSERT_FALSE(station.is_audible());
    
    station.set_station_state(AUDIBLE);
    TEST_ASSERT_EQUAL(AUDIBLE, station.get_station_state());
    TEST_ASSERT_TRUE(station.is_audible());
    
    station.set_station_state(SILENT);
    TEST_ASSERT_EQUAL(SILENT, station.get_station_state());
    TEST_ASSERT_FALSE(station.is_audible());
    
    station.set_station_state(DORMANT);
    TEST_ASSERT_EQUAL(DORMANT, station.get_station_state());
    TEST_ASSERT_FALSE(station.is_audible());
}

void test_sim_transmitter_active_flag(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test active flag operations
    TEST_ASSERT_FALSE(station.isActive());
    
    station.setActive(true);
    TEST_ASSERT_TRUE(station.isActive());
    
    station.setActive(false);
    TEST_ASSERT_FALSE(station.isActive());
}

void test_sim_transmitter_frequency_bounds_checking(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test frequency access
    float freq = station.get_fixed_frequency();
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, freq);
    
    // Test that frequency is reasonable for amateur radio
    TEST_ASSERT_GREATER_THAN(1000000.0, freq);  // Above 1 MHz
    TEST_ASSERT_LESS_THAN(30000000.0, freq);    // Below 30 MHz (HF bands)
}

void test_sim_transmitter_reinitialize(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test reinitialize with new frequency
    bool result = station.reinitialize(1000, 14205000.0);
    TEST_ASSERT_TRUE(result);
    
    // Should have new frequency
    TEST_ASSERT_EQUAL_FLOAT(14205000.0, station.get_fixed_frequency());
    
    // Should be in ACTIVE state after reinitialize
    TEST_ASSERT_EQUAL(ACTIVE, station.get_station_state());
}

void test_sim_transmitter_charge_pulse_logic(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Initialize station for testing
    station.begin(0);
    
    // Test VFO frequency setting for charge calculation
    station.set_vfo_frequency_for_test(7002000.0);  // Exact match
    
    // This test validates that the charge pulse infrastructure exists
    // The actual charge pulse logic is tested in test_signal_meter_carrier_integration.cpp
    TEST_ASSERT_TRUE(true);
}

void test_sim_transmitter_numbers_station_specifics(void) {
    SimNumbers numbers_station(&mock_realizer_pool, &mock_signal_meter, 7004000.0, 18);
    
    // Test Numbers station initialization
    TEST_ASSERT_EQUAL_FLOAT(7004000.0, numbers_station.get_fixed_frequency());
    TEST_ASSERT_EQUAL(DORMANT, numbers_station.get_station_state());
    
    // Test state transitions work for Numbers stations too
    numbers_station.set_station_state(AUDIBLE);
    TEST_ASSERT_TRUE(numbers_station.is_audible());
}

void test_sim_transmitter_idempotent_operations(void) {
    SimStation station(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
    
    // Test that begin() can be called multiple times safely
    bool result1 = station.begin(1000);
    bool result2 = station.begin(1500);
    
    // Both should succeed (idempotent)
    TEST_ASSERT_TRUE(result1);
    TEST_ASSERT_TRUE(result2);
    
    // Test that end() can be called multiple times safely
    station.end();
    station.end();  // Should not crash
    
    TEST_ASSERT_TRUE(true);  // If we get here, operations were safe
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_sim_transmitter_initialization);
    RUN_TEST(test_sim_transmitter_state_transitions);
    RUN_TEST(test_sim_transmitter_active_flag);
    RUN_TEST(test_sim_transmitter_frequency_bounds_checking);
    RUN_TEST(test_sim_transmitter_reinitialize);
    RUN_TEST(test_sim_transmitter_charge_pulse_logic);
    RUN_TEST(test_sim_transmitter_numbers_station_specifics);
    RUN_TEST(test_sim_transmitter_idempotent_operations);
    
    return UNITY_END();
}
