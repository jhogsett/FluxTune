
#include "vfo.h"
#include "wavegen.h"
#include "sim_station.h"

// mode is expected to be a derivative of VFO
SimStation::SimStation(Realizer *realizer) : Realization(realizer)
{
    _realizer = realizer;
    _frequency = 0.0;
    _morse.start_morse("CQ CQ CD DE W1AW W1AW K                ", 13, true);
    _active = false;
}

#define MAX_AUDIBLE_FREQ 5000.0
#define SPACE_FREQUENCY 0.1

void SimStation::realize(){
    WaveGen  *wavegen = (WaveGen*)_realizer;
    // Serial.println(_frequency);
    if(_active && (_frequency <= MAX_AUDIBLE_FREQ)){
        // Serial.println("1");
    	wavegen->set_frequency(_frequency);
    } else {
        // Serial.println("2");
    	wavegen->set_frequency(SPACE_FREQUENCY);
    }
}

// returns true on successful update
bool SimStation::update(Mode *mode){
    // Serial.println("--------");
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = abs(_frequency - 7010000.0);

    realize();

    return true;
}

// // returns true on successful begin
// void SimStation::begin(unsigned long time){
//     // _started = time;
//     // _period = period;
//     // _next_internal_step = time + _period;
// }

// call periodically to keep realization dynamic
// returns true if it should keep going
bool SimStation::step(unsigned long time){
    switch(_morse.step_morse(time)){
    	case STEP_MORSE_TURN_ON:
            // Serial.println("A");
            _active = true;
            realize();
    		// AD1.setActiveFrequency((MD_AD9833::channel_t)0);
    		// AD1.setFrequency((MD_AD9833::channel_t)0, 900.0);
    		break;

    	case STEP_MORSE_TURN_OFF:
            // Serial.println("B");
            _active = false;
            realize();
    		// AD1.setActiveFrequency((MD_AD9833::channel_t)1);
    		// AD1.setFrequency((MD_AD9833::channel_t)0, 0.0);
    		break;
    }

    return true;
}

// void Realization::internal_step(unsigned long time){
// }

// void SimStation::end(){

// }
