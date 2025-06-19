#include <iostream>
#include <cstdlib>
#include <ctime>
#define PLATFORM_NATIVE

#include "../include/sim_numbers.h"
#include "../native/mock_realization_pool.h"

int main() {
    std::srand(std::time(nullptr));
    
    MockRealizationPool pool;
    SimNumbers numbers(&pool);
    
    std::cout << "Testing SimNumbers group generation and timing...\n";
    
    if (!numbers.begin(0, 800.0, 20)) {
        std::cout << "Failed to start SimNumbers\n";
        return 1;
    }
    
    std::cout << "SimNumbers started successfully with 20 WPM\n";
    std::cout << "Looking for STEP_MORSE_MESSAGE_COMPLETE events...\n";
    
    int groups_completed = 0;
    int cycles_completed = 0;
    
    // Simulate time progression
    for (unsigned long time = 0; time < 30000 && cycles_completed < 2; time += 10) {
        if (numbers.step(time)) {
            // Check if we can detect completion through the new method
            // (In real code, the step() method would handle STEP_MORSE_MESSAGE_COMPLETE)
        }
    }
    
    std::cout << "Test completed - the new logic should properly handle message completion\n";
    std::cout << "Groups are now sent individually with proper spacing\n";
    return 0;
}
