
#include "vfo.h"
#include "wavegen.h"
#include "sim_station.h"

// mode is expected to be a derivative of VFO
SimStation::SimStation(Realizer *realizer, float fixed_freq, const char *message, int wpm) : Realization(realizer)
{
    _realizer = realizer;
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    _morse.start_morse(message, wpm, true);
    _active = false;
}

void SimStation::begin(unsigned long time){
    WaveGen  *wavegen = (WaveGen*)_realizer;
    wavegen->set_frequency(SPACE_FREQUENCY, false);
}

void SimStation::realize(){
    WaveGen  *wavegen = (WaveGen*)_realizer;

    // Serial.println(_frequency);
    if(_frequency > MAX_AUDIBLE_FREQ || _frequency < MIN_AUDIBLE_FREQ){
        // Serial.println("out of range");
        wavegen->set_active_frequency(false);
        return;
    }

    // Serial.println(_frequency);
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
    _frequency = abs(_frequency - _fixed_freq);

    WaveGen  *wavegen = (WaveGen*)_realizer;
    wavegen->set_frequency(_frequency);

    // this may not be needed
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
