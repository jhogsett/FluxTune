#include "vfo.h"
#include "vfo_tuner.h"

VFO_Tuner::VFO_Tuner(Mode * mode) : ModeHandler(mode) {
    VFO *vfo = (VFO*) _mode;
}

// does mode-specific handling of the event to modify the mode
// returns true if event was consumed
bool VFO_Tuner::event_sink(int event, int event_data){
    VFO *vfo = (VFO*) _mode;

    unsigned long _old_freq = vfo->_frequency;
    if(event == 1){
        vfo->_frequency += vfo->_step;
        if(_old_freq > vfo->_frequency){
            // unsigned long wrapped up
            vfo->_frequency = (unsigned long)-1L;
        }        
    } else if(event == -1){
        vfo->_frequency -= vfo->_step;
        if(_old_freq < vfo->_frequency){
            // unsigned long wrapped down
            vfo->_frequency = 0;
        }
    }

    return true;
}

// // periodic timing events for dynamic activities
// void VFO_Tuner::step(unsigned long time){

// }

void VFO_Tuner::update_display(HT16K33Disp *display){
    char buffer[20];

    VFO *vfo = (VFO*) _mode;

    if(vfo->_step >= 100){
        // Display as xxxx.yyyy in MHz
        int intpart = vfo->_frequency / 1000000L;
        long decpart = vfo->_frequency - (intpart * 1000000L);
        int decparti = decpart / 100L;

        sprintf(buffer, "%4d.%04d", intpart, decparti);
    } else {
        // Display in Hz
        sprintf(buffer, "%8ld", vfo->_frequency);
    }

    display->show_string(buffer);

}
