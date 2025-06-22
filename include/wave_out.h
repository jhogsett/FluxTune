#ifndef __WAVE_OUT_H__
#define __WAVE_OUT_H__

#include "realization.h"
#include "realizer_pool.h"

class WaveOut : public Realization
{
public:
    WaveOut(WaveGenPool *waveGenPool);
    
    virtual bool update(Mode *mode);

    // virtual bool begin(unsigned long time);
    // virtual bool step(unsigned long time);
    // virtual void end();

    // virtual void internal_step(unsigned long time);
};

#endif
