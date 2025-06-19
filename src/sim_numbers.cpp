#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_numbers.h"

#ifdef PLATFORM_NATIVE
#include <cstdlib>  // For rand()
#include <cstdio>   // For sprintf
#else
#include <Arduino.h>
#endif

#define INTER_GROUP_DELAY 2000  // 2 seconds delay between number groups (more distinct)
#define INTER_CYCLE_DELAY 8000  // 8 seconds delay between complete cycles

SimNumbers::SimNumbers(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    // Base class initializes all common variables
    _groups_sent = 0;
    _total_groups_per_cycle = 13;  // 13 groups for creepiness
    _in_inter_group_delay = false;
    _next_group_time = 0;
    _transmission_active = false;
    _wpm = 18;  // Default WPM
}

bool SimNumbers::begin(unsigned long time, float fixed_freq, int wpm)
{
    if(!common_begin(time, fixed_freq))
        return false;
    
    _wpm = wpm;  // Store WPM setting for consistent use
    
    // Start with first group immediately
    generate_next_number_group();
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
{
    // Handle morse code timing
    int morse_state = _morse.step_morse(time);
      switch(morse_state){
        case STEP_MORSE_TURN_ON:
            _active = true;
            _transmission_active = true;
            realize();
            break;

        case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
            break;
            
        case STEP_MORSE_LEAVE_OFF:
            _active = false;
            realize();
            break;
            
        case STEP_MORSE_MESSAGE_COMPLETE:
            // Message transmission just completed!
            _active = false;
            _transmission_active = false;
            realize();
            
            // Start delay before next group
            _in_inter_group_delay = true;
            _groups_sent++;
            
            if(_groups_sent >= _total_groups_per_cycle) {
                // Cycle complete, long delay
                _next_group_time = time + INTER_CYCLE_DELAY;
                _groups_sent = 0;
            } else {
                // More groups in cycle, short delay
                _next_group_time = time + INTER_GROUP_DELAY;
            }
            break;    }
    
    // Check if it's time for next group
    if(_in_inter_group_delay && time >= _next_group_time) {
        _in_inter_group_delay = false;
        // Generate fresh random group
        generate_next_number_group();
        _morse.start_morse(_group_buffer, _wpm, false, 0);  // Start new group with stored WPM
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
