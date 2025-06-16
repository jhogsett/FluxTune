#ifndef __UTILS_H__
#define __UTILS_H__

#include <Arduino.h>
// #include "bank.h"

// shortcut for using strings stored in program memory
#define FSTR(s) (load_f_string(F(s)))

extern char * load_f_string(const __FlashStringHelper* f_string, char *override_buffer=NULL);
extern void micros_to_ms(char * buffer, unsigned long micros);
extern long time_to_seconds(byte second, byte minute, byte hour);
extern void seconds_to_time(long seconds, byte &second, byte &minute, byte &hour);
extern char *format_long(long num, long basis=0);
extern void random_unique(int count, int max_value, int *result);

#endif
