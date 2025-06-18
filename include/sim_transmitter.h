#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

// Common constants for simulated transmitters
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);

protected:
    // Common member variables shared by SimStation and SimRTTY
    float _fixed_freq;
    bool _enabled;
    float _audible_frequency;  // The virtually tuned frequency heard by the user
    bool _active;
};

#endif
