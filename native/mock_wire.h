#ifndef __MOCK_WIRE_H__
#define __MOCK_WIRE_H__

// Mock Wire.h for native compilation
#include "mock_arduino.h"
#include <iostream>

class TwoWire {
public:
    void begin() {
        std::cout << "[MOCK] Wire.begin()" << std::endl;
    }
    
    void beginTransmission(uint8_t address) {
        std::cout << "[MOCK] Wire.beginTransmission(0x" << std::hex << (int)address << ")" << std::endl;
        current_address = address;
    }
    
    size_t write(uint8_t data) {
        std::cout << "[MOCK] Wire.write(0x" << std::hex << (int)data << ")" << std::endl;
        return 1;
    }
    
    size_t write(const uint8_t* data, size_t length) {
        std::cout << "[MOCK] Wire.write(data, " << length << ")" << std::endl;
        return length;
    }
    
    uint8_t endTransmission(bool sendStop = true) {
        std::cout << "[MOCK] Wire.endTransmission(" << (sendStop ? "true" : "false") << ")" << std::endl;
        return 0; // Success
    }
    
    uint8_t requestFrom(uint8_t address, uint8_t length, bool sendStop = true) {
        std::cout << "[MOCK] Wire.requestFrom(0x" << std::hex << (int)address << ", " << (int)length << ")" << std::endl;
        available_bytes = length;
        return length;
    }
    
    int available() {
        return available_bytes;
    }
    
    int read() {
        if (available_bytes > 0) {
            available_bytes--;
            return 0x42; // Mock data
        }
        return -1;
    }

private:
    uint8_t current_address = 0;
    int available_bytes = 0;
};

extern TwoWire Wire;

#endif // __MOCK_WIRE_H__
