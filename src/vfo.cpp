#include "vfo.h"

VFO::VFO(const char *title, unsigned long frequency, unsigned long step, int band) : Mode(title)
{
    _frequency = frequency;
    _step = step;
    band = band;
}

// // render the current state for display
// void VFO::render(char *buffer){

// }
