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
// bool async_element_space;
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
    // Serial.println("start step element");
    // Serial.println("async_char should be low int:");
    // Serial.println(int(async_char));

    async_morse = pgm_read_byte(morsedata + async_char);
    // Serial.println(async_morse);
    
    for(async_element = 0; async_element < 7; async_element++){
        // Serial.println(async_element);
        async_morse = async_morse >> 1;
        byte bit = async_morse & 0x1;
        // Serial.println(bit);
        if(bit == 1){
            async_next_event = time;
            // async_element = 0;
            async_space = true;
            async_element_done = false;
            return true;
        }
    }
    // Serial.println("false");
    return false;
}

void start_morse(const char *s, int wpm){
    // Serial.println("start morse");
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
    // Serial.println("------------------------------------------------");
    // Serial.println(async_char);
    // Serial.println("------------------------------------------------");
    
    if(async_char == ' '){
        async_phase = PHASE_SPACE;

        // Serial.println(async_position);
        // async_position++;
        // Serial.println(async_position);
        // async_char = async_str[async_position];
        // async_char = _lookup_morse_char(async_char);    
        return;
    }

    async_char = _lookup_morse_char(async_char);    

    if(!start_step_element(millis()))
        return;// false;


    if(async_char > 0){
        // _send_morse(c, time);
        // start char
        async_phase = PHASE_CHAR;
    }    
}    

void async_debug(){
    char buffer[80];
    sprintf(buffer, "p:%d e:%d a:%d s:%d", async_position, async_element, async_active, async_space);
    // Serial.println(buffer);

}

#define STEP_ELEMENT_EARLY 0
#define STEP_ELEMENT_ACTIVE 1
#define STEP_ELEMENT_DONE 2

// returns false after the character is done sending
// early, active, done
int step_element(unsigned long time){
    // Serial.println("1111111111");
    if(async_element_done){
        // Serial.println("step element - element done");
        return STEP_ELEMENT_DONE;
    }

    // Serial.println("2222222");

    // // Serial.println("step element");
    if(time < async_next_event){
        // // Serial.println("step element - early");
        return STEP_ELEMENT_EARLY;
    }

    // Serial.println("333333333");
    
    // async_debug();
    
    if(async_space){
        // Serial.println("444444444");
        // Serial.println("step element - was handling element space");
        
        async_space = false;
        async_element++;

        if(async_element >= 7){
            // Serial.println("step element - end of element");
            async_element_done = true;
            return STEP_ELEMENT_DONE;
        }

        // Serial.println("55555555");

        async_morse = async_morse >> 1;
        byte bit = async_morse & 0x1;
    
        async_active = true;
        if(bit == 1){
            // Serial.println("-");
            async_next_event = time + (3 * async_element_del);
        } else{ 
            // Serial.println(".");
            async_next_event = time + async_element_del;
        }
    } else {
        // Serial.println("step element - was element");
        // was handling dot/dash, switch to element space
        // Serial.println("_");

        // Serial.println("66666666");

        async_space = true;
        async_active = false;
        async_next_event = time + async_element_del;
    }

    // Serial.println("777777777");

    return STEP_ELEMENT_ACTIVE;
}



// returns false if past the end of the sending string
bool step_position(unsigned long time){
    int ret = step_element(time); 

    if(ret == STEP_ELEMENT_EARLY)
        return true;

    // // Serial.println("step position");
    if(ret == STEP_ELEMENT_DONE){
        // Serial.println("step position 1");
        
        async_position++;
        if(async_position >= async_length)
            return false;
        
        async_element = 0;
        async_element_done = false;

        async_char = async_str[async_position];
        // Serial.println("------------------------------------------------");
        // Serial.println(async_char);
        // Serial.println("------------------------------------------------");

        if(async_char == ' '){
            
            async_position++;
            if(async_position >= async_length)
            return false;
            
            async_element = 0;
            async_element_done = false;
            
            async_char = async_str[async_position];
            // Serial.println("------------------------------------------------");
            // Serial.println(async_char);
            // Serial.println("------------------------------------------------");
            
            async_char = _lookup_morse_char(async_char);
            
            if(!start_step_element(time)){
                return false;
            }

            async_phase = PHASE_SPACE;
            async_next_event = time + (7 * async_element_del);

            return true;
        }

        // if(async_char == ' '){
        //     Serial.println("FOUND SPACE");
        //     // _send_word_space(time);
        //     // start word space
        //     async_phase = PHASE_SPACE;
        //     return true;
        // }

        async_char = _lookup_morse_char(async_char);
        // Serial.println("looked up:");
        // Serial.println(int(async_char));


        // Serial.println("looked up char:");
        // Serial.println(int(async_char));

        if(!start_step_element(time))
            return false;

        // if(async_char > 0){
        //     // _send_morse(c, time);
        //     // start char
        //     async_phase = PHASE_CHAR;
        // }
        async_next_event = time + (3 * async_element_del);
        async_phase = PHASE_SPACE;
        // Serial.println("CHAR SPACE");
    }
    return true;
}

void step_space(unsigned long time){
    if(time < async_next_event){
        return;
    }
    // Serial.println("SPACE3");

    async_phase = PHASE_CHAR;
}

bool step_morse(unsigned long time){
    // // Serial.println("step morse");
    switch(async_phase){
        case PHASE_DONE:
            break;
        case PHASE_CHAR:
            if(!step_position(time))
                async_phase = PHASE_DONE;
            break;
        case PHASE_SPACE:
            // Serial.println("SPACE2");
            step_space(time);
            break;
        case PHASE_WORD_SPACE:
            break;
    }
    return async_active;
}






/////// needs to handle character spaces