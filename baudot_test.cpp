// Baudot RTTY Enhancement for AsyncRTTY
// This adds real Baudot code transmission capability

#include "../native/platform.h"
#include "../include/async_rtty.h"

// JH! is this file needed?

// Baudot code lookup table (5-bit codes) - stored in PROGMEM to save RAM
// Index corresponds to ASCII character, value is 5-bit Baudot code
// 0xFF means character not supported in Baudot
const unsigned char baudot_letters[] PROGMEM = {
    // ASCII 0-31 (control characters) - most not supported
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 0-7
    0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0x08, 0xFF, 0xFF, // 8-15  (LF=0x02, CR=0x08)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 16-23
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 24-31
    
    // ASCII 32-63 (space, punctuation, numbers)
    0x04, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 32-39  (space=0x04)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 40-47
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 48-55  (numbers in figures mode)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 56-63
    
    // ASCII 64-95 (@ and uppercase letters)
    0xFF,                                           // 64 (@)
    0x03, 0x19, 0x0E, 0x09, 0x01, 0x0D, 0x1A,     // 65-71 (A-G)
    0x14, 0x06, 0x0B, 0x0F, 0x12, 0x1C, 0x18,     // 72-78 (H-N)  
    0x16, 0x17, 0x0A, 0x05, 0x10, 0x07, 0x1E,     // 79-85 (O-U)
    0x1D, 0x13, 0x1B, 0x11, 0x15,                 // 86-90 (V-Z)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF,                  // 91-95 (punctuation)
    
    // ASCII 96-127 (` and lowercase letters)
    0xFF,                                           // 96 (`)
    0x03, 0x19, 0x0E, 0x09, 0x01, 0x0D, 0x1A,     // 97-103 (a-g) same as uppercase
    0x14, 0x06, 0x0B, 0x0F, 0x12, 0x1C, 0x18,     // 104-110 (h-n)
    0x16, 0x17, 0x0A, 0x05, 0x10, 0x07, 0x1E,     // 111-117 (o-u)
    0x1D, 0x13, 0x1B, 0x11, 0x15,                 // 118-122 (v-z)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF                   // 123-127 (punctuation)
};

// Special Baudot codes
#define BAUDOT_LTRS    0x1F  // Switch to letters mode
#define BAUDOT_FIGS    0x1B  // Switch to figures mode  
#define BAUDOT_SPACE   0x04  // Space character
#define BAUDOT_CR      0x08  // Carriage return
#define BAUDOT_LF      0x02  // Line feed

// Test function to demonstrate Baudot encoding
void test_baudot_encoding() {
    const char* test_message = "HELLO WORLD\r\n";
    
    Serial.print("Encoding message: ");
    Serial.println(test_message);
    
    for (int i = 0; test_message[i] != '\0'; i++) {
        char c = test_message[i];
        if (c < 128) {
            unsigned char code = pgm_read_byte(baudot_letters + c);
            if (code != 0xFF) {
                Serial.print("'");
                Serial.print(c);
                Serial.print("' -> 0x");
                Serial.print(code, HEX);
                Serial.print(" (");
                for (int bit = 4; bit >= 0; bit--) {
                    Serial.print((code >> bit) & 1);
                }
                Serial.println(")");
            } else {
                Serial.print("'");
                Serial.print(c);
                Serial.println("' -> NOT SUPPORTED");
            }
        }
    }
}

// JH! 
