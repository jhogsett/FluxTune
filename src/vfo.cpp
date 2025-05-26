#include <MD_AD9833.h>
#include "wavegen.h"
#include "vfo.h"

VFO::VFO(const char *title, unsigned long frequency, unsigned long step, int band) : Mode(title)
{
    _frequency = frequency;
    _step = step;
    band = band;
}

// step needs to be in 0.1Hz units
// when step is 0.1Hz, use xxxxxxx.y format
void VFO::update_display(HT16K33Disp *display){
    char buffer[20];
    if(_frequency >= 10000000L){
        // Display as 2450.0000 in MHz
        int megpart = _frequency / 1000000L;
        long decpart = _frequency - (megpart * 1000000L);
        int decparti = decpart / 100L;
        
        sprintf(buffer, "%4d.%04d", megpart, decparti);

    } else if(_frequency >= 1000000L) {
        // Display 7,015,089 as 7015.089 in KHz
        int megpart = _frequency / 1000000L;
        long remainder = _frequency - (megpart * 1000000L);
        int kilpart = remainder / 1000L;
        remainder = remainder - (kilpart * 1000L);
        int unipart = remainder;

        sprintf(buffer, " %1d.%03d.%03d", megpart, kilpart, unipart);

    } else {
        // Display in Hz
        sprintf(buffer, "%8ld", _frequency);
    }



    // if(_step >= 1000){
    // } else if(_step >= 100){
    //     // Display as xxxx.yyyy in MHz
    //     int intpart = _frequency / 1000000L;
    //     long decpart = _frequency - (intpart * 1000000L);
    //     int decparti = decpart / 100L;
        
    //     sprintf(buffer, "%4d.%04d", intpart, decparti);
    // } else {
    //     // Display in Hz
    //     sprintf(buffer, "%8ld", _frequency);
    // }
    
    display->show_string(buffer);
}

void VFO::update_realization(Realization *realization){
    WaveGen *wavegen = (WaveGen *)realization;
	wavegen->_sig_gen->setFrequency((MD_AD9833::channel_t)0, float(_frequency));
}
