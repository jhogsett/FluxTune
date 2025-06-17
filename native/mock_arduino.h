#ifndef __MOCK_ARDUINO_H__
#define __MOCK_ARDUINO_H__

// Mock Arduino.h for native compilation
// This provides Arduino-compatible definitions for PC compilation

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <string>

// Basic Arduino types
typedef uint8_t byte;

// Digital pin values
#define HIGH 0x1
#define LOW  0x0

// Pin modes
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// Analog pins (mock definitions)
#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18
#define A5 19
#define A6 20
#define A7 21

// Flash memory support for AVR
#ifdef __AVR__
    #include <avr/pgmspace.h>
#else    // Mock PROGMEM support for native compilation
    #define PROGMEM
    #define PSTR(s) (s)
    #define F(s) ((const __FlashStringHelper*)(s))
    #define strcpy_P strcpy
    #define sprintf_P sprintf
    #define pgm_read_ptr(addr) (*(addr))
    #define pgm_read_byte(addr) (*(const unsigned char*)(addr))
    typedef const char* __FlashStringHelper;
#endif

// Mock Arduino functions for native compilation
#ifdef __cplusplus
extern "C" {
#endif

// Time functions
unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

// Digital I/O functions  
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

// Analog I/O functions
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);

#ifdef __cplusplus
}

// C++ only functions (overloaded functions need to be outside extern "C")
long random(long max);
long random(long min, long max);
void randomSeed(unsigned long seed);
#endif

#ifdef __cplusplus
// Arduino String class mock (minimal)
class String {
public:
    String() {}
    String(const char* str) : data(str ? str : "") {}
    String(int value) : data(std::to_string(value)) {}
    
    const char* c_str() const { return data.c_str(); }
    operator const char*() const { return c_str(); }
    
private:
    std::string data;
};
#endif

#endif // __MOCK_ARDUINO_H__
