#ifndef __SIM_RTTY_H__
#define __SIM_RTTY_H__

#include "sim_transmitter.h"
#include "async_rtty.h"

#define MARK_FREQ_SHIFT 170.0

class SimRTTY : public SimTransmitter
{
public:
    SimRTTY(RealizerPool *realizer_pool);
    
    virtual bool begin(unsigned long time, float fixed_freq);
    
    virtual bool update(Mode *mode);

    virtual bool step(unsigned long time);    void realize();

    AsyncRTTY _rtty;
    int _phase;
};

#endif
