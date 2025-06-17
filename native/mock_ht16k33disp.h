#ifndef MOCK_HT16K33DISP_H
#define MOCK_HT16K33DISP_H

#include "mock_arduino.h"

#define DEFAULT_ADDRESS_ 0x70
#define DEFAULT_NUM_DISPLAYS 1
#define DECIMAL_PT_SEGMENT 0x4000
#define NUM_DIGITS_PER_DISPLAY 4
#define DEFAULT_SHOW_DELAY 750
#define DEFAULT_SCROLL_DELAY 200

class HT16K33Disp {
public:
    HT16K33Disp() {}
    HT16K33Disp(int address) {}
    HT16K33Disp(int address, int num_displays) {}
    
    void begin() {}
    void clear() {}
    void display() {}
    void setBrightness(uint8_t brightness) {}
    void writeDigitNum(uint8_t pos, uint8_t num, bool dot = false) {}
    void writeDigitRaw(uint8_t pos, uint16_t bitmask) {}
    void print(const char* str) {}
    void print(int num) {}
    void print(float num, int decimals = 2) {}    void println(const char* str) {}
    void println(int num) {}
    void println(float num, int decimals = 2) {}
    void drawColon(bool state) {}
    void blinkRate(uint8_t rate) {}
    void show_string(const char* str) {}  // Add missing method
};

#endif // MOCK_HT16K33DISP_H
