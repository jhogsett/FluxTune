#ifndef __ASYNC_MORSE_H__
#define __ASYNC_MORSE_H__

#define MORSE_TIME_FROM_WPM(w) (1000 / w)

#define PHASE_DONE 0
#define PHASE_CHAR 1
#define PHASE_SPACE 2
#define PHASE_WAIT 3

#define MAX_ELEMENT 6

#define STEP_MORSE_TURN_ON   1
#define STEP_MORSE_TURN_OFF  2
#define STEP_MORSE_LEAVE_ON  3
#define STEP_MORSE_LEAVE_OFF 4

#define STEP_ELEMENT_EARLY 0
#define STEP_ELEMENT_ACTIVE 1
#define STEP_ELEMENT_DONE 2

class AsyncMorse
{
public:
    AsyncMorse();

    void start_morse(const char *s, int wpm, bool repeat, int wait_seconds);
    int step_morse(unsigned long time);
    
private:
    char lookup_morse_char(char c);
    bool start_step_element(unsigned long time);
    void restart_morse();
    unsigned long compute_element_time(unsigned long time, uint8_t element_count, bool is_space);
    int step_element(unsigned long time);
    bool step_position(unsigned long time);
    void step_space(unsigned long time);
    void start_wait(unsigned long time);
    void step_wait(unsigned long time);

    const char *async_str = NULL;
    int async_length;
    int async_element_del;
    bool async_repeat;
    uint8_t async_phase;
    uint8_t async_position;
    char async_char;
    uint8_t async_morse;
    uint8_t async_element;
    bool async_element_done;
    bool async_active;
    unsigned long async_next_event;
    bool async_space;
    bool async_switched_on;
    int async_wait_seconds;

};

extern void start_morse(const char *s, int wpm, bool repeat);
extern int step_morse(unsigned long time);

#endif
