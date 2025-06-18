
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
    if(!common_begin(time, fixed_freq))
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
    common_frequency_update(mode);

    if(_enabled){
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
