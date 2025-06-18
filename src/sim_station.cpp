
#include "sim_station.h"
#include "vfo.h"
#include "wavegen.h"

#define WAIT_SECONDS 4
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

SimStation::SimStation(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    _fixed_freq = 0.0;
    _frequency = 0.0;
    _enabled = false;
    _message = nullptr;
    _wpm = 20;
    _message_set = false;
}

bool SimStation::begin(unsigned long time, float fixed_freq, const char *message, int wpm)
{
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    _message = message;
    _wpm = wpm;
    _message_set = true;
    
    if (!Realization::begin(time))
        return false;
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
    
    if (_message && _message_set) {
        _morse.start_morse(_message, _wpm, true, WAIT_SECONDS);
    }
    
    return true;
}

bool SimStation::update(Mode *mode)
{
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = _frequency - _fixed_freq;
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    
    if (_frequency <= MAX_AUDIBLE_FREQ && _frequency >= MIN_AUDIBLE_FREQ) {
        if (!_enabled) {
            _enabled = true;
        }
        // Set both frequencies for Morse keying
        wavegen->set_frequency(_frequency, true);        // Active tone frequency
        wavegen->set_frequency(SPACE_FREQUENCY, false);  // Silent frequency
    } else {
        if (_enabled) {
            _enabled = false;
            wavegen->set_frequency(SILENT_FREQ, true);
            wavegen->set_frequency(SILENT_FREQ, false);
        }
    }
    
    return true;
}

bool SimStation::step(unsigned long time)
{
    if (!_enabled) {
        return Realization::step(time);
    }
    
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
    
    int morse_result = _morse.step_morse(time);
    
    switch (morse_result) {
        case STEP_MORSE_TURN_ON:
        case STEP_MORSE_LEAVE_ON:
            wavegen->set_active_frequency(true);  // Use tone frequency
            break;
        case STEP_MORSE_TURN_OFF:
        case STEP_MORSE_LEAVE_OFF:
            wavegen->set_active_frequency(false); // Use silent frequency
            break;
    }
    
    return Realization::step(time);
}

void SimStation::end()
{
    if (_enabled) {
        WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);
        wavegen->set_frequency(SILENT_FREQ, true);
        wavegen->set_frequency(SILENT_FREQ, false);
        _enabled = false;
    }
    Realization::end();
}

void SimStation::set_message(const char *message, int wpm)
{
    _message = message;
    _wpm = wpm;
    if (_message_set) {
        _morse.start_morse(message, wpm, true, WAIT_SECONDS);
    }
}
