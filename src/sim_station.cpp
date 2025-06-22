#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_station.h"
#include "signal_meter.h"

#ifdef PLATFORM_NATIVE
#include <cstdlib>  // For rand()
#include <cstdio>   // For sprintf
#else
#include <Arduino.h>
#endif

#define WAIT_SECONDS 4

// mode is expected to be a derivative of VFO
SimStation::SimStation(WaveGenPool *realizer_pool, SignalMeter *signal_meter, float fixed_freq, int wpm) 
    : SimTransmitter(realizer_pool), _signal_meter(signal_meter), _stored_wpm(wpm)
{
    // Store fixed frequency in base class
    _fixed_freq = fixed_freq;    // Initialize operator frustration drift tracking
    _cycles_completed = 0;
#ifdef PLATFORM_NATIVE
    _cycles_until_qsy = 3 + (rand() % 6);  // 3-8 cycles before frustration (realistic)
#else
    _cycles_until_qsy = 3 + (random(6));   // 3-8 cycles before frustration (realistic)
#endif
    
    // Initialize repetition state
    _in_wait_delay = false;
    _next_cq_time = 0;
    
    // Generate random callsign and CQ message for this session
    generate_cq_message();
}

bool SimStation::begin(unsigned long time){
    if(!common_begin(time, _fixed_freq))
        return false;
    
    // DEBUG: Track reallocation and frequency setting
    #ifdef PLATFORM_NATIVE
    printf("DEBUG: SimStation::begin() - realizer=%d, time=%lu\n", _realizer, time);
    #endif
      WaveGen *wavegen = _realizer_pool->access_realizer(_realizer);
    wavegen->set_frequency(SPACE_FREQUENCY, false);    // Set _enabled and force frequency update with existing _vfo_freq
    // _vfo_freq should retain its value from the previous cycle
    _enabled = true;
    force_frequency_update();
    realize();  // CRITICAL: Set active state for audio output!
    
    #ifdef PLATFORM_NATIVE
    printf("DEBUG: SimStation::begin() - _enabled=%s, _vfo_freq=%f, _frequency=%f\n", 
           _enabled ? "true" : "false", _vfo_freq, _frequency);
    #endif
    
    // Start first CQ immediately (after frequencies are set)
    _morse.start_morse(_generated_message, _stored_wpm);
    _in_wait_delay = false;

    return true;
}

void SimStation::realize(){
    if(_realizer == -1) {
        return;  // No WaveGen allocated
    }
    
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
    #ifdef PLATFORM_NATIVE
    printf("DEBUG: SimStation::realize() - _active=%s, _frequency=%f\n", 
           _active ? "true" : "false", _frequency);
    #endif
    
    WaveGen *wavegen = _realizer_pool->access_realizer(_realizer);
    wavegen->set_active_frequency(_active);
}

