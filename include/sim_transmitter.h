#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

// Shared constants - frequency limits and values
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

/**
 * Minimal base class for simulated transmitting stations.
 * Gradually moving shared functionality here through incremental refactoring.
 */
class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);
    
    // No additional functionality yet - derived classes handle everything

protected:
    // Simple utility method shared by all transmitters
    class WaveGen* get_wavegen();
    
    // Shared data members
    float _fixed_freq;        // Target frequency for this station
    float _audible_frequency; // Current audible frequency difference from VFO
    bool _enabled;            // True when frequency is in audible range
};

#endif
