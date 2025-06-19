// Test demonstrating the capacitor-like signal meter behavior

#include "signal_meter.h"
#include <iostream>

int main() {
    SignalMeter meter;
    meter.init();
    
    std::cout << "Testing capacitor-like signal meter behavior:\n\n";
    
    // Simulate charge pulses
    std::cout << "Adding charge pulses...\n";
    for (int i = 0; i < 5; i++) {
        meter.add_charge(10);
        std::cout << "Charge pulse " << (i+1) << " added\n";
    }
    
    // Simulate time-based decay
    std::cout << "\nSimulating decay over time...\n";
    unsigned long sim_time = 0;
    for (int i = 0; i < 10; i++) {
        sim_time += 100;  // Advance time by 100ms
        meter.update(sim_time);
        std::cout << "Time: " << sim_time << "ms - Decay update applied\n";
    }
    
    std::cout << "\nCapacitor behavior test complete!\n";
    return 0;
}
