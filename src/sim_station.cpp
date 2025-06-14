
#include "vfo.h"
#include "wavegen.h"
#include "sim_station.h"

#define WAIT_SECONDS 4

// mode is expected to be a derivative of VFO
SimStation::SimStation(Realizer *realizer) : Realization(realizer)
{
    _realizer = realizer;
    _active = false;
    _enabled = false;
}

void SimStation::begin(unsigned long time, float fixed_freq, const char *message, int wpm){
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    _morse.start_morse(message, wpm, true, WAIT_SECONDS);
    WaveGen  *wavegen = (WaveGen*)_realizer;
    wavegen->set_frequency(SPACE_FREQUENCY, false);
}

void SimStation::realize(){
    WaveGen  *wavegen = (WaveGen*)_realizer;

    if(_frequency > MAX_AUDIBLE_FREQ || _frequency < MIN_AUDIBLE_FREQ){
        if(_enabled){
            _enabled = false;
            wavegen->set_frequency(SILENT_FREQ, true);
            wavegen->set_frequency(SILENT_FREQ, false);
        }
        return;
    } 
        
    if(!_enabled){
        _enabled = true;
    }

    // if(_frequency > MAX_AUDIBLE_FREQ || _frequency < MIN_AUDIBLE_FREQ){
    //     wavegen->set_active_frequency(false);
    //     return;
    // }

    wavegen->set_active_frequency(_active);

    // if(_active && (_frequency <= MAX_AUDIBLE_FREQ)){
    // 	// wavegen->set_frequency(_frequency);

    // } else {
    //     wavegen->set_active_frequency(false);
    // 	// wavegen->set_frequency(SPACE_FREQUENCY);
    // }
}

// returns true on successful update
bool SimStation::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

    // _frequency = abs(_frequency - _fixed_freq);
    _frequency = _frequency - _fixed_freq;

    if(_enabled){
        WaveGen  *wavegen = (WaveGen*)_realizer;
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
