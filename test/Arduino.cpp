#ifndef ARDUINO

#include "Arduino.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

static unsigned long s_millis = 0;
static uint8_t s_pin_modes[256] = {0};
static uint8_t s_pin_states[256] = {0};

SerialClass Serial;

unsigned long millis() {
    return s_millis;
}

void delay(unsigned long ms) {
    s_millis += ms;
}

void pinMode(uint8_t pin, uint8_t mode) {
    if (pin < 256) s_pin_modes[pin] = mode;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    if (pin < 256) s_pin_states[pin] = val;
}

int digitalRead(uint8_t pin) {
    return (pin < 256) ? s_pin_states[pin] : LOW;
}

long random(long max) {
    return random(0, max);
}

long random(long min, long max) {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = true;
    }
    return min + (rand() % (max - min + 1));
}

#endif
