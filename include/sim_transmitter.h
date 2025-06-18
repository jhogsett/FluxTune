#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);

protected:
    // Common member variables shared by SimStation and SimRTTY
    float _fixed_freq;
    bool _enabled;
    float _frequency;
    bool _active;
};

#endif
