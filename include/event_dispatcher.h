#ifndef __EVENT_DISPATCHER_H__
#define __EVENT_DISPATCHER_H__

#include "mode_handler.h"

// input: encoder events
// has: current mode handler
// output: issues events to the current mode handler

class EventDispatcher
{
public:
    EventDispatcher(ModeHandler *mode_handler);

    void set_mode(ModeHandler *mode_handler);

    bool dispatch_event(int event, int event_data);

    void update_display(HT16K33Disp *display);


private:
    ModeHandler * _mode_handler;

};

#endif // __EVENT_DISPATCHER_H__
