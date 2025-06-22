#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef

// Test the critical VFO signal charge calculation in isolation
// This is a static method that can be tested independently

// Include just what we need
#include "basic_types.h"

// Copy the critical function from VFO class for isolated testing
// This ensures the logic is tested without complex dependencies
int calculate_signal_charge_test(float station_freq, float vfo_freq) {
    // Calculate the frequency difference
    float freq_diff = station_freq - vfo_freq;
    if (freq_diff < 0) freq_diff = -freq_diff;  // Absolute value
    
    // Signal charge based on proximity (tuning window)
    const float MAX_RANGE = 2500.0;  // ±2500 Hz for signal detection
    
    if (freq_diff > MAX_RANGE) {
        return 0;  // No signal when far away
    }
    
    // Linear falloff from 2 (exact match) to 0 (edge of range)
    float ratio = (MAX_RANGE - freq_diff) / MAX_RANGE;
    int charge = (int)(ratio * 2.0);  // Scale to 0-2 range
    
    return charge;
}

void setUp(void) {
    // This is run before EACH test  
}

void tearDown(void) {
    // This is run after EACH test
}

void test_signal_charge_exact_match(void) {
    // Test charge calculation when VFO exactly matches station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002000.0;
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Should get maximum charge (2) when exactly tuned
    TEST_ASSERT_EQUAL(2, charge);
}

void test_signal_charge_close_frequency(void) {
    // Test charge calculation when VFO is close to station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002100.0;  // 100 Hz away
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Should get strong charge when close
    TEST_ASSERT_GREATER_THAN(1, charge);
    TEST_ASSERT_LESS_OR_EQUAL(2, charge);
}

void test_signal_charge_medium_distance(void) {
    // Test charge calculation when VFO is medium distance from station
    float station_freq = 7002000.0;
    float vfo_freq = 7003000.0;  // 1000 Hz away
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Should get moderate charge when reasonably close
    TEST_ASSERT_GREATER_THAN(0, charge);
    TEST_ASSERT_LESS_THAN(2, charge);
}

void test_signal_charge_far_frequency(void) {
    // Test charge calculation when VFO is far from station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 3500000.0;  // Way off frequency (different band)
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Should get no charge when far away
    TEST_ASSERT_EQUAL(0, charge);
}

void test_signal_charge_edge_of_range(void) {
    // Test charge calculation at the edge of detection range
    float station_freq = 7002000.0;
    float vfo_freq = 7004500.0;  // Exactly 2500 Hz away (edge)
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // At exact edge, should get minimal charge (0)
    TEST_ASSERT_EQUAL(0, charge);
}

void test_signal_charge_just_inside_range(void) {
    // Test charge calculation just inside detection range
    float station_freq = 7002000.0;
    float vfo_freq = 7004499.0;  // Just under 2500 Hz away
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Just inside range should give minimal but non-zero charge
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_signal_charge_symmetry(void) {
    // Test that charge calculation is symmetric (same distance above/below)
    float station_freq = 7002000.0;
    float offset = 500.0;
    
    int charge_above = calculate_signal_charge_test(station_freq, station_freq + offset);
    int charge_below = calculate_signal_charge_test(station_freq, station_freq - offset);
    
    // Should get same charge for same distance in either direction
    TEST_ASSERT_EQUAL(charge_above, charge_below);
}

void test_signal_charge_monotonic_decrease(void) {
    // Test that charge decreases monotonically as distance increases
    float station_freq = 7002000.0;
    
    int charge_100hz = calculate_signal_charge_test(station_freq, station_freq + 100.0);
    int charge_500hz = calculate_signal_charge_test(station_freq, station_freq + 500.0);
    int charge_1000hz = calculate_signal_charge_test(station_freq, station_freq + 1000.0);
    int charge_2000hz = calculate_signal_charge_test(station_freq, station_freq + 2000.0);
    
    // Charge should decrease as distance increases
    TEST_ASSERT_GREATER_OR_EQUAL(charge_100hz, charge_500hz);
    TEST_ASSERT_GREATER_OR_EQUAL(charge_500hz, charge_1000hz);
    TEST_ASSERT_GREATER_OR_EQUAL(charge_1000hz, charge_2000hz);
}

void test_signal_charge_uhf_frequencies(void) {
    // Test charge calculation with UHF frequencies (like pager stations)
    float station_freq = 146800000.0;  // 2m band
    float vfo_freq = 146800500.0;      // 500 Hz away
    
    int charge = calculate_signal_charge_test(station_freq, vfo_freq);
    
    // Should work with UHF frequencies too
    TEST_ASSERT_GREATER_THAN(0, charge);
}

void test_signal_charge_boundary_conditions(void) {
    // Test various boundary conditions
    float station_freq = 7002000.0;
    
    // Test zero frequency difference
    TEST_ASSERT_EQUAL(2, calculate_signal_charge_test(station_freq, station_freq));
    
    // Test maximum range
    TEST_ASSERT_EQUAL(0, calculate_signal_charge_test(station_freq, station_freq + 2500.0));
    TEST_ASSERT_EQUAL(0, calculate_signal_charge_test(station_freq, station_freq - 2500.0));
    
    // Test just beyond range
    TEST_ASSERT_EQUAL(0, calculate_signal_charge_test(station_freq, station_freq + 2501.0));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_signal_charge_exact_match);
    RUN_TEST(test_signal_charge_close_frequency);
    RUN_TEST(test_signal_charge_medium_distance);
    RUN_TEST(test_signal_charge_far_frequency);
    RUN_TEST(test_signal_charge_edge_of_range);
    RUN_TEST(test_signal_charge_just_inside_range);
    RUN_TEST(test_signal_charge_symmetry);
    RUN_TEST(test_signal_charge_monotonic_decrease);
    RUN_TEST(test_signal_charge_uhf_frequencies);
    RUN_TEST(test_signal_charge_boundary_conditions);
    
    return UNITY_END();
}
