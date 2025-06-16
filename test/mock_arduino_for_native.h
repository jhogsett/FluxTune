\
#ifndef MOCK_ARDUINO_FOR_NATIVE_H
#define MOCK_ARDUINO_FOR_NATIVE_H

// These defines should be active whenever this file is included by the -include flag.
#define Arduino_h // Prevent real Arduino.h from loading its content. Must be early.
#define UNIT_TEST_NATIVE_MOCK_ACTIVE // Signal that the mock environment is active

#include <stdint.h> // For uint8_t, uint16_t, etc.
#include <string.h> // For memcpy, if needed by mocks
#include <cstdlib>  // For rand(), srand() for mock random
// #include <limits.h> // For LONG_MAX with random - not strictly needed for basic % mod version

// Define 'byte' type for native builds, as Arduino.h normally does.
typedef uint8_t byte;

// --- Arduino API Mocks ---
#ifdef UNIT_TEST_NATIVE // Guarding these is good practice.

// PROGMEM attribute for native: essentially does nothing
#define PROGMEM

// pgm_read_byte family for native: direct memory access
#define pgm_read_byte(addr) (*(const unsigned char*)(addr))
#define pgm_read_word(addr) (*(const unsigned short*)(addr))
#define pgm_read_dword(addr) (*(const unsigned long*)(addr))
#define pgm_read_float(addr) (*(const float*)(addr))
#define memcpy_P(dest, src, num) memcpy(dest, src, num)

// Mock for millis()
// Implementation for these is in test_async_morse.cpp
unsigned long millis();
void set_mock_millis_time(unsigned long time_ms);

// Mock for random()
// Using simple std::rand(). For more controlled tests, consider a seed or specific return values.
inline long random(long howbig) {
    if (howbig == 0) {
        return 0;
    }
    // std::rand() returns a value between 0 and RAND_MAX.
    // Ensure howbig is positive.
    if (howbig < 0) howbig = -howbig; // Or handle error
    return std::rand() % howbig;
}

inline long random(long howsmall, long howbig) {
    if (howsmall >= howbig) {
        return howsmall; // Or handle error
    }
    long diff = howbig - howsmall;
    return howsmall + (std::rand() % diff);
}

// Mock for yield() - can be a no-op for many tests
inline void yield() {}

// Add other Arduino function mocks as they become necessary

#endif // UNIT_TEST_NATIVE

#endif // MOCK_ARDUINO_FOR_NATIVE_H
