#include "contrast.h"
#include "contrast_handler.h"
#include "saved_data.h"

ContrastHandler::ContrastHandler(Mode * mode) : ModeHandler(mode) 
{
}

// does mode-specific handling of the event to modify the mode
// returns true if event was consumed
bool ContrastHandler::event_sink(int event, int event_data){
    Contrast *contrast = (Contrast*) _mode;

    // unsigned long _old_freq = vfo->_frequency;
    if(event == 1){
        contrast->next_option();
    } else if(event == -1){
        contrast->prev_option();
    }

    save_data();

    return true;
}

bool ContrastHandler::event_sink(bool pressed, bool long_pressed){
    return false;
}

// // periodic timing events for dynamic activities
// void VFO_Tuner::step(unsigned long time){

// }
