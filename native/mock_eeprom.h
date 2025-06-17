#ifndef __MOCK_EEPROM_H__
#define __MOCK_EEPROM_H__

// Mock EEPROM.h for native compilation
#include "mock_arduino.h"
#include <iostream>
#include <vector>
#include <fstream>

class EEPROMClass {
public:
    EEPROMClass() : data(1024, 0xFF) {
        loadFromFile();
    }
    
    ~EEPROMClass() {
        saveToFile();
    }
    
    uint8_t read(int address) {
        if (address >= 0 && address < (int)data.size()) {
            std::cout << "[MOCK] EEPROM.read(" << address << ") = 0x" << std::hex << (int)data[address] << std::endl;
            return data[address];
        }
        return 0xFF;
    }
    
    void write(int address, uint8_t value) {
        if (address >= 0 && address < (int)data.size()) {
            std::cout << "[MOCK] EEPROM.write(" << address << ", 0x" << std::hex << (int)value << ")" << std::endl;
            data[address] = value;
        }
    }
    
    void update(int address, uint8_t value) {
        if (address >= 0 && address < (int)data.size()) {
            if (data[address] != value) {
                std::cout << "[MOCK] EEPROM.update(" << address << ", 0x" << std::hex << (int)value << ")" << std::endl;
                data[address] = value;
            }
        }
    }
    
    uint8_t& operator[](int address) {
        static uint8_t dummy = 0xFF;
        if (address >= 0 && address < (int)data.size()) {
            return data[address];
        }
        return dummy;
    }
    
    void get(int address, void* ptr, size_t size) {
        uint8_t* bytes = (uint8_t*)ptr;
        for (size_t i = 0; i < size; i++) {
            if (address + i < data.size()) {
                bytes[i] = data[address + i];
            }
        }
        std::cout << "[MOCK] EEPROM.get(" << address << ", " << size << " bytes)" << std::endl;
    }
      void put(int address, const void* ptr, size_t size) {
        const uint8_t* bytes = (const uint8_t*)ptr;
        for (size_t i = 0; i < size; i++) {
            if (address + i < data.size()) {
                data[address + i] = bytes[i];
            }
        }
        std::cout << "[MOCK] EEPROM.put(" << address << ", " << size << " bytes)" << std::endl;
    }
    
    // Template methods to match Arduino EEPROM library interface
    template<typename T>
    void get(int address, T& value) {
        get(address, &value, sizeof(T));
    }
    
    template<typename T>
    void put(int address, const T& value) {
        put(address, &value, sizeof(T));
    }

private:
    std::vector<uint8_t> data;
    
    void loadFromFile() {
        std::ifstream file("eeprom_mock.dat", std::ios::binary);
        if (file.good()) {
            file.read((char*)data.data(), data.size());
            std::cout << "[MOCK] EEPROM loaded from file" << std::endl;
        }
    }
    
    void saveToFile() {
        std::ofstream file("eeprom_mock.dat", std::ios::binary);
        if (file.good()) {
            file.write((char*)data.data(), data.size());
            std::cout << "[MOCK] EEPROM saved to file" << std::endl;
        }
    }
};

extern EEPROMClass EEPROM;

#endif // __MOCK_EEPROM_H__
