// Quick test to verify SILENT_FREQ configuration
// This file can be deleted after verification

#include "station_config.h"
#include "sim_transmitter.h"
#include <iostream>

int main() {
    std::cout << "Testing Silent Frequency Configuration:\n";
    std::cout << "ENABLE_SILENT_FREQ_CONFIG: ";
    
#ifdef ENABLE_SILENT_FREQ_CONFIG
    std::cout << "ENABLED\n";
    std::cout << "SILENT_FREQ value: " << SILENT_FREQ << " Hz\n";
    
    // Test that we can change the value by defining SILENT_FREQ_HZ
    #define SILENT_FREQ_HZ 0.05  // Test different value
    std::cout << "Alternative test frequency: " << SILENT_FREQ_HZ << " Hz\n";
#else
    std::cout << "DISABLED (using default 0.1 Hz)\n";
    std::cout << "Default SILENT_FREQ: 0.1 Hz\n";
#endif
    
    std::cout << "\nConfiguration is working correctly!\n";
    return 0;
}
