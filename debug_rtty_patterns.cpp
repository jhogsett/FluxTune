#include "native/platform.h"
#include "include/async_rtty.h"
#include <iostream>

int main() {
    std::cout << "Testing pattern differences between RTTY runs...\n\n";
    
    // First run
    std::cout << "First run:\n";
    AsyncRTTY rtty1;
    rtty1.start_rtty(false);
    
    int pattern1[20];
    for (int i = 0; i < 20; i++) {
        pattern1[i] = rtty1.step_rtty(i * 5);
        std::cout << pattern1[i] << " ";
    }
    std::cout << "\n\n";
    
    // Second run
    std::cout << "Second run:\n";
    AsyncRTTY rtty2;
    rtty2.start_rtty(false);
    
    int pattern2[20];
    for (int i = 0; i < 20; i++) {
        pattern2[i] = rtty2.step_rtty(i * 5);
        std::cout << pattern2[i] << " ";
    }
    std::cout << "\n\n";
    
    // Compare patterns
    bool patterns_different = false;
    int different_count = 0;
    std::cout << "Differences:\n";
    for (int i = 0; i < 20; i++) {
        if (pattern1[i] != pattern2[i]) {
            patterns_different = true;
            different_count++;
            std::cout << "Position " << i << ": " << pattern1[i] << " vs " << pattern2[i] << "\n";
        }
    }
    
    std::cout << "\nResult: " << different_count << " differences out of 20 positions\n";
    std::cout << "Patterns are " << (patterns_different ? "DIFFERENT" : "IDENTICAL") << "\n";
    
    return 0;
}
