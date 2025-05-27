#ifndef __WAVE_OUT_H__
#define __WAVE_OUT_H__

#include "realization.h"

class WaveOut : public Realization
{
public:
    WaveOut(Realizer *realizer);
    
    virtual bool update(Mode *mode);

    // virtual bool begin(unsigned long time);
    // virtual bool step(unsigned long time);
    // virtual void end();

    // virtual void internal_step(unsigned long time);
};

#endif
