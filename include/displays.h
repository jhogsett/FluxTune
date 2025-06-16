#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <HT16K33Disp.h>

#define DISPLAY_BRIGHTNESS1 3 // Red needs maximum brightness to match minimum Emerald Green brightness
#define DISPLAY_BRIGHTNESS2 3 // Red needs maximum brightness to match minimum Emerald Green brightness
// #define DISPLAY_BRIGHTNESS3 15

// main 8 digit full display
extern HT16K33Disp display;

// individual left and right red displays
extern HT16K33Disp disp1;
extern HT16K33Disp disp2;
// extern HT16K33Disp disp3;

// array of the individual displays
extern HT16K33Disp displays[];

// used for matching the displays
extern const uint8_t display_brightnesses[];

#endif
