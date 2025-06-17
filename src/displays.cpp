#ifdef PLATFORM_NATIVE
#include "../native/platform.h"
#else
#include <HT16K33Disp.h>
#endif
#include "displays.h"

// Main display object representing both physical displays (8 chars total)
HT16K33Disp display(0x70, 2);
