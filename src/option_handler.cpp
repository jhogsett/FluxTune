
#include "option.h"
#include "option_handler.h"

Option_Handler::Option_Handler(Mode * mode) : ModeHandler(mode) {
    // VFO *vfo = (VFO*) _mode;
}

// does mode-specific handling of the event to modify the mode
// returns true if event was consumed
bool Option_Handler::event_sink(int event, int event_data){
    Option *option = (Option*) _mode;

    if(event == 1){
        option->next_option();
    } else if(event == -1){
        option->prev_option();
    }

    return true;
}

bool Option_Handler::event_sink(bool pressed, bool long_pressed){
    return false;
}

// // periodic timing events for dynamic activities
// void Option_Handler::step(unsigned long time){

// }

// void Option_Handler::update_display(HT16K33Disp *display){
//     Option *option = (Option*) _mode;
//     _mode->update_display(display);
// }

