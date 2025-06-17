#include "native/platform.h"
#include "include/async_rtty.h"
#include <iostream>
#include <vector>

int main() {
    AsyncRTTY rtty;
    rtty.start_rtty(false);
    
    std::vector<unsigned long> meaningful_intervals;
    unsigned long prev_transition = 0;
    int prev_state = rtty.step_rtty(0);
    
    std::cout << "Time\tState\tInterval\n";
    std::cout << "0\t" << prev_state << "\t-\n";
    
    // Collect intervals, filtering out the immediate (1-step) transitions
    for (unsigned long time = 1; time <= 300; time += 1) {
        int state = rtty.step_rtty(time);
        if (state != prev_state) {
            if (prev_transition > 0) {
                unsigned long interval = time - prev_transition;
                std::cout << time << "\t" << state << "\t" << interval;
                
                // Only count meaningful intervals (not immediate transitions)
                if (interval > 5) {
                    meaningful_intervals.push_back(interval);
                    std::cout << " (meaningful)";
                }
                std::cout << "\n";
            } else {
                std::cout << time << "\t" << state << "\t-\n";
            }
            prev_transition = time;
            prev_state = state;
        }
    }
    
    std::cout << "\nMeaningful intervals:\n";
    for (auto interval : meaningful_intervals) {
        std::cout << interval << " ";
    }
    std::cout << "\n";
    
    std::cout << "\nTiming constants:\n";
    std::cout << "RTTY_TIME_BASIS: " << RTTY_TIME_BASIS << "\n";
    std::cout << "RTTY_TIME_BASIS2: " << RTTY_TIME_BASIS2 << "\n";
    std::cout << "Expected range: " << (RTTY_TIME_BASIS - 5) << " to " << ((RTTY_TIME_BASIS2 * 2) + 10) << "\n";
    
    // Check which intervals are out of range
    std::cout << "\nOut of range intervals:\n";
    for (auto interval : meaningful_intervals) {
        if (!(interval >= RTTY_TIME_BASIS - 5 && interval <= (RTTY_TIME_BASIS2 * 2) + 10)) {
            std::cout << interval << " ";
        }
    }
    std::cout << "\n";
    
    return 0;
}
