#include <iostream>
#include <cstdlib>

// Include native platform mock first
#include "native/platform.h"

// Include our headers to measure their memory footprint
#include "include/async_morse.h"
#include "include/async_rtty.h"
#include "include/sim_transmitter.h"
#include "include/sim_station.h"
#include "include/sim_rtty.h"
#include "include/buffers.h"
#include "include/vfo.h"
#include "include/realization_pool.h"
#include "include/realizer_pool.h"

// Simple memory measurement helpers
template<typename T>
void print_sizeof(const char* name) {
    std::cout << name << ": " << sizeof(T) << " bytes" << std::endl;
}

void print_array_usage() {
    std::cout << "\n=== ARRAY AND BUFFER USAGE ===" << std::endl;
    std::cout << "fstring_buffer[" << FSTRING_BUFFER << "]: " << FSTRING_BUFFER << " bytes" << std::endl;
    std::cout << "display_text_buffer[10]: 10 bytes" << std::endl;
    std::cout << "Realizer *realizers[4]: " << (4 * sizeof(void*)) << " bytes" << std::endl;
    std::cout << "bool realizer_stats[4]: 4 bytes" << std::endl;
    std::cout << "Realization *realizations[4]: " << (4 * sizeof(void*)) << " bytes" << std::endl;
    std::cout << "bool realization_stats[4]: 4 bytes" << std::endl;
    std::cout << "ModeHandler arrays (3+3+1): " << (7 * sizeof(void*)) << " bytes" << std::endl;
}

void estimate_main_objects() {
    std::cout << "\n=== ESTIMATED MAIN.CPP OBJECTS ===" << std::endl;
    std::cout << "SimStation instances (3x): " << (3 * sizeof(SimStation)) << " bytes" << std::endl;
    std::cout << "SimRTTY instance (1x): " << sizeof(SimRTTY) << " bytes" << std::endl;
    std::cout << "VFO instances (3x): " << (3 * sizeof(VFO)) << " bytes" << std::endl;
    std::cout << "RealizerPool instance: " << sizeof(RealizerPool) << " bytes" << std::endl;
    std::cout << "RealizationPool instance: " << sizeof(RealizationPool) << " bytes" << std::endl;
}

int main() {
    std::cout << "=== FLUXTUNE RAM ANALYSIS ===" << std::endl;
    
    std::cout << "\n=== BASIC TYPE SIZES ===" << std::endl;
    std::cout << "Pointer size: " << sizeof(void*) << " bytes" << std::endl;
    std::cout << "int: " << sizeof(int) << " bytes" << std::endl;
    std::cout << "float: " << sizeof(float) << " bytes" << std::endl;
    std::cout << "bool: " << sizeof(bool) << " bytes" << std::endl;
    std::cout << "byte: " << sizeof(byte) << " bytes" << std::endl;
    std::cout << "unsigned long: " << sizeof(unsigned long) << " bytes" << std::endl;
    
    std::cout << "\n=== CORE CLASS SIZES ===" << std::endl;
    print_sizeof<AsyncMorse>("AsyncMorse");
    print_sizeof<AsyncRTTY>("AsyncRTTY");
    print_sizeof<SimTransmitter>("SimTransmitter");
    print_sizeof<SimStation>("SimStation");
    print_sizeof<SimRTTY>("SimRTTY");
    
    std::cout << "\n=== POOL AND MANAGEMENT CLASSES ===" << std::endl;
    print_sizeof<RealizerPool>("RealizerPool");
    print_sizeof<RealizationPool>("RealizationPool");
    print_sizeof<VFO>("VFO");
    
    print_array_usage();
    estimate_main_objects();
    
    // Calculate totals
    int total_arrays = FSTRING_BUFFER + 10 + (4 * sizeof(void*)) + 4 + (4 * sizeof(void*)) + 4 + (7 * sizeof(void*));
    int total_objects = (3 * sizeof(SimStation)) + sizeof(SimRTTY) + (3 * sizeof(VFO)) + sizeof(RealizerPool) + sizeof(RealizationPool);
    
    std::cout << "\n=== TOTALS ===" << std::endl;
    std::cout << "Arrays and buffers: " << total_arrays << " bytes" << std::endl;
    std::cout << "Main objects: " << total_objects << " bytes" << std::endl;
    std::cout << "Estimated total: " << (total_arrays + total_objects) << " bytes" << std::endl;
    std::cout << "\nNote: This excludes Arduino libraries, stack usage, and heap allocations." << std::endl;
    
    return 0;
}
