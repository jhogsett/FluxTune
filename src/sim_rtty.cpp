
#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_rtty.h"

// mode is expected to be a derivative of VFO
SimRTTY::SimRTTY(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool){
    // Base class now initializes _active, _enabled, _fixed_freq, _frequency
    _rtty.start_rtty_message("CQ CQ DE N6CCM K       ", true);
}

bool SimRTTY::begin(unsigned long time, float fixed_freq){
    _fixed_freq = fixed_freq;
    _audible_frequency = 0.0;

    // attempt to acquire a realizer
    // _realizer = _realizer_pool->get_realizer();
    // if(_realizer == -1)
    //     return false;
    if(!Realization::begin(time))
        return false;

    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);

    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);

    // this might turn on too early, maybe enable late
    // wavegen->set_active_frequency(true);
    return true;
}

#define MAX_PHASE 36

void SimRTTY::realize(){
    // WaveGen  *wavegen = (WaveGen*)_realizer;
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);

    if(_audible_frequency > MAX_AUDIBLE_FREQ || _audible_frequency < MIN_AUDIBLE_FREQ){
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

    // wavegen->set_frequency(_frequency, true);
    // wavegen->set_frequency(_frequency + MARK_FREQ_SHIFT, false);

    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimRTTY::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    _audible_frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

    // _audible_frequency = abs(_audible_frequency - _fixed_freq);
    _audible_frequency = _audible_frequency - _fixed_freq;
    if(_enabled){
        // WaveGen  *wavegen = (WaveGen*)_realizer;
        WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
        wavegen->set_frequency(_audible_frequency, true);
        wavegen->set_frequency(_audible_frequency + MARK_FREQ_SHIFT, false);
    }

    realize();

    return true;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool SimRTTY::step(unsigned long time){
    switch(_rtty.step_rtty(time)){
    	case STEP_RTTY_TURN_ON:
            _active = true;
            realize();
    		break;

    	case STEP_RTTY_TURN_OFF:
            _active = false;
            realize();
    		break;
    }

    return true;
}
