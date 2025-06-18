#include "sim_transmitter.h"
#include "wavegen.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    _fixed_freq = 0.0;
    _audible_frequency = 0.0;
    _enabled = false;
}

WaveGen* SimTransmitter::get_wavegen()
{
    return (WaveGen*)_realizer_pool->access_realizer(_realizer);
}
