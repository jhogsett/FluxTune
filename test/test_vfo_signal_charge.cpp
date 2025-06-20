#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "vfo.h"

void setUp(void) {
    // This is run before EACH test
}

void tearDown(void) {
    // This is run after EACH test
}

void test_vfo_charge_calculation_exact_match(void) {
    // Test charge calculation when VFO exactly matches station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002000.0;
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get maximum charge (2) when exactly tuned
    TEST_ASSERT_EQUAL(2, charge);
}

void test_vfo_charge_calculation_close_frequency(void) {
    // Test charge calculation when VFO is close to station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 7002500.0;  // 500 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get some charge (>0 but <2) when close
    TEST_ASSERT_GREATER_THAN(0, charge);
    TEST_ASSERT_LESS_THAN(2, charge);
}

void test_vfo_charge_calculation_far_frequency(void) {
    // Test charge calculation when VFO is far from station frequency
    float station_freq = 7002000.0;
    float vfo_freq = 3500000.0;  // Way off frequency
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get no charge when far away
    TEST_ASSERT_EQUAL(0, charge);
}

void test_vfo_charge_calculation_edge_of_range(void) {
    // Test charge calculation at the edge of the range (±2500 Hz)
    float station_freq = 7002000.0;
    float vfo_freq = 7004500.0;  // Exactly 2500 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get minimal charge at edge of range
    TEST_ASSERT_GREATER_OR_EQUAL(0, charge);
    TEST_ASSERT_LESS_OR_EQUAL(1, charge);
}

void test_vfo_charge_calculation_just_outside_range(void) {
    // Test charge calculation just outside the range
    float station_freq = 7002000.0;
    float vfo_freq = 7004501.0;  // Just over 2500 Hz away
    
    int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
    
    // Should get no charge outside range
    TEST_ASSERT_EQUAL(0, charge);
}

void test_vfo_charge_calculation_with_new_parameters(void) {
    // Test that with the new parameters (wider range, lower charge), 
    // we get reasonable values
    
    float station_freq = 7002000.0;
    
    // Test various distances
    struct {
        float freq_offset;
        int expected_min;
        int expected_max;
    } test_cases[] = {
        {0.0,      2, 2},    // Exact match - max charge
        {100.0,    1, 2},    // Very close - high charge
        {500.0,    1, 2},    // Close - medium charge
        {1000.0,   0, 1},    // Medium distance - low charge
        {2000.0,   0, 1},    // Far but in range - minimal charge
        {2500.0,   0, 0},    // Edge of range - minimal or no charge
        {3000.0,   0, 0},    // Out of range - no charge
    };
    
    for (int i = 0; i < 7; i++) {
        float vfo_freq = station_freq + test_cases[i].freq_offset;
        int charge = VFO::calculate_signal_charge(station_freq, vfo_freq);
        
        char msg[100];
        sprintf(msg, "Offset %.1f Hz: charge %d not in range [%d, %d]", 
                test_cases[i].freq_offset, charge, 
                test_cases[i].expected_min, test_cases[i].expected_max);
        
        TEST_ASSERT_GREATER_OR_EQUAL_MESSAGE(test_cases[i].expected_min, charge, msg);
        TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(test_cases[i].expected_max, charge, msg);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_vfo_charge_calculation_exact_match);
    RUN_TEST(test_vfo_charge_calculation_close_frequency);
    RUN_TEST(test_vfo_charge_calculation_far_frequency);
    RUN_TEST(test_vfo_charge_calculation_edge_of_range);
    RUN_TEST(test_vfo_charge_calculation_just_outside_range);
    RUN_TEST(test_vfo_charge_calculation_with_new_parameters);
    
    return UNITY_END();
}
