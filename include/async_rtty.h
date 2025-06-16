#ifndef __ASYNC_RTTY_H__
#define __ASYNC_RTTY_H__

#include <Arduino.h>

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

class AsyncRTTY
{
public:
    AsyncRTTY();

    void start_rtty(bool repeat);
    int step_rtty(unsigned long time);
    
private:
    bool start_step_element(unsigned long time);
    // void restart_rtty();
    unsigned long compute_element_time(unsigned long time, bool stop_bit);
    int step_element(unsigned long time);

    const char *async_str = NULL;
    int async_length;
    int async_element_del;
    bool async_repeat;
    uint8_t async_phase;
    uint8_t async_position;
    char async_char;
    uint8_t async_rtty;
    uint8_t async_element;
    bool async_element_done;
    bool async_active;
    unsigned long async_next_event;
    bool async_space;
    bool async_switched_on;

};

// extern void start_rtty(const char *s, int wpm, bool repeat);
// extern int step_rtty(unsigned long time);

#endif
