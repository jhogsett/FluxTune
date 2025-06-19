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

#define WAIT_SECONDS 8  // Longer pause between transmissions for spooky effect

SimNumbers::SimNumbers(RealizerPool *realizer_pool) : SimTransmitter(realizer_pool)
{
    // Base class initializes all common variables
    _current_group = 1;
    _transmission_complete = true;  // Start by generating first transmission
}

bool SimNumbers::begin(unsigned long time, float fixed_freq)
{
    if(!common_begin(time, fixed_freq))
        return false;
    
    // Generate initial transmission
    generate_next_transmission();
    _morse.start_morse(_transmission_buffer, 8, true, WAIT_SECONDS);  // Slow, deliberate 8 WPM

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
    switch(_morse.step_morse(time)){
        case STEP_MORSE_TURN_ON:
            _active = true;
            realize();
            break;

        case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
            break;
    }
    
    return true;
}

void SimNumbers::generate_next_transmission()
{
    // Create creepy number group transmission
    char group_text[20];
    generate_number_group(group_text, _current_group);
    
    // Format: "GROUP N... X-X-X-X-X... GROUP N... Y-Y-Y-Y-Y..."
#ifdef PLATFORM_NATIVE
    snprintf(_transmission_buffer, sizeof(_transmission_buffer), 
             "GROUP %d   %s   GROUP %d   ", 
             _current_group, group_text, _current_group + 1);
#else
    sprintf(_transmission_buffer, 
            "GROUP %d   %s   GROUP %d   ", 
            _current_group, group_text, _current_group + 1);
#endif
    
    _current_group += 2;  // Next transmission will be groups N+2, N+3
    
    // Reset to group 1 after reaching 99 to keep it manageable
    if(_current_group > 99) {
        _current_group = 1;
    }
}

void SimNumbers::generate_number_group(char *buffer, int group_number)
{
    // Generate a 5-digit number group with dashes for authentic feel
    // Use group number to seed some predictability but add randomness
    
    int digits[5];
    
    // Generate 5 random digits (0-9)
    for(int i = 0; i < 5; i++) {
#ifdef PLATFORM_NATIVE
        digits[i] = rand() % 10;
#else
        digits[i] = random(10);
#endif
    }
    
    // Format as "X-X-X-X-X"
#ifdef PLATFORM_NATIVE
    snprintf(buffer, 20, "%d-%d-%d-%d-%d", 
             digits[0], digits[1], digits[2], digits[3], digits[4]);
#else
    sprintf(buffer, "%d-%d-%d-%d-%d", 
            digits[0], digits[1], digits[2], digits[3], digits[4]);
#endif
}
