#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_numbers.h"
#include "signal_meter.h"

#ifdef PLATFORM_NATIVE
#include <cstdlib>  // For rand()
#include <cstdio>   // For sprintf
#else
#include <Arduino.h>
#endif

#define INTER_GROUP_DELAY 2000  // 2 seconds delay between number groups (more distinct)
#define INTER_CYCLE_DELAY 8000  // 8 seconds delay between complete cycles

SimNumbers::SimNumbers(RealizerPool *realizer_pool, SignalMeter *signal_meter) : SimTransmitter(realizer_pool), _signal_meter(signal_meter)
{
    // Base class initializes all common variables
    _groups_sent = 0;
    _total_groups_per_cycle = 13;  // 13 groups for creepiness
    _in_inter_group_delay = false;
    _next_group_time = 0;
    _transmission_active = false;    _wpm = 18;  // Default WPM
    
    // Enhanced numbers station state
    _current_phase = PHASE_INTERVAL_SIGNAL;
    _interval_repeats_sent = 0;
    _total_interval_repeats = DEFAULT_INTERVAL_REPEATS;  // Default interval repeats for optimal anticipation
}

bool SimNumbers::begin(unsigned long time, float fixed_freq, int wpm)
{
    if(!common_begin(time, fixed_freq))
        return false;
    
    _wpm = wpm;  // Store WPM setting for consistent use
    
    // Start with interval signal phase
    _current_phase = PHASE_INTERVAL_SIGNAL;
    _interval_repeats_sent = 0;
    _groups_sent = 0;
    
    generate_interval_signal();
    _morse.start_morse(_group_buffer, _wpm, false, 0);  // No repeat, no wait

    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    wavegen->set_frequency(NUMBERS_SPACE_FREQUENCY, false);

    return true;
}

void SimNumbers::realize()
{
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    wavegen->set_active_frequency(_active);
}

bool SimNumbers::update(Mode *mode)
{
    common_frequency_update(mode);
    
    if(_enabled){
        WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
        wavegen->set_frequency(_frequency);
    }

    realize();
    return true;
}

bool SimNumbers::step(unsigned long time)
{    // Handle morse code timing
    int morse_state = _morse.step_morse(time);
      switch(morse_state){
        case STEP_MORSE_TURN_ON:
            _active = true;
            _transmission_active = true;
            realize();
            send_carrier_charge_pulse();  // Send charge pulse when carrier turns on
            break;

        case STEP_MORSE_LEAVE_ON:
            // Carrier remains on - send another charge pulse
            send_carrier_charge_pulse();
            break;

        case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
            // No charge pulse when carrier turns off
            break;
            
        case STEP_MORSE_LEAVE_OFF:
            _active = false;
            realize();
            // No charge pulse when carrier is off
            break;
              case STEP_MORSE_MESSAGE_COMPLETE:
            // Message transmission just completed!
            _active = false;
            _transmission_active = false;
            realize();
            
            // Handle phase transitions
            switch(_current_phase) {
                case PHASE_INTERVAL_SIGNAL:
                    _interval_repeats_sent++;
                    if(_interval_repeats_sent >= _total_interval_repeats) {
                        // Interval signal complete, start numbers phase
                        _current_phase = PHASE_NUMBERS;
                        _groups_sent = 0;
                        _next_group_time = time + INTER_GROUP_DELAY;  // Short delay before first number
                    } else {
                        // Send another "FT"
                        _next_group_time = time + INTER_GROUP_DELAY;
                    }
                    break;
                    
                case PHASE_NUMBERS:
                    _groups_sent++;
                    if(_groups_sent >= _total_groups_per_cycle) {
                        // Numbers complete, send ending sequence
                        _current_phase = PHASE_ENDING;
                        _next_group_time = time + INTER_GROUP_DELAY;
                    } else {
                        // More groups in cycle, short delay
                        _next_group_time = time + INTER_GROUP_DELAY;
                    }
                    break;
                    
                case PHASE_ENDING:
                    // Ending sequence complete, start cycle delay
                    _current_phase = PHASE_CYCLE_DELAY;
                    _next_group_time = time + INTER_CYCLE_DELAY;
                    break;
                    
                case PHASE_CYCLE_DELAY:
                    // This shouldn't happen (handled in delay check below)
                    break;
            }
            
            _in_inter_group_delay = true;
            break;}
      // Check if it's time for next transmission
    if(_in_inter_group_delay && time >= _next_group_time) {
        _in_inter_group_delay = false;
        
        switch(_current_phase) {
            case PHASE_INTERVAL_SIGNAL:
                generate_interval_signal();
                _morse.start_morse(_group_buffer, _wpm, false, 0);
                break;
                
            case PHASE_NUMBERS:
                generate_next_number_group();
                _morse.start_morse(_group_buffer, _wpm, false, 0);
                break;
                
            case PHASE_ENDING:
                generate_ending_sequence();
                _morse.start_morse(_group_buffer, _wpm, false, 0);
                break;
                
            case PHASE_CYCLE_DELAY:
                // Cycle complete, restart with interval signal
                _current_phase = PHASE_INTERVAL_SIGNAL;
                _interval_repeats_sent = 0;
                _groups_sent = 0;
                generate_interval_signal();
                _morse.start_morse(_group_buffer, _wpm, false, 0);
                break;
        }
    }
    
    return true;
}

void SimNumbers::generate_next_number_group()
{
    // Generate one fresh random 5-digit group
    int digits[5];
    
    // Generate 5 random digits (0-9) - fresh every time!
    for(int i = 0; i < 5; i++) {
#ifdef PLATFORM_NATIVE
        digits[i] = rand() % 10;
#else
        digits[i] = random(10);
#endif
    }
    
    // Format as "XXXXX" (5 digits only, no space - we handle pauses with timing)
#ifdef PLATFORM_NATIVE
    snprintf(_group_buffer, sizeof(_group_buffer), "%d%d%d%d%d", 
             digits[0], digits[1], digits[2], digits[3], digits[4]);
#else
    sprintf(_group_buffer, "%d%d%d%d%d", 
            digits[0], digits[1], digits[2], digits[3], digits[4]);
#endif
}

void SimNumbers::generate_interval_signal()
{
    // Generate "FT" interval signal for FluxTune signature
    // This creates the anticipatory, hypnotic effect before numbers
#ifdef PLATFORM_NATIVE
    snprintf(_group_buffer, sizeof(_group_buffer), "FT");
#else
    sprintf(_group_buffer, "FT");
#endif
}

void SimNumbers::generate_ending_sequence()
{
    // Generate "00000" ending sequence (standard numbers station ending)
    // This clearly marks the end of the message transmission
#ifdef PLATFORM_NATIVE
    snprintf(_group_buffer, sizeof(_group_buffer), "00000");
#else
    sprintf(_group_buffer, "00000");
#endif
}

// Send charge pulse to signal meter based on VFO proximity
void SimNumbers::send_carrier_charge_pulse() {
    if (_signal_meter) {
        int charge = VFO::calculate_signal_charge(_fixed_freq, _vfo_freq);
        
        if (charge > 0) {
            _signal_meter->add_charge(charge);
        }
    }
}
