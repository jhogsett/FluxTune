#include "sim_transmitter.h"
#include "wavegen.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    // Initialize common member variables
    _fixed_freq = 0.0;
    _enabled = false;
    _frequency = 0.0;
    _active = false;
}

bool SimTransmitter::check_frequency_bounds()
{
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    
    if(_frequency > MAX_AUDIBLE_FREQ || _frequency < MIN_AUDIBLE_FREQ){
        if(_enabled){
            _enabled = false;
            wavegen->set_frequency(SILENT_FREQ, true);
            wavegen->set_frequency(SILENT_FREQ, false);
        }
        return false;  // Out of bounds
    } 
        
    if(!_enabled){
        _enabled = true;
    }
    
    return true;  // In bounds
}