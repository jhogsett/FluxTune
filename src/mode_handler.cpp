#include "mode.h"
#include "mode_handler.h"

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

// // periodic timing events for dynamic activities
// void step(unsigned long time){

// }

void ModeHandler::update_display(HT16K33Disp *display){

}
