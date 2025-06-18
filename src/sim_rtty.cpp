#include "sim_rtty.h"
#include "vfo.h"
#include "wavegen.h"

#define MAX_PHASE 36

SimRTTY::SimRTTY(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    _message = "CQ CQ DE N6CCM K       ";
    _phase = 0;
}

bool SimRTTY::begin(unsigned long time, float fixed_freq)
{    _fixed_freq = fixed_freq;
    _audible_frequency = 0.0;
      if (!Realization::begin(time))
        return false;
    
    WaveGen *wavegen = get_wavegen();
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
    
    _rtty.start_rtty_message(_message, true);
    
    return true;
}

bool SimRTTY::update(Mode *mode)
{    VFO *vfo = (VFO*)mode;
    _audible_frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _audible_frequency = _audible_frequency - _fixed_freq;    WaveGen *wavegen = get_wavegen();
    
    if (_audible_frequency <= MAX_AUDIBLE_FREQ && _audible_frequency >= MIN_AUDIBLE_FREQ) {
        if (!_enabled) {
            _enabled = true;
        }
        // Set both frequencies for RTTY FSK
        wavegen->set_frequency(_audible_frequency, true);                    // SPACE frequency
        wavegen->set_frequency(_audible_frequency + MARK_FREQ_SHIFT, false); // MARK frequency
    } else {
        if (_enabled) {
            _enabled = false;
            wavegen->set_frequency(SILENT_FREQ, true);
            wavegen->set_frequency(SILENT_FREQ, false);
        }
    }
    
    return true;
}

bool SimRTTY::step(unsigned long time)
{
    if (!_enabled) {        return Realization::step(time);
    }
    
    WaveGen *wavegen = get_wavegen();
    
    int rtty_result = _rtty.step_rtty(time);
    
    switch (rtty_result) {
        case STEP_RTTY_TURN_ON:
        case STEP_RTTY_LEAVE_ON:
            wavegen->set_active_frequency(false); // Use MARK frequency (channel 1)
            break;
        case STEP_RTTY_TURN_OFF:
        case STEP_RTTY_LEAVE_OFF:
            wavegen->set_active_frequency(true);  // Use SPACE frequency (channel 0)
            break;
    }
    
    _phase++;
    if (_phase >= MAX_PHASE) {
        _phase = 0;
    }
    
    return Realization::step(time);
}

void SimRTTY::end()
{
    if (_enabled) {
        WaveGen *wavegen = get_wavegen();
        wavegen->set_frequency(SILENT_FREQ, true);
        wavegen->set_frequency(SILENT_FREQ, false);
        _enabled = false;
    }
    Realization::end();
}

void SimRTTY::set_message(const char *message)
{
    _message = message;
    _rtty.start_rtty_message(message, true);
}
