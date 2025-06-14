#ifndef __SIM_SIGNAL_H__
#define __SIM_SIGNAL_H__

#include "realization.h"
#include "realizer_pool.h"

class SimSignal : public Realization
{
public:
    SimSignal(RealizerPool *realizer_pool);
    
    virtual bool update(Mode *mode);

    // virtual void begin(unsigned long time);
    // virtual bool step(unsigned long time);
    // virtual void end();

    virtual void internal_step(unsigned long time);

    void realize();

    int _phase;
    float _frequency;
};

#endif
