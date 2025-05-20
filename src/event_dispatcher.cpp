#include "mode_handler.h"
#include "event_dispatcher.h"

EventDispatcher::EventDispatcher(ModeHandler *mode_handler){
    set_mode(mode_handler);
}

void EventDispatcher::set_mode(ModeHandler *mode_handler){
    _mode_handler = mode_handler;
}

//returns true if the event was consumed
bool EventDispatcher::dispatch_event(int event, int event_data){
    return _mode_handler->event_sink(event, event_data);
}

void EventDispatcher::update_display(HT16K33Disp *display){
    _mode_handler->update_display(display);
}
