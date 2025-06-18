#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_pager.h"

SimPager::SimPager(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    // Base class initializes all common variables
    // Pager transmission will be started in begin() method
}

bool SimPager::begin(unsigned long time, float fixed_freq)
{
    if(!common_begin(time, fixed_freq))
        return false;    // Start pager transmission with repeat enabled
    _pager.start_pager_transmission(true);

    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));

    // Initialize both channels to silent
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);

    return true;
}

void SimPager::realize()
{
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    
    if(_active) {
        // Set frequencies based on current pager state
        switch(_pager.get_current_state()) {
            case PAGER_STATE_TONE_A:
                // Transmit Tone A on both channels (or just one)
                wavegen->set_frequency(_frequency + PAGER_TONE_A_OFFSET, true);
                wavegen->set_frequency(_frequency + PAGER_TONE_A_OFFSET, false);
                break;
                
            case PAGER_STATE_TONE_B:
                // Transmit Tone B on both channels (or just one) 
                wavegen->set_frequency(_frequency + PAGER_TONE_B_OFFSET, true);
                wavegen->set_frequency(_frequency + PAGER_TONE_B_OFFSET, false);
                break;
                
            default:
                // Silent state - should not reach here when _active is true
                wavegen->set_frequency(SILENT_FREQ, true);
                wavegen->set_frequency(SILENT_FREQ, false);
                break;
        }
    } else {
        // Explicitly set silent frequencies when inactive
        wavegen->set_frequency(SILENT_FREQ, true);
        wavegen->set_frequency(SILENT_FREQ, false);
    }
    
    wavegen->set_active_frequency(_active);
}

bool SimPager::update(Mode *mode)
{
    common_frequency_update(mode);

    if(_enabled) {
        // Note: We don't set frequencies here like RTTY does
        // Pager frequencies are set in realize() based on current tone
    }

    realize();
    return true;
}

bool SimPager::step(unsigned long time)
{
    switch(_pager.step_pager(time)) {
        case STEP_PAGER_TURN_ON:
            _active = true;
            realize();
            break;

        case STEP_PAGER_TURN_OFF:
            _active = false;
            realize();
            break;
              case STEP_PAGER_CHANGE_FREQ:
            // Transmitter stays on, but frequency needs to change
            realize();
            break;
            
        // LEAVE_ON and LEAVE_OFF don't require action since _active state doesn't change
        // and no frequency update is needed during silence
    }

    return true;
}
