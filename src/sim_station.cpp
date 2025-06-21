#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_station.h"
#include "signal_meter.h"

#define WAIT_SECONDS 4

// mode is expected to be a derivative of VFO
SimStation::SimStation(RealizerPool *realizer_pool, SignalMeter *signal_meter, float fixed_freq, const char *message, int wpm) 
    : SimTransmitter(realizer_pool), _signal_meter(signal_meter)
{
    // Base class now initializes all common variables
    _stored_fixed_freq = fixed_freq;
    _stored_message = message;
    _stored_wpm = wpm;
}

bool SimStation::begin(unsigned long time){
    if(!common_begin(time, _stored_fixed_freq))
        return false;
        
    _morse.start_morse(_stored_message, _stored_wpm, true, WAIT_SECONDS);

    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    wavegen->set_frequency(SPACE_FREQUENCY, false);

    return true;
}

void SimStation::realize(){
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimStation::update(Mode *mode){
    common_frequency_update(mode);
    
    if(_enabled){
        WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
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
            send_carrier_charge_pulse(_signal_meter);  // Send charge pulse when carrier turns on
    		break;

    	case STEP_MORSE_LEAVE_ON:
            // Carrier remains on - send another charge pulse
            send_carrier_charge_pulse(_signal_meter);
            break;

    	case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
            // No charge pulse when carrier turns off
    		break;
    }
    
    return true;
}

// Use base class end() method for cleanup
