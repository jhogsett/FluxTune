#include "../native/platform.h"
#include "../include/async_rtty.h"
#include <iostream>

int main() {
    std::cout << "=== AsyncRTTY Timing Debug ===" << std::endl;
    std::cout << "RTTY_TIME_BASIS: " << RTTY_TIME_BASIS << std::endl;
    std::cout << "RTTY_TIME_BASIS2: " << RTTY_TIME_BASIS2 << std::endl;
    std::cout << std::endl;
    
    for (int run = 0; run < 3; run++) {
        std::cout << "--- Run " << (run + 1) << " ---" << std::endl;
        AsyncRTTY rtty;
        rtty.start_rtty(false);
        
        int prev_state = rtty.step_rtty(0);
        unsigned long prev_transition = 0;
        std::cout << "Time 0: State " << prev_state << std::endl;
        
        std::vector<unsigned long> intervals;
        
        for (unsigned long time = 1; time <= 200; time += 1) {
            int state = rtty.step_rtty(time);
            if (state != prev_state) {
                unsigned long interval = time - prev_transition;
                intervals.push_back(interval);
                std::cout << "Time " << time << ": State " << state 
                          << " (interval: " << interval << ")" << std::endl;
                prev_transition = time;
                prev_state = state;
            }
        }
        
        std::cout << "Intervals found: ";
        for (auto i : intervals) {
            std::cout << i << " ";
        }
        std::cout << std::endl << std::endl;
    }
    
    return 0;
}
