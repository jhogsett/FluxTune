#include "mode.h"
#include "mode_handler.h"
#include "signal_meter.h"

ModeHandler::ModeHandler(Mode *mode){
    set_mode(mode);    
}

void ModeHandler::set_mode(Mode *mode){
    _mode = mode;    
}

// does mode-specific handling of the event to modify the mode
// returns true if event was consumed
bool ModeHandler::event_sink(int event, int event_data){
    return false;
}

bool ModeHandler::event_sink(bool pressed, bool long_pressed){
    return false;
}

// // periodic timing events for dynamic activities
// void step(unsigned long time){

// }




void ModeHandler::show_title(HT16K33Disp *display){
#ifndef DISABLE_DISPLAY_OPERATIONS
    display->scroll_string(_mode->_title);
#endif
    // update_display(display);
}


void ModeHandler::update_display(HT16K33Disp *display){
    _mode->update_display(display);
}

void ModeHandler::update_signal_meter(SignalMeter *signal_meter){
    _mode->update_signal_meter(signal_meter);
}

void ModeHandler::update_realization(){
    _mode->update_realization();
}
