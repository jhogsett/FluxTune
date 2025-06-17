#ifndef __MOCK_POLOLU_LED_STRIP_H__
#define __MOCK_POLOLU_LED_STRIP_H__

// Mock PololuLedStrip.h for native compilation
#include "mock_arduino.h"
#include <iostream>
#include <vector>

struct rgb_color {
    uint8_t red, green, blue;
    
    rgb_color() : red(0), green(0), blue(0) {}
    rgb_color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
};

template<uint8_t pin>
class PololuLedStrip {
public:
    void write(rgb_color* colors, uint16_t count) {
        std::cout << "[MOCK] PololuLedStrip<" << (int)pin << ">.write(" << count << " colors)" << std::endl;
        for (uint16_t i = 0; i < count && i < 10; i++) { // Limit output for readability
            std::cout << "  [" << i << "] RGB(" << (int)colors[i].red << "," << (int)colors[i].green << "," << (int)colors[i].blue << ")" << std::endl;
        }
        if (count > 10) {
            std::cout << "  ... and " << (count - 10) << " more" << std::endl;
        }
    }
    
    static void interruptFriendlyWrite(rgb_color* colors, uint16_t count) {
        std::cout << "[MOCK] PololuLedStrip::interruptFriendlyWrite(" << count << " colors)" << std::endl;
    }
};

#endif // __MOCK_POLOLU_LED_STRIP_H__
