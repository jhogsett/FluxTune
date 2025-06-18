#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

// Common constants for simulated transmitters
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

/**
 * Empty base class for simulated transmitters.
 * This is the most minimal starting point for refactoring.
 */
class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);

protected:
    // Completely empty base class - derived classes handle everything
    
    // Common member variables
    float _fixed_freq;  // Target frequency for this station
};

#endif