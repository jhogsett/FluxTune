#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t byte;

class SerialClass {
public:
    void begin(unsigned long) {}
    size_t write(uint8_t) { return 1; }
    size_t print(const char*) { return 0; }
    size_t println(const char*) { return 0; }
    size_t println(int) { return 0; }
    void flush() {}
    void end() {}
};

extern SerialClass Serial;

#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define LED_BUILTIN 13
#define NULL 0

#define PROGMEM
#define pgm_read_ptr(addr) (*(addr))
inline uint8_t pgm_read_byte(const uint8_t* addr) { return *addr; }

unsigned long millis();
void delay(unsigned long ms);
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
long random(long max);
long random(long min, long max);

#endif
