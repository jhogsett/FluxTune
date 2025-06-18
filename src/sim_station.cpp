#include "sim_station.h"
#include "vfo.h"
#include "wavegen.h"
// #include <string.h>  // Removed due to RAM constraints

// Use WAIT_SECONDS from header

SimStation::SimStation(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    _message = nullptr;
    _wpm = 20;
    _message_set = false;
      // Initialize restart parameters
    _init_params_set = false;
    _init_fixed_freq = 0.0;
    _init_message = nullptr;
    _init_wpm = 20;
    _transmission_complete = false;
    _last_cycle_time = 0;    // Initialize simple cycle diagnostics
    _cycle_diagnostics = false;
    _diagnostic_cycles = 0;
}

bool SimStation::begin(unsigned long time, float fixed_freq, const char *message, int wpm)
{
    // Store parameters for restart capability
    _init_fixed_freq = fixed_freq;
    _init_message = message;
    _init_wpm = wpm;
    _init_params_set = true;
    _transmission_complete = false;
    
    _fixed_freq = fixed_freq;
    _audible_frequency = 0.0;
    _message = message;
    _wpm = wpm;
    _message_set = true;
      if (!Realization::begin(time))
        return false;
        
    WaveGen *wavegen = get_wavegen();
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
    _audible_frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _audible_frequency = _audible_frequency - _fixed_freq;
    
    WaveGen *wavegen = get_wavegen();
    
    if (_audible_frequency <= MAX_AUDIBLE_FREQ && _audible_frequency >= MIN_AUDIBLE_FREQ) {
        if (!_enabled) {
            _enabled = true;
        }
        // Set both frequencies for Morse keying
        wavegen->set_frequency(_audible_frequency, true);        // Active tone frequency
        wavegen->set_frequency(SPACE_FREQUENCY, false);          // Silent frequency
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
    if (!_enabled) {        return Realization::step(time);
    }
    
    WaveGen *wavegen = get_wavegen();
      int morse_result = _morse.step_morse(time);
    
    // Handle CW key states and transmission completion
    switch (morse_result) {
        case STEP_MORSE_TURN_ON:
        case STEP_MORSE_LEAVE_ON:
            wavegen->set_active_frequency(true);  // Use tone frequency
            break;
        case STEP_MORSE_TURN_OFF:
        case STEP_MORSE_LEAVE_OFF:
            wavegen->set_active_frequency(false); // Use silent frequency
            break;
        case STEP_MORSE_CYCLE_COMPLETE:
            // A complete transmission cycle just finished
            _current_runs++;  // Increment lifecycle counter
            _transmission_complete = true;
            break;
    }
    
    // Track transmission cycles for auto-stopping
    check_transmission_cycle();
    
    return Realization::step(time);
}

void SimStation::end()
{
    if (_enabled) {
        WaveGen *wavegen = get_wavegen();
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

// Override restart to restore station parameters
bool SimStation::try_restart(unsigned long time)
{
    if (_init_params_set && !is_active()) {
        // Reset diagnostic parameters to original values on restart
        if (_cycle_diagnostics) {
            _wpm = _init_wpm;
            _fixed_freq = _init_fixed_freq;
        }
        
        // Add slight frequency variation on restart to make it audible
        float freq_variation = (random(200) - 100); // ±100 Hz variation
        float restart_freq = _init_fixed_freq + freq_variation;
        
        // Debug output disabled to prevent Arduino instability
        // #ifdef NATIVE_BUILD
        //     printf("SimStation restarting with freq variation: %.1f Hz\n", freq_variation);
        // #else
        //     Serial.print("SimStation restarting with freq variation: ");
        //     Serial.print(freq_variation);
        //     Serial.println(" Hz");
        // #endif
        
        return begin(time, restart_freq, _init_message, _init_wpm);
    }
    return false;
}

// Track transmission cycles - improved per-instance tracking
void SimStation::check_transmission_cycle()
{    // Real transmission cycle detection (for lifecycle management)
    // This is now handled in step() method using morse_result
    
    // Track based on time intervals for each station instance
    unsigned long current_time = millis();
      // Estimate cycle time: message + pauses should be roughly 30-60 seconds
    if (_last_cycle_time == 0) {
        _last_cycle_time = current_time; // Initialize on first call
    }    if (current_time - _last_cycle_time > 15000) { // 15 second intervals for easier observation
        _diagnostic_cycles++;  // Diagnostic-specific counter only
        _last_cycle_time = current_time;
        
        // Simple cycle diagnostics: increment WPM and frequency
        if (_cycle_diagnostics) {
            // Reset to original values after 4 diagnostic cycles
            if (_diagnostic_cycles >= 4) {
                _diagnostic_cycles = 0;  // Reset diagnostic cycle counter
                _wpm = _init_wpm;   // Reset to original WPM
                _fixed_freq = _init_fixed_freq;  // Reset to original frequency
            } else {
                _wpm += 3;  // Increase by 3 WPM each cycle  
                _fixed_freq += 100.0;  // Back to 100 Hz steps since 200 didn't help
            }// Force immediate frequency update by adjusting the current audible frequency
            if (_enabled) {
                WaveGen *wavegen = get_wavegen();
                if (wavegen) {
                    // Adjust current audible frequency by the change we made to _fixed_freq
                    _audible_frequency -= 100.0;  // Since _fixed_freq increased by 100, audible decreases by 100
                    if (_audible_frequency <= MAX_AUDIBLE_FREQ && _audible_frequency >= MIN_AUDIBLE_FREQ) {
                        wavegen->set_frequency(_audible_frequency, true);
                        wavegen->set_frequency(SPACE_FREQUENCY, false);
                    }
                }
            }
            
            // Restart transmission with new WPM
            _morse.start_morse(_message, _wpm, true, WAIT_SECONDS);
        }
          // Debug output disabled to prevent Arduino instability
        // #ifdef NATIVE_BUILD
        //     printf("SimStation cycle completed, run count: %d/%d\n", _current_runs, _max_runs);
        // #else
        //     Serial.print("SimStation cycle completed, run count: ");
        //     Serial.print(_current_runs);
        //     Serial.print("/");
        //     Serial.println(_max_runs);
        // #endif
    }
    
    // Real transmission cycle detection (for lifecycle management)
    // This runs separately from diagnostic cycles
    static unsigned long last_activity_check = 0;
    if (current_time - last_activity_check > 30000) { // Check every 30 seconds for completed transmissions
        _current_runs++;  // Increment real transmission cycle counter for lifecycle
        last_activity_check = current_time;
        _transmission_complete = true;
    }
}

// Diagnostic message building removed due to RAM constraints
