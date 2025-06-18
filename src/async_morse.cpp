#include "../native/platform.h"
#include "../include/async_morse.h"

// ========================================
// MORSE CODE LOOKUP TABLE  
// ========================================
// Each byte represents a morse character with bits indicating dot (0) or dash (1)
// Bits are read from LSB to MSB, with unused high bits as padding
const unsigned char morsedata[] PROGMEM = {
    0b10100000, // A  .-
    0b00011000, // B  -...
    0b01011000, // C  -.-.
    0b00110000, // D  -..
    0b01000000, // E  .
    0b01001000, // F  ..-.
    0b01110000, // G  --.
    0b00001000, // H  ....
    0b00100000, // I  ..
    0b11101000, // J  .---
    0b10110000, // K  -.-
    0b00101000, // L  .-..
    0b11100000, // M  --
    0b01100000, // N  -.
    0b11110000, // O  ---
    0b01101000, // P  .--.
    0b10111000, // Q  --.-
    0b01010000, // R  .-.
    0b00010000, // S  ...
    0b11000000, // T  -
    0b10010000, // U  ..-
    0b10001000, // V  ...-
    0b11010000, // W  .--
    0b10011000, // X  -..-
    0b11011000, // Y  -.--
    0b00111000, // Z  --..
    0b11111100, // 0  -----
    0b11110100, // 1  .----
    0b11100100, // 2  ..---
    0b11000100, // 3  ...--
    0b10000100, // 4  ....-
    0b00000100, // 5  .....
    0b00001100, // 6  -....
    0b00011100, // 7  --...
    0b00111100, // 8  ---..
    0b01111100  // 9  ----.
};

// ========================================
// CONSTRUCTOR
// ========================================
AsyncMorse::AsyncMorse() {
    // Initialize all state variables to safe defaults
    // Most initialization happens in start_morse()
}

// ========================================
// CHARACTER LOOKUP HELPER
// ========================================    
char AsyncMorse::lookup_morse_char(char c){
    // Convert character to morse table index
    // Returns: 0-25 for A-Z, 26-35 for 0-9, 0 for invalid characters
    int offset = -1;
    if(c >= '0' && c <= 'z'){
        if(c >= '0' && c <= '9'){
            c -= '0';
            offset = 26;  // Numbers start at index 26
        } else if(c >= 'A' && c <= 'Z'){
            c -= 'A';
            offset = 0;   // Letters start at index 0
        } else if(c >= 'a' && c <= 'z'){
            c -= 'a';     // Convert lowercase to uppercase
            offset = 0;   // Letters start at index 0
        }
    }
    if(offset >= 0)
        return c + offset;
    else
        return 0;  // Invalid character
}

// ========================================
// ELEMENT INITIALIZATION HELPER
// ========================================
// returns true unless the start bit is not found
bool AsyncMorse::start_step_element(unsigned long time){
    async_element_done = false;
    async_morse = pgm_read_byte(morsedata + async_char);
    
    for(async_element = 0; async_element < 7; async_element++){
        async_morse = async_morse >> 1;
        byte bit = async_morse & 0x1;
        if(bit == 1){
            // Set next event to start immediately (time + 0) to eliminate startup delay
            async_next_event = time;
            async_space = true;
            return true;
        }
    }
    return false;
}

void AsyncMorse::restart_morse(){
    async_phase = PHASE_CHAR;
    async_position = 0;
    async_morse = 0;

    async_element = 0;
    async_active = false;
    async_next_event = 0L;
    async_space = false;

    async_element_done = true;

    async_char = async_str[async_position];
    
    if(async_char == ' '){
        async_phase = PHASE_SPACE;

        return;
    }

    async_char = lookup_morse_char(async_char);    

    if(!start_step_element(millis()))
        return;

    if(async_char > 0){
        async_phase = PHASE_CHAR;
    }    
}

void AsyncMorse::start_morse(const char *s, int wpm, bool repeat, int wait_seconds){
    async_str = s;
    async_length = strlen(s);
    async_element_del = MORSE_TIME_FROM_WPM(wpm);
    async_repeat = repeat;
    async_wait_seconds = wait_seconds;

    async_phase = PHASE_CHAR;
    async_position = 0;
    async_morse = 0;    async_element = 0;
    async_active = false;
    async_next_event = 0L;
    async_space = false;
    async_switched_on = false;  // Ensure clean initial state

    async_element_done = true;

    async_char = async_str[async_position];
    
    if(async_char == ' '){
        async_phase = PHASE_SPACE;
        return;
    }

    async_char = lookup_morse_char(async_char);    

    // Initialize with time 0 to eliminate startup delay
    if(!start_step_element(0))
        return;

    if(async_char > 0){
        async_phase = PHASE_CHAR;
    }    
}

