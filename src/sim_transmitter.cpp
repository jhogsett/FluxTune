#include "sim_transmitter.h"
#include "wavegen.h"
#include "vfo.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    // Initialize common member variables
    _fixed_freq = 0.0;
    _enabled = false;
    _frequency = 0.0;
    _active = false;
}

bool SimTransmitter::common_begin(unsigned long time, float fixed_freq)
{
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    
    return Realization::begin(time);
}

void SimTransmitter::common_frequency_update(Mode *mode)
{
    // Note: mode is expected to be a VFO object
    VFO *vfo = static_cast<VFO*>(mode);
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = _frequency - _fixed_freq;
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

void SimTransmitter::end()
{
    if(_realizer != -1) {
        _realizer_pool->free_realizer(_realizer);
        _realizer = -1;  // Reset to avoid double-free or invalid access
    }
}