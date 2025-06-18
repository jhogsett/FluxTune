
#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_station.h"

#define WAIT_SECONDS 4

// mode is expected to be a derivative of VFO
SimStation::SimStation(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    // Base class now initializes all common variables
}

bool SimStation::begin(unsigned long time, float fixed_freq, const char *message, int wpm){
    _fixed_freq = fixed_freq;
    _frequency = 0.0;

    // attempt to acquire a realizer
    // _realizer = _realizer_pool->get_realizer();
    // if(_realizer == -1)
    //     return false;
    if(!Realization::begin(time))
        return false;

    _morse.start_morse(message, wpm, true, WAIT_SECONDS);

    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    wavegen->set_frequency(SPACE_FREQUENCY, false);

    return true;
}

void SimStation::realize(){
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimStation::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

    // _frequency = abs(_frequency - _fixed_freq);
    _frequency = _frequency - _fixed_freq;

    if(_enabled){
        // WaveGen  *wavegen = (WaveGen*)_realizer;

        WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);

        wavegen->set_frequency(_frequency);
    }

    realize();

    return true;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool SimStation::step(unsigned long time){
    switch(_morse.step_morse(time)){
    	case STEP_MORSE_TURN_ON:
            _active = true;
            realize();
    		break;

    	case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
    		break;
    }

    return true;
}

void SimStation::end(){
    if(_realizer != -1)
        _realizer_pool->free_realizer(_realizer);
}
