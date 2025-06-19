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
    HT16K33Disp() : _num_displays(1), _num_digits(4) {}
    HT16K33Disp(int address) : _num_displays(1), _num_digits(4) {}
    HT16K33Disp(int address, int num_displays) : _num_displays(num_displays), _num_digits(num_displays * 4) {}
    
    void begin() {}
    void clear() {}
    void display() {}
    void setBrightness(uint8_t brightness) {}
    void writeDigitNum(uint8_t pos, uint8_t num, bool dot = false) {}
    void writeDigitRaw(uint8_t pos, uint16_t bitmask) {}
    void print(const char* str) {}
    void print(int num) {}
    void print(float num, int decimals = 2) {}
    void println(const char* str) {}
    void println(int num) {}
    void println(float num, int decimals = 2) {}
    void drawColon(bool state) {}
    void blinkRate(uint8_t rate) {}
    
    // Display manager compatible methods
    void show_string(const char* str, bool pad_blanks = true) {}
    void scroll_string(const char* str, int show_delay = 0, int scroll_delay = 0) {}
    void simple_show_string(const char* str) {}
    int begin_scroll_string(const char* str, int show_delay = 0, int scroll_delay = 0) { return 0; }
    bool step_scroll_string(unsigned long time) { return false; }
    void init(const byte* brightLevels) {}
    
    // Public member variables (like the real class)
    int _num_displays;
    int _num_digits;
};

#endif // MOCK_HT16K33DISP_H
