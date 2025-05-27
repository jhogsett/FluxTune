#ifndef __CONTRAST_HANDLER_H__
#define __CONTRAST_HANDLER_H__

#include <HT16K33Disp.h>
#include "mode.h"
#include "mode_handler.h"

class ContrastHandler : public ModeHandler
{
public:
    // constructor
    ContrastHandler(Mode * mode);

    virtual bool event_sink(int event, int count);
    virtual bool event_sink(bool pressed, bool long_pressed);

    // virtual void update_display(HT16K33Disp *display);

    // virtual void step(unsigned long time);


    // void frequency_up(unsigned long steps);

    // void frequency_down(unsigned long steps);

    // void band_up(unsigned long steps);

    // void band_down(unsigned long steps);

    // void step_up(unsigned long steps);

    // void step_down(unsigned long steps);

private:
};

#endif // __CONTRAST_HANDLER_H__
