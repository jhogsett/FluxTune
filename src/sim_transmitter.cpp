#include "sim_transmitter.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    // Initialize common member variables
    _active = false;
    _enabled = false;
    _fixed_freq = 0.0;
    _frequency = 0.0;
}
