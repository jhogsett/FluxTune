#include "vfo.h"
#include "wavegen.h"
#include "wave_gen_pool.h"
#include "sim_rtty.h"
#include "signal_meter.h"

// Single authentic RTTY message for realistic station simulation
static const char rtty_message[] = "CQ CQ DE N6CCM K       ";

// mode is expected to be a derivative of VFO
SimRTTY::SimRTTY(WaveGenPool *wave_gen_pool, SignalMeter *signal_meter, float fixed_freq) 
    : SimTransmitter(wave_gen_pool), _signal_meter(signal_meter)
{
    // Store fixed frequency in base class
    _fixed_freq = fixed_freq;    // Initialize message cycling state - start with first message
    _in_wait_delay = false;
    _next_message_time = 0;
    _message_repeat_count = 3;  // Repeat each message 3 times for longer transmission
    _current_repeat = 0;
    
    // Start first message transmission immediately
    start_next_message();
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
    // Handle state transitions for message cycling and idle patterns
    if (_in_wait_delay) {
        // Check if wait period is over
        if (time >= _next_message_time) {
            _in_wait_delay = false;
            
            // Skip idle pattern and start next message directly  
            start_next_message();
        }
        // During wait, keep transmitter silent
        return true;
    }
    
    // Process RTTY state machine
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

    // Check for message completion to trigger state transitions
    if (_rtty.is_message_complete()) {
        if (!_in_wait_delay) {
            // Regular message finished - check if we need more repetitions            
            _current_repeat++;
            if (_current_repeat < _message_repeat_count) {
                // Send the same message again
                _rtty.start_rtty_message(rtty_message, false);
            } else {
                // All repetitions done, start wait delay
                _in_wait_delay = true;
                _next_message_time = time + (RTTY_WAIT_SECONDS * 1000);
            }
        }
    }

    return true;
}

void SimRTTY::start_next_message()
{
    // Reset repetition counter and start transmission of the message
    _current_repeat = 0;
    _rtty.start_rtty_message(rtty_message, false);
}
