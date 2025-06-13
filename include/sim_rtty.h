#ifndef __SIM_RTTY_H__
#define __SIM_RTTY_H__

#include "realization.h"
#include "realizer_pool.h"
#include "async_rtty.h"

#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0

#define MARK_FREQ_SHIFT 170.0
#define SILENT_FREQ 0.1

class SimRTTY : public Realization
{
public:
    SimRTTY(RealizerPool *realizer_pool);
    
    virtual bool begin(unsigned long time, float fixed_freq);
    
    virtual bool update(Mode *mode);

    virtual bool step(unsigned long time);

    void realize();

    float _fixed_freq;
    bool _enabled;
    float _frequency;
    AsyncRTTY _rtty;
    bool _active;

    int _phase;
};

#endif
