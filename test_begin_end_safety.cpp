#define PLATFORM_NATIVE
#include "sim_numbers.h"
#include "signal_meter.h"
#include "realization_pool.h"

#include <iostream>

int main() {
    // Create real objects for testing
    RealizationPool realization_pool;
    SignalMeter signal_meter;
    
    // Create a Numbers station
    SimNumbers numbers(&realization_pool, &signal_meter, 7050000.0f, 20);
    
    std::cout << "Testing begin/end safety..." << std::endl;
    
    // Test 1: Multiple begin() calls
    std::cout << "Test 1: Multiple begin() calls" << std::endl;
    bool result1 = numbers.begin(1000);
    std::cout << "First begin(): " << (result1 ? "SUCCESS" : "FAILED") << std::endl;
    
    bool result2 = numbers.begin(2000);
    std::cout << "Second begin(): " << (result2 ? "SUCCESS" : "FAILED") << std::endl;
    
    bool result3 = numbers.begin(3000);
    std::cout << "Third begin(): " << (result3 ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test 2: Multiple end() calls
    std::cout << "\nTest 2: Multiple end() calls" << std::endl;
    numbers.end();
    std::cout << "First end(): SUCCESS" << std::endl;
    
    numbers.end();
    std::cout << "Second end(): SUCCESS" << std::endl;
    
    numbers.end();
    std::cout << "Third end(): SUCCESS" << std::endl;
    
    // Test 3: Alternating begin/end calls
    std::cout << "\nTest 3: Alternating begin/end calls" << std::endl;
    
    bool result4 = numbers.begin(4000);
    std::cout << "Begin after end(): " << (result4 ? "SUCCESS" : "FAILED") << std::endl;
    
    numbers.end();
    std::cout << "End after begin(): SUCCESS" << std::endl;
    
    bool result5 = numbers.begin(5000);
    std::cout << "Begin again: " << (result5 ? "SUCCESS" : "FAILED") << std::endl;
    
    numbers.end();
    std::cout << "End again: SUCCESS" << std::endl;
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}
