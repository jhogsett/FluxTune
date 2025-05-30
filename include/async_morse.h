#ifndef __ASYNC_MORSE_H__
#define __ASYNC_MORSE_H__

#define MORSE_TIME_FROM_WPM(w) (1000 / w)

#define STEP_MORSE_TURN_ON   1
#define STEP_MORSE_TURN_OFF  2
#define STEP_MORSE_LEAVE_ON  3
#define STEP_MORSE_LEAVE_OFF 4

extern void start_morse(const char *s, int wpm, bool repeat);
extern int step_morse(unsigned long time);
extern bool morse_active();

#endif
