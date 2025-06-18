#include "sim_rtty.h"
#include "vfo.h"
#include "wavegen.h"

#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1
#define MAX_PHASE 36

SimRTTY::SimRTTY(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    _fixed_freq = 0.0;
    _frequency = 0.0;
    _enabled = false;
    _message = "CQ CQ DE N6CCM K       ";
    _phase = 0;
}

bool SimRTTY::begin(unsigned long time, float fixed_freq)
{
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    
    if (!Realization::begin(time))
        return false;
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
    
    _rtty.start_rtty_message(_message, true);
    
    return true;
}

bool SimRTTY::update(Mode *mode)
{
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = _frequency - _fixed_freq;
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    
    if (_frequency <= MAX_AUDIBLE_FREQ && _frequency >= MIN_AUDIBLE_FREQ) {
        if (!_enabled) {
            _enabled = true;
        }
        // Set both frequencies for RTTY FSK
        wavegen->set_frequency(_frequency, true);                    // SPACE frequency
        wavegen->set_frequency(_frequency + MARK_FREQ_SHIFT, false); // MARK frequency
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
    if (!_enabled) {
        return Realization::step(time);
    }
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    
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
        WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
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
