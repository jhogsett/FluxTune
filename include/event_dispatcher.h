#ifndef __EVENT_DISPATCHER_H__
#define __EVENT_DISPATCHER_H__

#include "mode_handler.h"

#define ID_ENCODER_TUNING 0
#define ID_ENCODER_MODES 1

// input: encoder events
// has: current mode handler
// output: issues events to the current mode handler

class EventDispatcher
{
public:
    EventDispatcher(ModeHandler **mode_handlers, int nhandlers);

    void set_mode(int nhandler);
    void set_mode(HT16K33Disp *display, int nhandler);
     
    bool dispatch_event(HT16K33Disp *display, int encoder_id, int event, int event_data);
    bool dispatch_event(HT16K33Disp *display, int encoder_id, bool press, bool long_press);

    void update_display(HT16K33Disp *display);


private:
    ModeHandler * _mode_handler;
    ModeHandler ** _mode_handlers;
    int _nhandlers;
    int _ncurrent_handler;
};

#endif // __EVENT_DISPATCHER_H__
