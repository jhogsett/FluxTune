#include "vfo.h"

VFO::VFO(unsigned long frequency, unsigned long step, int band){
    _frequency = frequency;
    _step = step;
    band = band;
}

// render the current state for display
void VFO::render(char *buffer){
    // Serial.println("A");



    // // sprintf_P(buffer, PSTR("%3.5f"), _frequency);
    // int intpart = int(_frequency);
    // unsigned long decpart = _frequency - intpart;
    // int decparti = int(decpart * 1000.0);

    // // sprintf(buffer, "%3d.%4d", intpart, decparti);
    // char buffer2[20];
    // sprintf(buffer2, "%s", "testing");
    // Serial.println(buffer2);




}
