#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef

// Test VFO signal charge calculation - critical for dynamic station pipelining
#include "vfo.h"

void setUp(void) {
    // This is run before EACH test  
}

void tearDown(void) {
    // This is run after EACH test
}

void test_vfo_signal_charge_exact_match(void) {
    // Test charge calculation when VFO exactly matches station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002000.0;
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get maximum charge when exactly tuned
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_vfo_signal_charge_close_frequency(void) {
    // Test charge calculation when VFO is close to station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002100.0;  // 100 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get some charge when close
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_vfo_signal_charge_medium_distance(void) {
    // Test charge calculation when VFO is medium distance from station
    float station_freq = 7002000.0;
    float vfo_freq = 7003000.0;  // 1000 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get some charge when reasonably close
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_vfo_signal_charge_far_frequency(void) {
    // Test charge calculation when VFO is far from station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 3500000.0;  // Way off frequency (different band)
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get no charge when far away
    TEST_ASSERT_EQUAL(0, charge);
}

void test_vfo_signal_charge_edge_of_range(void) {
    // Test charge calculation at the edge of audible range
    float station_freq = 7002000.0;
    float vfo_freq = 7004500.0;  // 2500 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // At edge of range, should get minimal or no charge
    TEST_ASSERT_LESS_THAN(2, charge);  // Less than max charge
}

void test_vfo_signal_charge_symmetry(void) {
    // Test that charge calculation is symmetric (same distance above/below)
    float station_freq = 7002000.0;
    float offset = 500.0;
    
    int charge_above = VFO::calculate_signal_charge(station_freq, station_freq + offset);
    int charge_below = VFO::calculate_signal_charge(station_freq, station_freq - offset);
    
    // Should get same charge for same distance in either direction
    TEST_ASSERT_EQUAL(charge_above, charge_below);
}

void test_vfo_signal_charge_different_bands(void) {
    // Test charge calculation across different amateur bands
    
    // 40m band station
    float station_40m = 7002000.0;
    float vfo_20m = 14200000.0;  // 20m band
    
    int charge = VFO::calculate_signal_charge(station_40m, vfo_20m);
    
    // Different bands should give zero charge
    TEST_ASSERT_EQUAL(0, charge);
}

void test_vfo_signal_charge_uhf_frequencies(void) {
    // Test charge calculation with UHF frequencies (like pager stations)
    float station_freq = 146800000.0;  // 2m band
    float vfo_freq = 146800500.0;      // 500 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should work with UHF frequencies too
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_vfo_signal_charge_monotonic_decrease(void) {
    // Test that charge decreases monotonically as distance increases
    float station_freq = 7002000.0;
    
    int charge_100hz = VFO::calculate_signal_charge(station_freq, station_freq + 100.0);
    int charge_500hz = VFO::calculate_signal_charge(station_freq, station_freq + 500.0);
    int charge_1000hz = VFO::calculate_signal_charge(station_freq, station_freq + 1000.0);
    
    // Charge should decrease as distance increases
    TEST_ASSERT_GREATER_OR_EQUAL(charge_500hz, charge_1000hz);
    TEST_ASSERT_GREATER_OR_EQUAL(charge_100hz, charge_500hz);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_vfo_signal_charge_exact_match);
    RUN_TEST(test_vfo_signal_charge_close_frequency);
    RUN_TEST(test_vfo_signal_charge_medium_distance);
    RUN_TEST(test_vfo_signal_charge_far_frequency);
    RUN_TEST(test_vfo_signal_charge_edge_of_range);
    RUN_TEST(test_vfo_signal_charge_symmetry);
    RUN_TEST(test_vfo_signal_charge_different_bands);
    RUN_TEST(test_vfo_signal_charge_uhf_frequencies);
    RUN_TEST(test_vfo_signal_charge_monotonic_decrease);
    
    return UNITY_END();
}
