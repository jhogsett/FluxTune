#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef

// Test signal meter decay behavior - critical for dynamic station pipelining
#include "signal_meter.h"

void setUp(void) {
    // This is run before EACH test
}

void tearDown(void) {
    // This is run after EACH test
}

void test_signal_meter_initialization(void) {
    SignalMeter meter;
    meter.init();
    
    // Initial strength should be zero
    TEST_ASSERT_EQUAL(0, meter.get_current_strength());
    TEST_ASSERT_EQUAL(0, meter.get_panel_led_brightness());
}

void test_signal_meter_charge_accumulation(void) {
    SignalMeter meter;
    meter.init();
    
    // Add some charge
    meter.add_charge(10);
    
    // Should have accumulated the charge
    int strength = meter.get_current_strength();
    TEST_ASSERT_GREATER_THAN(0, strength);
    
    // Panel LED brightness should correlate with strength
    int brightness = meter.get_panel_led_brightness();
    TEST_ASSERT_GREATER_THAN(0, brightness);
}

void test_signal_meter_charge_clamping(void) {
    SignalMeter meter;
    meter.init();
    
    // Add massive charge (should be clamped)
    meter.add_charge(10000);
    
    // Should be clamped to maximum reasonable value
    int strength = meter.get_current_strength();
    TEST_ASSERT_LESS_THAN(300, strength);  // Reasonable maximum
}

void test_signal_meter_negative_charge_handling(void) {
    SignalMeter meter;
    meter.init();
    
    // Add some positive charge first
    meter.add_charge(50);
    int initial_strength = meter.get_current_strength();
    TEST_ASSERT_GREATER_THAN(0, initial_strength);
    
    // Add negative charge (signal lock behavior)
    meter.add_charge(-30);
    
    // Should reduce the signal strength
    int reduced_strength = meter.get_current_strength();
    TEST_ASSERT_LESS_THAN(initial_strength, reduced_strength);
}

void test_signal_meter_decay_over_time(void) {
    SignalMeter meter;
    meter.init();
    
    // Add initial charge
    meter.add_charge(100);
    int initial_strength = meter.get_current_strength();
    TEST_ASSERT_GREATER_THAN(0, initial_strength);
    
    // Simulate time passing with decay updates
    unsigned long time = 0;
    for (int i = 0; i < 10; i++) {
        time += 100;  // 100ms steps
        meter.update(time);
    }
    
    // Signal should have decayed
    int decayed_strength = meter.get_current_strength();
    TEST_ASSERT_LESS_THAN(initial_strength, decayed_strength);
}

void test_signal_meter_multiple_charge_pulses(void) {
    SignalMeter meter;
    meter.init();
    
    // Add multiple small charge pulses (simulating carrier on events)
    for (int i = 0; i < 5; i++) {
        meter.add_charge(10);
    }
    
    int strength = meter.get_current_strength();
    TEST_ASSERT_GREATER_THAN(40, strength);  // Should accumulate
    TEST_ASSERT_LESS_THAN(80, strength);    // But not linearly (capacitor behavior)
}

void test_signal_meter_panel_led_relationship(void) {
    SignalMeter meter;
    meter.init();
    
    // Test that panel LED brightness follows signal strength
    meter.add_charge(20);
    int strength1 = meter.get_current_strength();
    int brightness1 = meter.get_panel_led_brightness();
    
    meter.add_charge(30);
    int strength2 = meter.get_current_strength();
    int brightness2 = meter.get_panel_led_brightness();
    
    // More signal should mean more brightness
    if (strength2 > strength1) {
        TEST_ASSERT_GREATER_OR_EQUAL(brightness1, brightness2);
    }
}

void test_signal_meter_zero_decay_to_zero(void) {
    SignalMeter meter;
    meter.init();
    
    // Add charge then let it decay completely
    meter.add_charge(50);
    
    // Simulate long time passing
    unsigned long time = 0;
    for (int i = 0; i < 100; i++) {
        time += 100;  // 10 seconds total
        meter.update(time);
    }
    
    // Should eventually decay to zero or very close
    int final_strength = meter.get_current_strength();
    TEST_ASSERT_LESS_THAN(5, final_strength);  // Nearly zero
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_signal_meter_initialization);
    RUN_TEST(test_signal_meter_charge_accumulation);
    RUN_TEST(test_signal_meter_charge_clamping);
    RUN_TEST(test_signal_meter_negative_charge_handling);
    RUN_TEST(test_signal_meter_decay_over_time);
    RUN_TEST(test_signal_meter_multiple_charge_pulses);
    RUN_TEST(test_signal_meter_panel_led_relationship);
    RUN_TEST(test_signal_meter_zero_decay_to_zero);
    
    return UNITY_END();
}
