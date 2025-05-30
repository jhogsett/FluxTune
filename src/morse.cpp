#include <Arduino.h>
#include "saved_data.h"
// #include "speaker.h"
#include "morse.h"

const unsigned char morsedata[] PROGMEM = {
    0b10100000, // A
    0b00011000, // B
    0b01011000, // C
    0b00110000, // D
    0b01000000, // E
    0b01001000, // F
    0b01110000, // G
    0b00001000, // H
    0b00100000, // I
    0b11101000, // J
    0b10110000, // K
    0b00101000, // L
    0b11100000, // M
    0b01100000, // N
    0b11110000, // O
    0b01101000, // P
    0b10111000, // Q
    0b01010000, // R
    0b00010000, // S
    0b11000000, // T
    0b10010000, // U
    0b10001000, // V
    0b11010000, // W
    0b10011000, // X
    0b11011000, // Y
    0b00111000, // Z
    0b11111100, // 0
    0b11110100, // 1
    0b11100100, // 2
    0b11000100, // 3
    0b10000100, // 4
    0b00000100, // 5
    0b00001100, // 6
    0b00011100, // 7
    0b00111100, // 8
    0b01111100  // 9
    };


void _send_dot(int time){
    // beep(DEFAULT_MORSE_FREQ, time * DOT_FACTOR);
    delay(time * ELEMENT_SPACE_FACTOR);
}

void _send_dash(int time){
    // beep(DEFAULT_MORSE_FREQ, time * DASH_FACTOR);
    delay(time * ELEMENT_SPACE_FACTOR);
}

void _send_char_space(int time){
    delay(time * CHAR_SPACE_FACTOR);
}

void _send_word_space(int time){
    delay(time * WORD_SPACE_FACTOR);
}

// need async_ morse and start bit
// maybe auto skip to start bit

void _send_morse(int c, int time){
    byte morse = pgm_read_byte(morsedata + c);
    bool start_bit = false;
    for(int i = 0; i < 7; i++){
        morse = morse >> 1;
        byte bit = morse & 0x1;

        if(!start_bit){
            if(bit == 1){
                start_bit = true;
            }
            continue;
        }
        if(bit == 1){
            _send_dash(time);
        } else {
            _send_dot(time);
        }
    }
}

char _lookup_morse_char(char c){
    int offset = -1;
    if(c >= '0' && c <= 'z'){
        if(c >= '0' && c <= '9'){
            c -= '0';
            offset = 26;
        } else if(c >= 'A' && c <= 'Z'){
            c -= 'A';
            offset = 0;
        } else if(c >= 'a' && c <= 'z'){
            c -= 'a';
            offset = 0;
        }
    }
    if(offset >= 0)
        return c + offset;
    else
        return 0;
}

void _send_morse_char(char c, int time){
    if(c == ' '){
        _send_word_space(time);
        return;
    }

    c = _lookup_morse_char(c);
    if(c > 0)
        _send_morse(c, time);
}

void send_morse(char c, int wpm){
    // if(wpm == 0)
    //     wpm = option_wpm;

    _send_morse_char(c, MORSE_TIME_FROM_WPM(wpm));
}

void send_morse(const char *s, int wpm){
    // if(wpm == 0)
    //     wpm = option_wpm;

    int time = MORSE_TIME_FROM_WPM(wpm);
    int l = strlen(s);
    for(int i = 0; i < l; i++){
        _send_morse_char(s[i], time);
        _send_char_space(time);
    }
}

const char *async_str = NULL;
int async_length;
int async_element_del;
byte async_phase;
byte async_position;
char async_char;
byte async_morse;
byte async_element;
bool async_element_done;
bool async_active;
unsigned long async_next_event;
bool async_space;

#define PHASE_DONE 0
#define PHASE_CHAR 1
#define PHASE_SPACE 2
#define PHASE_WORD_SPACE 3

#define MAX_ELEMENT 6

bool morse_active(){
    return async_active;
}

// returns true unless the start bit is not found
bool start_step_element(unsigned long time){
    async_element_done = false;
    async_morse = pgm_read_byte(morsedata + async_char);
    
    for(async_element = 0; async_element < 7; async_element++){
        async_morse = async_morse >> 1;
        byte bit = async_morse & 0x1;
        if(bit == 1){
            async_next_event = time;
            async_space = true;
            return true;
        }
    }
    return false;
}

void start_morse(const char *s, int wpm){
    async_str = s;
    async_length = strlen(s);
    async_element_del = MORSE_TIME_FROM_WPM(wpm);

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

    async_char = _lookup_morse_char(async_char);    

    if(!start_step_element(millis()))
        return;

    if(async_char > 0){
        async_phase = PHASE_CHAR;
    }    
}    

#define STEP_ELEMENT_EARLY 0
#define STEP_ELEMENT_ACTIVE 1
#define STEP_ELEMENT_DONE 2

int step_element(unsigned long time){
    if(async_phase != PHASE_CHAR)
        return STEP_ELEMENT_DONE;

    if(async_element_done){
        return STEP_ELEMENT_DONE;
    }

    if(time < async_next_event){
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
            async_next_event = time + (3 * async_element_del);
        } else{ 
            async_next_event = time + async_element_del;
        }
    } else {
        async_space = true;
        async_active = false;
        async_next_event = time + async_element_del;
    }

    return STEP_ELEMENT_ACTIVE;
}



// returns false if past the end of the sending string
bool step_position(unsigned long time){
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
                async_char = _lookup_morse_char(async_char);
                if(!start_step_element(time)){
                    return false;
                }
            }

            async_phase = PHASE_SPACE;
            async_next_event = time + (7 * async_element_del);

            return true;
        }

        async_char = _lookup_morse_char(async_char);

        if(!start_step_element(time))
            return false;

        async_next_event = time + (3 * async_element_del);
        async_phase = PHASE_SPACE;
    }
    return true;
}

bool step_space(unsigned long time){
    if(time < async_next_event){
        return true;
    }

    async_phase = PHASE_CHAR;

    return true;
}

bool async_switched_on = false;
int step_morse(unsigned long time){
    switch(async_phase){
        case PHASE_DONE:
            break;
        case PHASE_CHAR:
            if(!step_position(time)){
                async_phase = PHASE_DONE;
            }
            break;
        case PHASE_SPACE:
            if(!step_space(time))
                async_phase = PHASE_DONE;
            break;
        case PHASE_WORD_SPACE:
            break;
    }

    if(async_switched_on){
        if(async_active){
            return STEP_MORSE_LEAVE_ON;
        } else {
            async_switched_on = false;
            return STEP_MORSE_TURN_OFF;
        } 
    } else { // switched off
        if(!async_active){
            return STEP_MORSE_LEAVE_OFF;
        } else {
            async_switched_on = true;
            return STEP_MORSE_TURN_ON;
        }
    }

    // return async_active;
}
