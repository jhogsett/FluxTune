#ifndef __VFO_TUNER_H__
#define __VFO_TUNER_H__

#include <HT16K33Disp.h>
#include "mode.h"
#include "mode_handler.h"

class VFO_Tuner : public ModeHandler
{
public:
    // constructor
    VFO_Tuner(Mode * mode);

    virtual bool event_sink(int event, int count);

    // virtual void step(unsigned long time);

    virtual void update_display(HT16K33Disp *display);


    void frequency_up(float steps);

    void frequency_down(float steps);

    void band_up(float steps);

    void band_down(float steps);

    void step_up(float steps);

    void step_down(float steps);

private:
};

#endif // __VFO_TUNER_H__
