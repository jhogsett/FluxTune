#include "../native/platform.h"
#include "../include/async_rtty.h"
#include "../include/station_config.h"

#ifndef RTTY_RANDOM_BITS_ONLY
// Baudot code lookup table (5-bit codes) - stored in PROGMEM to save RAM
// Index corresponds to ASCII character, value is 5-bit Baudot code
// 0xFF means character not supported in Baudot
// This table can be disabled to save ~128 bytes Flash by defining RTTY_RANDOM_BITS_ONLY
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
    0xFF,                                           // 96 (`
    0x03, 0x19, 0x0E, 0x09, 0x01, 0x0D, 0x1A,     // 97-103 (a-g) same as uppercase
    0x14, 0x06, 0x0B, 0x0F, 0x12, 0x1C, 0x18,     // 104-110 (h-n)    0x16, 0x17, 0x0A, 0x05, 0x10, 0x07, 0x1E,     // 111-117 (o-u)
    0x1D, 0x13, 0x1B, 0x11, 0x15,                 // 118-122 (v-z)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF                   // 123-127 (punctuation)
};
#endif // RTTY_RANDOM_BITS_ONLY

AsyncRTTY::AsyncRTTY()
{
    // Initialize all state variables to safe defaults
    async_active = false;
    async_switched_on = false;
    async_element_done = true;
    async_element = 0;
    async_next_event = 0;
    async_str = NULL;
    async_length = 0;
    async_str_pos = 0;
}

unsigned char AsyncRTTY::get_baudot_code(char c) {
    // Get Baudot code for ASCII character
#ifdef RTTY_RANDOM_BITS_ONLY
    // Memory optimization: return random 5-bit value instead of real Baudot
    // This saves ~128 bytes Flash but RTTY will sound authentic
    #ifdef PLATFORM_NATIVE
        return rand() & 0x1F;  // Random 5-bit value (0-31)
    #else
        return random(32);     // Random 5-bit value (0-31)
    #endif
#else
    // Use real Baudot encoding
    if (c >= 0 && c < 128) {
        return pgm_read_byte(baudot_letters + c);
    }
    return 0xFF; // Unsupported character
#endif
}

bool AsyncRTTY::start_step_element(unsigned long time){
    async_element_done = false;
    async_element = 0;
    return false;
}

void AsyncRTTY::start_rtty_message(const char* message, bool repeat) {
    async_repeat = repeat;
    async_active = false;
    async_switched_on = false;
    async_next_event = 0L;
    async_element_done = true;
    async_str = message;
    async_length = strlen(message);
    async_str_pos = 0;

    if(!start_step_element(0))  // Use 0 instead of millis() - timing will be set on first step_rtty call
        return;
}


unsigned long AsyncRTTY::compute_element_time(unsigned long time, bool stop_bit){
    return time + (stop_bit ? RTTY_TIME_BASIS2 : RTTY_TIME_BASIS);
}

int AsyncRTTY::step_element(unsigned long time){
    if(async_element_done){
        return STEP_ELEMENT_DONE;
    }
    
    if(time < async_next_event){
        return STEP_ELEMENT_EARLY;
    }

    switch(async_element){
        case 0:
            // start bit SPACE
            async_active = false;
            async_next_event = compute_element_time(time, false);
            async_element++;
            break;        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            // Generate data bits from Baudot message
            if (async_str != NULL && async_str_pos < async_length) {
                // Get current character and its Baudot code
                char current_char = async_str[async_str_pos];
                unsigned char baudot_code = get_baudot_code(current_char);
                
                if (baudot_code != 0xFF) {
                    // Extract the specific bit for this element (LSB first)
                    int bit_index = async_element - 1; // element 1-5 -> bit 0-4
                    async_active = (baudot_code >> bit_index) & 1;
                } else {
                    // Unsupported character, use space
                    async_active = (BAUDOT_SPACE >> (async_element - 1)) & 1;
                }
            } else {
                // End of message or no message - transmit idle (all marks/ones)
                async_active = true;
            }
            
            async_next_event = compute_element_time(time, false);
            async_element++;
            break;

        case 6:
            async_active = true;
            async_next_event = compute_element_time(time, true);
            async_element = 0;
            
            // Move to next character after stop bit
            if (async_str != NULL && async_str_pos < async_length) {
                async_str_pos++;
                // If we've reached the end and repeat is enabled, restart
                if (async_str_pos >= async_length && async_repeat) {
                    async_str_pos = 0;
                }
            }
            break;
    }

    return STEP_ELEMENT_ACTIVE;
}

int AsyncRTTY::step_rtty(unsigned long time){
    step_element(time);

    // Generate output signal based on current active state
    if(async_active != async_switched_on) {
        async_switched_on = async_active;
        return async_active ? STEP_RTTY_TURN_ON : STEP_RTTY_TURN_OFF;
    } else {
        return async_active ? STEP_RTTY_LEAVE_ON : STEP_RTTY_LEAVE_OFF;
    }
}

bool AsyncRTTY::is_message_complete() const {
    // Message is complete if we've reached the end and not repeating
    return (async_str == NULL || (async_str_pos >= async_length && !async_repeat));
}


