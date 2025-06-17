#include "native/platform.h"
#include "include/async_rtty.h"
#include <iostream>

class DebugAsyncRTTY : public AsyncRTTY {
public:
    void debug_info() {
        std::cout << "Element: " << (int)async_element 
                  << ", Active: " << async_active 
                  << ", SwitchedOn: " << async_switched_on 
                  << ", Done: " << async_element_done 
                  << ", NextEvent: " << async_next_event << std::endl;
    }
};

int main() {
    std::cout << "Debugging RTTY internal state...\n\n";
    
    DebugAsyncRTTY rtty;
    rtty.start_rtty(false);
    
    std::cout << "Time\tState\tInternal State\n";
    for (unsigned long time = 0; time < 100; time += 5) {
        int state = rtty.step_rtty(time);
        std::cout << time << "\t" << state << "\t";
        rtty.debug_info();
    }
    
    return 0;
}