unsigned long AsyncMorse::compute_element_time(unsigned long time, byte element_count, bool is_space){
    // Simple timing calculation - both elements and spacing use the same base timing
    // The is_space parameter is kept for potential future timing enhancements
    return time + (element_count * async_element_del);
}

int AsyncMorse::step_element(unsigned long time){
    if(async_phase != PHASE_CHAR)
        return STEP_ELEMENT_DONE;

    if(async_element_done){
        return STEP_ELEMENT_DONE;
    }    if(!is_time_ready(time)){
        return STEP_ELEMENT_EARLY;
    }
    
    if(async_space){
        async_space = false;
        async_element++;

        if(async_element >= 7){
            async_element_done = true;
            return STEP_ELEMENT_DONE;
        }

        async_morse = async_morse >> 1;
        byte bit = async_morse & 0x1;
    
        async_active = true;
        if(bit == 1){
            async_next_event = compute_element_time(time, 3, false);//  time + (3 * async_element_del);
        } else{ 
            async_next_event = compute_element_time(time, 1, false);//time + async_element_del;
        }
    } else {
        async_space = true;
        async_active = false;
        async_next_event = compute_element_time(time, 1, false);//time + async_element_del;
    }

    return STEP_ELEMENT_ACTIVE;
}

// returns false if past the end of the sending string
bool AsyncMorse::step_position(unsigned long time){
    int ret = step_element(time); 

    if(ret == STEP_ELEMENT_EARLY)
        return true;

    if(ret == STEP_ELEMENT_DONE){
        async_position++;
        if(async_position >= async_length)
            return false;
        
        async_char = async_str[async_position];

        if(async_char == ' '){
            async_position++;
            async_char = async_str[async_position];

            if(async_char != ' '){
                async_char = lookup_morse_char(async_char);
                if(!start_step_element(time)){
                    return false;
                }
            }

            async_phase = PHASE_SPACE;
            async_next_event = compute_element_time(time, 7, true);//time + (7 * async_element_del);

            return true;
        }

        async_char = lookup_morse_char(async_char);

        if(!start_step_element(time))
            return false;

        async_next_event = compute_element_time(time, 3, true);//time + (3 * async_element_del);
        async_phase = PHASE_SPACE;
    }
    return true;
}

void AsyncMorse::step_space(unsigned long time){
    if(!is_time_ready(time)){
        return;
    }

    async_phase = PHASE_CHAR;
}

void AsyncMorse::start_wait(unsigned long time){
    async_phase = PHASE_WAIT;
    async_next_event = time + (async_wait_seconds * 1000);
}

void AsyncMorse::step_wait(unsigned long time){
    if(!is_time_ready(time)){
        return;
    }

    if(async_repeat){
        restart_morse();
    } else {
        async_phase = PHASE_DONE;
    }
}

// ========================================
// TRANSMISSION COMPLETION HELPER
// ========================================
void AsyncMorse::handle_transmission_complete(unsigned long time) {
    if(async_repeat) {
        start_wait(time);
    } else {
        async_phase = PHASE_DONE;
    }
}

int AsyncMorse::step_morse(unsigned long time){
    switch(async_phase){
        case PHASE_DONE:
            break;        case PHASE_CHAR:
            if(!step_position(time)){
                handle_transmission_complete(time);
            }
            break;
        case PHASE_SPACE:
            step_space(time);
            break;
        case PHASE_WAIT:
            step_wait(time);
            break;
    }// Generate output signal based on current active state
    if(async_active != async_switched_on) {
        async_switched_on = async_active;
        return async_active ? STEP_MORSE_TURN_ON : STEP_MORSE_TURN_OFF;
    } else {
        return async_active ? STEP_MORSE_LEAVE_ON : STEP_MORSE_LEAVE_OFF;
    }
}

// ========================================
// TIMING HELPER
// ========================================
bool AsyncMorse::is_time_ready(unsigned long current_time) {
    return current_time >= async_next_event;
}


