#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#include <Wire.h>
#endif

#include "displays.h"

int main() {
    // Initialize display
    HT16K33Disp display(0x70, 2);
    DisplayManager displayManager;
    
    unsigned long time = 0;
    
    // Test static display
    displayManager.showString("HELLO", true);
    
    // Simulate time passing
    for (int i = 0; i < 100; i++) {
        time += 10;
        displayManager.update(time);
    }
    
    // Test scrolling
    displayManager.scrollString("This is a long scrolling message", 1000, 200);
    
    // Simulate time passing for scroll
    for (int i = 0; i < 500; i++) {
        time += 100;
        displayManager.update(time);
    }
    
    std::cout << "DisplayManager test completed successfully!" << std::endl;
    return 0;
}