// returns true on successful update
bool SimStation::update(Mode *mode){
    common_frequency_update(mode);
    
    #ifdef PLATFORM_NATIVE
    static int update_count = 0;
    if(update_count < 5) {  // Only show first few updates to avoid spam
        printf("DEBUG: SimStation::update() - _frequency=%f, _enabled=%s, _realizer=%d\n", 
               _frequency, _enabled ? "true" : "false", _realizer);
        update_count++;
    }
    #endif
    
    if(_enabled && _realizer != -1){
        WaveGen *wavegen = _realizer_pool->access_realizer(_realizer);
        wavegen->set_frequency(_frequency);
    }

    realize();

    return true;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool SimStation::step(unsigned long time){
    // Handle morse code timing
    int morse_state = _morse.step_morse(time);
      switch(morse_state){
    	case STEP_MORSE_TURN_ON:
            #ifdef PLATFORM_NATIVE
            printf("DEBUG: STEP_MORSE_TURN_ON - _frequency=%f, _enabled=%s, _realizer=%d\n", 
                   _frequency, _enabled ? "true" : "false", _realizer);
            #endif
            _active = true;
            realize();
            send_carrier_charge_pulse(_signal_meter);  // Send charge pulse when carrier turns on
    		break;

    	case STEP_MORSE_LEAVE_ON:
            // Carrier remains on - send another charge pulse
            send_carrier_charge_pulse(_signal_meter);
            break;

    	case STEP_MORSE_TURN_OFF:
            _active = false;
            realize();
            // No charge pulse when carrier turns off
    		break;
       	case STEP_MORSE_MESSAGE_COMPLETE:
            // CQ cycle completed! Check if operator gets frustrated and start wait delay
            _active = false;
            realize();
            
            _cycles_completed++;
            if(_cycles_completed >= _cycles_until_qsy) {
                // Operator gets frustrated, QSYs to new frequency
                apply_operator_frustration_drift();
                  // Reset frustration counter for next QSY
                _cycles_completed = 0;
#ifdef PLATFORM_NATIVE
                _cycles_until_qsy = 3 + (rand() % 6);  // 3-8 cycles before next frustration
#else
                _cycles_until_qsy = 3 + (random(6));   // 3-8 cycles before next frustration
#endif
            }
            
            // DYNAMIC PIPELINING: Free WaveGen at end of message cycle
            // This allows other stations to use the WaveGen during our wait period
            end();
            
            // Start wait delay before next CQ
            _in_wait_delay = true;
            _next_cq_time = time + (WAIT_SECONDS * 1000);
            break;
    }    // Check if it's time to start next CQ cycle
    if(_in_wait_delay && time >= _next_cq_time) {
        // DYNAMIC PIPELINING: Try to reallocate WaveGen for next message cycle
        if(begin(time)) {  // Only proceed if WaveGen is available
            _in_wait_delay = false;
            
            // Note: begin() now handles frequency update internally
        } else {
            // WaveGen not available - extend wait period and try again later
            _next_cq_time = time + 1000;  // Try again in 1 second
        }
    }
    
    return true;
}

// Use base class end() method for cleanup
void SimStation::generate_random_callsign(char *callsign_buffer, size_t buffer_size)
{
    // Generate fictional amateur radio callsigns for simulation
    // Uses doubled digits (00, 11, 22, etc.) to avoid generating real callsigns
    // This is like using "555" phone numbers in movies - sounds authentic but can't be real
    // Format: [W/K/N][XX][AAA] where XX = doubled digit (00-99)
      const char *prefixes[] = {"W", "K", "N"};
    
#ifdef PLATFORM_NATIVE
    // Improved randomness: Use time-based seeding for better distribution
    static unsigned long last_seed_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_seed_time > 1000) {  // Re-seed every second for better randomness
        srand(current_time);
        last_seed_time = current_time;
    }
    
    // More random prefix selection
    int prefix_idx = rand() % 3;
    int digit = rand() % 10;  // 0-9, will be doubled
    int suffix_len = 2 + (rand() % 2);  // 2 or 3 letters
    
    // Use doubled digit to ensure fictional callsign (e.g., W00ABC, K55XYZ, N99QRP)
    snprintf(callsign_buffer, buffer_size, "%s%d%d", prefixes[prefix_idx], digit, digit);
      for(int i = 0; i < suffix_len; i++) {
        char letter[2] = {'A' + (rand() % 26), '\0'};
        strncat(callsign_buffer, letter, buffer_size - strlen(callsign_buffer) - 1);
    }
#else    // Arduino version with improved randomness
    // Use current time for better seed distribution
    randomSeed(millis());
    
    int prefix_idx = random(3);
    int digit = random(10);  // 0-9, will be doubled
    int suffix_len = 2 + random(2);  // 2 or 3 letters
    
    // Use doubled digit to ensure fictional callsign
    sprintf(callsign_buffer, "%s%d%d", prefixes[prefix_idx], digit, digit);
      for(int i = 0; i < suffix_len; i++) {
        char letter[2] = {'A' + random(26), '\0'};
        strcat(callsign_buffer, letter);
    }
#endif
}

void SimStation::generate_cq_message()
{
    char callsign[8];  // Enough for W1ABC format
    generate_random_callsign(callsign, sizeof(callsign));
    
    // Generate standard CQ message: "CQ CQ DE [CALL] [CALL] K    "
#ifdef PLATFORM_NATIVE
    snprintf(_generated_message, sizeof(_generated_message), 
             "CQ CQ DE %s %s K    ", callsign, callsign);
#else
    sprintf(_generated_message, "CQ CQ DE %s %s K    ", callsign, callsign);
#endif
}

void SimStation::apply_operator_frustration_drift()
{
    // Operator gets frustrated by lack of response and QSYs (changes frequency)
    // Realistic amateur radio operator frequency adjustment
    const float DRIFT_RANGE = 75.0f;  // ±75 Hz - typical for frustrated amateur
    
#ifdef PLATFORM_NATIVE
    float drift = ((float)rand() / RAND_MAX) * (2.0f * DRIFT_RANGE) - DRIFT_RANGE;
#else
    float drift = ((float)random(0, (long)(2.0f * DRIFT_RANGE * 100))) / 100.0f - DRIFT_RANGE;
#endif
    
    // Apply drift to the base class frequency
    _fixed_freq = _fixed_freq + drift;
    
    // ENHANCEMENT: Generate new callsign to simulate a completely different operator
    // This makes it appear that a new station has come on frequency instead of
    // the same operator continuing to call CQ after frequency adjustment
    generate_cq_message();
    
    // Immediately update the wave generator frequency
    force_frequency_update();
}
