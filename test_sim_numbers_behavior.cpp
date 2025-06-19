#define PLATFORM_NATIVE
#include <iostream>
#include <cstdlib>
#include <ctime>

// Mock the required includes
#include "include/sim_numbers.h"
#include "native/mock_realization_pool.h"

int main() {
    srand(time(nullptr));
    
    MockRealizationPool pool;
    SimNumbers numbers(&pool);
    
    std::cout << "Testing SimNumbers behavior:\n";
    std::cout << "=============================\n";
    
    // Test the group generation
    std::cout << "\nTesting group generation (should be 5 digits only):\n";
    for(int i = 0; i < 5; i++) {
        numbers.generate_next_number_group();
        std::cout << "Group " << (i+1) << ": '" << numbers._group_buffer << "' (length: " << strlen(numbers._group_buffer) << ")\n";
    }
    
    std::cout << "\nBuffer size: " << sizeof(numbers._group_buffer) << " bytes\n";
    std::cout << "Groups per cycle: " << numbers._total_groups_per_cycle << "\n";
    
    return 0;
}
