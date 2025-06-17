#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef NATIVE_BUILD
#include "../native/platform.h"
#else
#include <HT16K33Disp.h>
#endif

#define DISPLAY_BRIGHTNESS1 3 // Red needs maximum brightness to match minimum Emerald Green brightness
#define DISPLAY_BRIGHTNESS2 3 // Red needs maximum brightness to match minimum Emerald Green brightness

// main 8 digit full display representing both physical displays
extern HT16K33Disp display;

#endif
