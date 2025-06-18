#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

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
};

#endif