#ifndef __MOCK_ENCODER_H__
#define __MOCK_ENCODER_H__

// Mock Encoder.h for native compilation
#include "mock_arduino.h"
#include <iostream>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB) : pin_a(pinA), pin_b(pinB), position(0) {
        std::cout << "[MOCK] Encoder(" << (int)pinA << ", " << (int)pinB << ")" << std::endl;
    }
    
    long read() {
        // Simulate encoder movement occasionally
        static int counter = 0;
        counter++;
        if (counter % 100 == 0) {
            position += (counter % 200 < 100) ? 1 : -1;
        }
        return position;
    }
    
    void write(long newPosition) {
        std::cout << "[MOCK] Encoder.write(" << newPosition << ")" << std::endl;
        position = newPosition;
    }

private:
    uint8_t pin_a, pin_b;
    long position;
};

#endif // __MOCK_ENCODER_H__
