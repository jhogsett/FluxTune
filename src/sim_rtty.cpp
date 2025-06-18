
#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_rtty.h"

// mode is expected to be a derivative of VFO
SimRTTY::SimRTTY(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool){
    // Base class now initializes all common variables
    // Don't start transmission here - wait for begin() for proper timing
}

bool SimRTTY::begin(unsigned long time, float fixed_freq){    if(!common_begin(time, fixed_freq))
        return false;    // Start RTTY transmission with repeat enabled
    _rtty.start_rtty_message("CQ CQ DE N6CCM K       ", true);

    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));

    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);

    // this might turn on too early, maybe enable late
    // wavegen->set_active_frequency(true);
    return true;
}

#define MAX_PHASE 36

void SimRTTY::realize(){
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimRTTY::update(Mode *mode){
    common_frequency_update(mode);    if(_enabled){
        WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
        wavegen->set_frequency(_frequency, true);
        wavegen->set_frequency(_frequency + MARK_FREQ_SHIFT, false);
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
