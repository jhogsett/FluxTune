#include <MD_AD9833.h>
#include "wavegen.h"
#include "vfo.h"

VFO::VFO(const char *title, unsigned long frequency, unsigned long step, int band) : Mode(title)
{
    _frequency = frequency;
    _step = step;
    band = band;
}

void VFO::update_display(HT16K33Disp *display){
    char buffer[17];
    if(_step >= 100){
        // Display as xxxx.yyyy in MHz
        int intpart = _frequency / 1000000L;
        long decpart = _frequency - (intpart * 1000000L);
        int decparti = decpart / 100L;
        
        sprintf(buffer, "%4d.%04d", intpart, decparti);
    } else {
        // Display in Hz
        sprintf(buffer, "%8ld", _frequency);
    }
    
    display->show_string(buffer);
}

void VFO::update_realization(Realization *realization){
    WaveGen *wavegen = (WaveGen *)realization;
	wavegen->_sig_gen->setFrequency((MD_AD9833::channel_t)0, float(_frequency));
}
