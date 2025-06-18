#ifndef __SIM_RTTY_H__
#define __SIM_RTTY_H__

#include "sim_transmitter.h"
#include "async_rtty.h"

#define MARK_FREQ_SHIFT 170.0

class SimRTTY : public SimTransmitter
{
public:
    SimRTTY(RealizerPool *realizer_pool);
    
    bool begin(unsigned long time, float fixed_freq);
    virtual bool update(class Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    void set_message(const char *message);

private:
    // Keep remaining data members for now (removing _enabled as it's now inherited)
    AsyncRTTY _rtty;
    const char *_message;
    int _phase;
};

#endif
