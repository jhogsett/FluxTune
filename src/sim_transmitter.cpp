#include "sim_transmitter.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    // Initialize common member variables
    _fixed_freq = 0.0;
    _enabled = false;
}