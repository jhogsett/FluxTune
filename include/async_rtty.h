#ifndef __ASYNC_RTTY_H__
#define __ASYNC_RTTY_H__

#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <Arduino.h>
#endif

#define RTTY_TIME_BASIS 22
#define RTTY_TIME_BASIS2 33

// #define MAX_ELEMENT 6

#define STEP_RTTY_TURN_ON   1
#define STEP_RTTY_TURN_OFF  2
#define STEP_RTTY_LEAVE_ON  3
#define STEP_RTTY_LEAVE_OFF 4

#define STEP_ELEMENT_EARLY 0
#define STEP_ELEMENT_ACTIVE 1
#define STEP_ELEMENT_DONE 2

// Baudot RTTY code definitions
#define BAUDOT_LTRS    0x1F  // Switch to letters mode
#define BAUDOT_FIGS    0x1B  // Switch to figures mode  
#define BAUDOT_SPACE   0x04  // Space character
#define BAUDOT_CR      0x08  // Carriage return
#define BAUDOT_LF      0x02  // Line feed

class AsyncRTTY
{
public:
    AsyncRTTY();

    void start_rtty(bool repeat);
    void start_rtty_message(const char* message, bool repeat); // New method for text transmission
    int step_rtty(unsigned long time);
    
private:
    bool start_step_element(unsigned long time);
    // void restart_rtty();
    unsigned long compute_element_time(unsigned long time, bool stop_bit);
    int step_element(unsigned long time);
    unsigned char get_baudot_code(char c);  // New method to get Baudot code for character

    const char *async_str;
    int async_length;
    int async_str_pos;      // Current position in string
    int async_element_del;
    bool async_repeat;
    byte async_phase;
    byte async_position;
    char async_char;
    byte async_rtty;
    byte async_element;
    bool async_element_done;
    bool async_active;
    unsigned long async_next_event;
    bool async_space;
    bool async_switched_on;

};

// extern void start_rtty(const char *s, int wpm, bool repeat);
// extern int step_rtty(unsigned long time);

#endif
