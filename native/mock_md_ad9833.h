#ifndef __MOCK_MD_AD9833_H__
#define __MOCK_MD_AD9833_H__

// Mock MD_AD9833.h for native compilation
#include "mock_arduino.h"
#include <iostream>

class MD_AD9833 {
public:
    enum channel_t { CHAN_0 = 0, CHAN_1 = 1 };
    enum mode_t { MODE_OFF, MODE_SINE, MODE_TRIANGLE, MODE_SQUARE1, MODE_SQUARE2 };
    
    MD_AD9833(uint8_t dataPin = 0, uint8_t clkPin = 0, uint8_t fsyncPin = 0) 
        : data_pin(dataPin), clk_pin(clkPin), fsync_pin(fsyncPin) {
        std::cout << "[MOCK] MD_AD9833(" << (int)dataPin << ", " << (int)clkPin << ", " << (int)fsyncPin << ")" << std::endl;
    }
    
    void begin() {
        std::cout << "[MOCK] MD_AD9833.begin()" << std::endl;
    }
    
    void setFrequency(channel_t chan, float frequency) {
        std::cout << "[MOCK] MD_AD9833.setFrequency(" << (int)chan << ", " << frequency << ")" << std::endl;
        freq[chan] = frequency;
    }
    
    void setMode(mode_t mode) {
        std::cout << "[MOCK] MD_AD9833.setMode(" << (int)mode << ")" << std::endl;
        current_mode = mode;
    }
    
    void setActiveFrequency(channel_t chan) {
        std::cout << "[MOCK] MD_AD9833.setActiveFrequency(" << (int)chan << ")" << std::endl;
        active_channel = chan;
    }
    
    float getFrequency(channel_t chan) {
        return freq[chan];
    }

private:
    uint8_t data_pin, clk_pin, fsync_pin;
    float freq[2] = {0.0, 0.0};
    mode_t current_mode = MODE_OFF;
    channel_t active_channel = CHAN_0;
};

#endif // __MOCK_MD_AD9833_H__
