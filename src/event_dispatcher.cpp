#include "mode_handler.h"
#include "event_dispatcher.h"
#include "signal_meter.h"

// change to accept an array of mode handlers
EventDispatcher::EventDispatcher(ModeHandler **mode_handlers, int nhandlers){
    _mode_handlers = mode_handlers;
    _nhandlers = nhandlers;
    _mode_handler = NULL;
    _ncurrent_handler = 0;
    set_mode(0);
}

void EventDispatcher::set_mode(int nhandler){
    _ncurrent_handler = nhandler;
    _mode_handler = _mode_handlers[_ncurrent_handler];
}

void EventDispatcher::set_mode(HT16K33Disp *display, int nhandler){    set_mode(nhandler);

#ifndef DISABLE_DISPLAY_OPERATIONS
    _mode_handler->show_title(display);
    update_display(display);
#endif

    update_realization();
}

//returns true if the event was consumed
// some events are meta-events, for example to change modes
bool EventDispatcher::dispatch_event(HT16K33Disp *display, int encoder_id, int event, int event_data){
    switch(encoder_id){
        case ID_ENCODER_TUNING:
            return _mode_handler->event_sink(event, event_data);

        case ID_ENCODER_MODES:
        {
            if(event == 1){
                int handler = _ncurrent_handler + 1;
                if(handler >= _nhandlers)
                handler = 0; 
                set_mode(display, handler);
                return true;
            } else if(event == -1){
                int handler = _ncurrent_handler - 1;
                if(handler < 0)
                handler = _nhandlers - 1; 
                set_mode(display, handler);
                return true;
            }
            break;
        }
    }
    return false;
}

bool EventDispatcher::dispatch_event(HT16K33Disp *display, int encoder_id, bool press, bool long_press){
    switch(encoder_id){
        case ID_ENCODER_TUNING:
            return _mode_handler->event_sink(press, long_press);

        case ID_ENCODER_MODES:
            return _mode_handler->event_sink(press, long_press);
    }
    return false;
}


void EventDispatcher::update_display(HT16K33Disp *display){
    _mode_handler->update_display(display);
}

void EventDispatcher::update_signal_meter(SignalMeter *signal_meter){
    _mode_handler->update_signal_meter(signal_meter);
}

void EventDispatcher::update_realization(){
    _mode_handler->update_realization();
}

Mode* EventDispatcher::get_current_mode(){
    return _mode_handler->get_mode();
}
