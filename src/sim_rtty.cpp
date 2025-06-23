#include "vfo.h"
#include "wavegen.h"
#include "wave_gen_pool.h"
#include "sim_rtty.h"
#include "signal_meter.h"

// mode is expected to be a derivative of VFO
SimRTTY::SimRTTY(WaveGenPool *wave_gen_pool, SignalMeter *signal_meter, float fixed_freq) 
    : SimTransmitter(wave_gen_pool), _signal_meter(signal_meter)
{
    // Store fixed frequency in base class
    _fixed_freq = fixed_freq;
    _rtty.start_rtty_message("CQ CQ DE N6CCM K       ", true);
}

bool SimRTTY::begin(unsigned long time){
    if(!common_begin(time, _fixed_freq))
        return false;

    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);

    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);

    return true;
}

#define MAX_PHASE 36

void SimRTTY::realize(){
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
      WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimRTTY::update(Mode *mode){
    common_frequency_update(mode);    if(_enabled){
        WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
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
            send_carrier_charge_pulse(_signal_meter);  // Send charge pulse when carrier turns on
    		break;

    	case STEP_RTTY_LEAVE_ON:
            // Carrier remains on - send another charge pulse
            send_carrier_charge_pulse(_signal_meter);
            break;

    	case STEP_RTTY_TURN_OFF:
            _active = false;
            realize();
            // No charge pulse when carrier turns off
    		break;
    }

    return true;
}
