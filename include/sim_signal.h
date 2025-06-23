#ifndef __SIM_SIGNAL_H__
#define __SIM_SIGNAL_H__

#include "realization.h"
#include "wave_gen_pool.h"

class SimSignal : public Realization
{
public:
    SimSignal(WaveGenPool *wave_gen_pool);
    
    virtual bool update(Mode *mode);

    virtual void internal_step(unsigned long time);

    void realize();

    int _phase;
    float _frequency;
};

#endif
