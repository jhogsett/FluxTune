#ifndef __TIMER_H__
#define __TIMER_H__

#include <Arduino.h>

// current timer value as set by the user
extern uint8_t timer_hour, timer_minute, timer_second;

extern void render_timer_string(uint8_t seconds, uint8_t minutes, uint8_t hours, bool running, bool going_up);
extern void increment_timer(uint8_t &second, uint8_t &minute, uint8_t &hour, uint8_t seconds=1, uint8_t minutes=0, uint8_t hours=0);
extern bool decrement_timer(uint8_t &second, uint8_t &minute, uint8_t &hour, int seconds=1, int minutes=0, int hours=0);
extern bool timer_prompt(uint8_t seconds, uint8_t minutes, uint8_t hours);

#endif
