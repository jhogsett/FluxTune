#include "mode_handler.h"
#include "event_dispatcher.h"

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

void EventDispatcher::set_mode(HT16K33Disp *display, int nhandler){
    set_mode(nhandler);
    _mode_handler->show_title(display);
}

//returns true if the event was consumed
// some events are meta-events, for example to change modes
bool EventDispatcher::dispatch_event(HT16K33Disp *display, int encoder_id, int event, int event_data){
    /// HARDCODED ///
    // encoder id 0 is the tuning knob
    // encoder id 1 is the modes knob
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
                // _mode_handler->show_title(display);
            } else if(event == -1){
                int handler = _ncurrent_handler - 1;
                if(handler < 0)
                    handler = _nhandlers - 1; 
                set_mode(display, handler);
                // _mode_handler->show_title(display);
            }
            break;
        }
    }
}

void EventDispatcher::update_display(HT16K33Disp *display){
    _mode_handler->update_display(display);
}


// need an array of mode handlers that can be cycled through