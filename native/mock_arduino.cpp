#include "mock_arduino.h"
#include <chrono>
#include <iostream>
#include <map>
#include <random>
#include <thread>

// Global state for mock Arduino
static auto start_time = std::chrono::high_resolution_clock::now();
static std::map<uint8_t, uint8_t> pin_modes;
static std::map<uint8_t, uint8_t> pin_states;
static std::random_device rd;
static std::mt19937 gen(rd());

// Time functions
unsigned long millis(void) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<unsigned long>(duration.count());
}

unsigned long micros(void) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
    return static_cast<unsigned long>(duration.count());
}

void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void delayMicroseconds(unsigned int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

// Digital I/O functions
void pinMode(uint8_t pin, uint8_t mode) {
    pin_modes[pin] = mode;
    std::cout << "[MOCK] pinMode(" << (int)pin << ", " << (mode == INPUT ? "INPUT" : mode == OUTPUT ? "OUTPUT" : "INPUT_PULLUP") << ")" << std::endl;
}

void digitalWrite(uint8_t pin, uint8_t val) {
    pin_states[pin] = val;
    std::cout << "[MOCK] digitalWrite(" << (int)pin << ", " << (val == HIGH ? "HIGH" : "LOW") << ")" << std::endl;
}

int digitalRead(uint8_t pin) {
    // Mock behavior: return LOW for most pins, HIGH occasionally
    static int counter = 0;
    counter++;
    int value = (counter % 10 == 0) ? HIGH : LOW;
    std::cout << "[MOCK] digitalRead(" << (int)pin << ") = " << (value == HIGH ? "HIGH" : "LOW") << std::endl;
    return value;
}

// Analog I/O functions
int analogRead(uint8_t pin) {
    // Return a mock analog value (0-1023)
    std::uniform_int_distribution<> dis(0, 1023);
    int value = dis(gen);
    std::cout << "[MOCK] analogRead(" << (int)pin << ") = " << value << std::endl;
    return value;
}

void analogWrite(uint8_t pin, int val) {
    std::cout << "[MOCK] analogWrite(" << (int)pin << ", " << val << ")" << std::endl;
}

// Math functions
long random(long max) {
    std::uniform_int_distribution<long> dis(0, max - 1);
    return dis(gen);
}

long random(long min, long max) {
    std::uniform_int_distribution<long> dis(min, max - 1);
    return dis(gen);
}

void randomSeed(unsigned long seed) {
    gen.seed(seed);
}
