#include "vfo.h"
#include "wavegen.h"
#include "wave_gen_pool.h"
#include "sim_pager2.h"
#include "signal_meter.h"

SimPager2::SimPager2(WaveGenPool *wave_gen_pool, SignalMeter *signal_meter, float fixed_freq) 
    : SimTransmitter(wave_gen_pool, fixed_freq), _signal_meter(signal_meter),
      _current_dtmf_digit_1('?'), _current_dtmf_digit_2('?')
{
#if defined(ENABLE_SECOND_GENERATOR) || defined(ENABLE_DUAL_GENERATOR)
    _realizer_b = -1;
#endif

    // Generate initial tone pair
    generate_new_tone_pair();
    // Pager transmission will be started in begin() method
}

bool SimPager2::begin(unsigned long time)
{
#ifdef ENABLE_FIRST_GENERATOR
    // FIRST GENERATOR ONLY MODE
    if(!common_begin(time, _fixed_freq))
        return false;
        
    // Start pager transmission with repeat enabled
    _pager.start_pager_transmission(true);

    // Check if we have a valid realizer before accessing it
    if(_realizer == -1) {
        return false;  // No realizer available
    }

    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);

    // Initialize both channels to silent
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
#endif

#ifdef ENABLE_SECOND_GENERATOR
    // SECOND GENERATOR ONLY MODE
    if(!acquire_second_generator()) {
        return false;  // No second generator available
    }
    
    // Start pager transmission with repeat enabled
    _pager.start_pager_transmission(true);

    WaveGen *wavegen_b = _wave_gen_pool->access_realizer(_realizer_b);

    // Initialize both channels to silent
    wavegen_b->set_frequency(SILENT_FREQ, false);
    wavegen_b->set_frequency(SILENT_FREQ, true);
#endif

#ifdef ENABLE_DUAL_GENERATOR
    // DUAL GENERATOR MODE: ATOMIC ACQUISITION - both must succeed or entire operation fails
    
    // Step 1: Try to acquire first generator
    if(!common_begin(time, _fixed_freq)) {
        return false;  // Failed to get first generator
    }
    
    // Step 2: Try to acquire second generator
    if(!acquire_second_generator()) {
        // CRITICAL: Release first generator since we failed to get both
        end();  // This releases the first generator
        return false;  // Failed to get second generator
    }
    
    // Start pager transmission with repeat enabled
    _pager.start_pager_transmission(true);

    // Initialize both generators to silent
    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
    WaveGen *wavegen_b = _wave_gen_pool->access_realizer(_realizer_b);

    // Initialize first generator channels to silent
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
    
    // Initialize second generator channels to silent
    wavegen_b->set_frequency(SILENT_FREQ, false);
    wavegen_b->set_frequency(SILENT_FREQ, true);
#endif

    return true;
}

void SimPager2::realize()
{
    if(!check_frequency_bounds()) {
        return;  // Out of audible range
    }
    
#ifdef ENABLE_FIRST_GENERATOR
    // FIRST GENERATOR ONLY MODE
    // Don't try to access wave generator if we don't have one (during silence)
    if(_realizer == -1) {
        return;
    }
    
    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
    
    if(_active) {
        // Set frequencies based on current pager state
        switch(_pager.get_current_state()) {
            case PAGER_STATE_TONE_A:
                // Transmit Tone A (longer unsquelch tone)
                wavegen->set_frequency(_frequency + _current_tone_a_offset, true);
                wavegen->set_frequency(_frequency + _current_tone_a_offset, false);
                break;
                
            case PAGER_STATE_TONE_B:
                // Transmit Tone B (shorter identification tone)
                wavegen->set_frequency(_frequency + _current_tone_b_offset, true);
                wavegen->set_frequency(_frequency + _current_tone_b_offset, false);
                break;
                  default:
                // Silent state (SILENCE) - should not reach here when _active is true
                wavegen->set_frequency(SILENT_FREQ, true);
                wavegen->set_frequency(SILENT_FREQ, false);
                break;
        }    } else {
        // Explicitly set silent frequencies when inactive (during SILENCE)
        wavegen->set_frequency(SILENT_FREQ, true);
        wavegen->set_frequency(SILENT_FREQ, false);
    }
    
    wavegen->set_active_frequency(_active);
#endif

#ifdef ENABLE_SECOND_GENERATOR
    // SECOND GENERATOR ONLY MODE
    // Use second generator with its own tone offsets
    if(_realizer_b == -1) {
        return;
    }
    
    WaveGen *wavegen_b = _wave_gen_pool->access_realizer(_realizer_b);
    
    if(_active) {
        // Set frequencies based on current pager state - using second generator tone offsets
        switch(_pager.get_current_state()) {
            case PAGER_STATE_TONE_A:
                // Transmit Tone A (longer unsquelch tone) - second generator frequencies
                wavegen_b->set_frequency(_frequency + _current_tone_a_offset_b, true);
                wavegen_b->set_frequency(_frequency + _current_tone_a_offset_b, false);
                break;
                
            case PAGER_STATE_TONE_B:
                // Transmit Tone B (shorter identification tone) - second generator frequencies
                wavegen_b->set_frequency(_frequency + _current_tone_b_offset_b, true);
                wavegen_b->set_frequency(_frequency + _current_tone_b_offset_b, false);
                break;
                  default:
                // Silent state (SILENCE) - should not reach here when _active is true
                wavegen_b->set_frequency(SILENT_FREQ, true);
                wavegen_b->set_frequency(SILENT_FREQ, false);
                break;
        }    } else {
        // Explicitly set silent frequencies when inactive (during SILENCE)
        wavegen_b->set_frequency(SILENT_FREQ, true);
        wavegen_b->set_frequency(SILENT_FREQ, false);
    }
    
    wavegen_b->set_active_frequency(_active);
#endif

#ifdef ENABLE_DUAL_GENERATOR
    // DUAL GENERATOR MODE: Control both generators simultaneously
    // Ensure we have both generators before proceeding
    if(_realizer == -1 || _realizer_b == -1) {
        return;
    }
    
    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
    WaveGen *wavegen_b = _wave_gen_pool->access_realizer(_realizer_b);
    
    if(_active) {
        // Set frequencies for BOTH generators based on current pager state
        switch(_pager.get_current_state()) {
            case PAGER_STATE_TONE_A:
                // FIRST GENERATOR: Transmit Tone A
                wavegen->set_frequency(_frequency + _current_tone_a_offset, true);
                wavegen->set_frequency(_frequency + _current_tone_a_offset, false);
                
                // SECOND GENERATOR: Transmit its own Tone A (different frequency)
                wavegen_b->set_frequency(_frequency + _current_tone_a_offset_b, true);
                wavegen_b->set_frequency(_frequency + _current_tone_a_offset_b, false);
                break;
                
            case PAGER_STATE_TONE_B:
                // FIRST GENERATOR: Transmit Tone B
                wavegen->set_frequency(_frequency + _current_tone_b_offset, true);
                wavegen->set_frequency(_frequency + _current_tone_b_offset, false);
                
                // SECOND GENERATOR: Transmit its own Tone B (different frequency)
                wavegen_b->set_frequency(_frequency + _current_tone_b_offset_b, true);
                wavegen_b->set_frequency(_frequency + _current_tone_b_offset_b, false);
                break;
                  default:
                // Silent state for both generators
                wavegen->set_frequency(SILENT_FREQ, true);
                wavegen->set_frequency(SILENT_FREQ, false);
                wavegen_b->set_frequency(SILENT_FREQ, true);
                wavegen_b->set_frequency(SILENT_FREQ, false);
                break;
        }    } else {
        // Both generators silent when inactive
        wavegen->set_frequency(SILENT_FREQ, true);
        wavegen->set_frequency(SILENT_FREQ, false);
        wavegen_b->set_frequency(SILENT_FREQ, true);
        wavegen_b->set_frequency(SILENT_FREQ, false);
    }
    
    // Activate/deactivate both generators together
    wavegen->set_active_frequency(_active);
    wavegen_b->set_active_frequency(_active);
#endif
}

bool SimPager2::update(Mode *mode)
{
    common_frequency_update(mode);

    if(_enabled) {
        // Note: We don't set frequencies here like RTTY does
        // Pager frequencies are set in realize() based on current tone
    }

    realize();
    return true;
}

bool SimPager2::step(unsigned long time)
{
    switch(_pager.step_pager(time)) {        case STEP_PAGER_TURN_ON:
            // Check if this is the start of a new page cycle (silence → tone A)
            if (_pager.get_current_state() == PAGER_STATE_TONE_A) {
                generate_new_tone_pair();
                
#ifdef ENABLE_FIRST_GENERATOR
                // FIRST GENERATOR ONLY: Acquire wave generator after silent period
                if(_realizer == -1) {
                    if(!common_begin(time, _fixed_freq)) {
                        // Failed to get wave generator - stay inactive
                        _active = false;
                        return true;
                    }
                    // CRITICAL: Force frequency update after reacquiring generator
                    force_frequency_update();
                }
#endif

#ifdef ENABLE_SECOND_GENERATOR
                // SECOND GENERATOR ONLY: Acquire second generator after silent period
                if(_realizer_b == -1) {
                    if(!acquire_second_generator()) {
                        // Failed to get second generator - stay inactive
                        _active = false;
                        return true;
                    }
                    // CRITICAL: Force frequency update after reacquiring generator
                    force_frequency_update();
                }
#endif

#ifdef ENABLE_DUAL_GENERATOR
                // DUAL GENERATOR MODE: ATOMIC REACQUISITION - both must succeed
                bool need_first = (_realizer == -1);
                bool need_second = (_realizer_b == -1);
                
                if (need_first || need_second) {
                    // Try to acquire first generator if needed
                    if (need_first && !common_begin(time, _fixed_freq)) {
                        _active = false;
                        return true;
                    }
                    
                    // Try to acquire second generator if needed
                    if (need_second && !acquire_second_generator()) {
                        // Release first generator if we just acquired it
                        if (need_first) {
                            end();
                        }
                        _active = false;
                        return true;
                    }
                    
                    // CRITICAL: Force frequency update after reacquiring generators
                    force_frequency_update();
                }
#endif
            }
            _active = true;
            realize();
            send_carrier_charge_pulse(_signal_meter);  // Send charge pulse when carrier turns on
            break;

        case STEP_PAGER_LEAVE_ON:
            // Carrier remains on - send another charge pulse
            send_carrier_charge_pulse(_signal_meter);
            break;

        case STEP_PAGER_TURN_OFF:
            _active = false;
            realize();
            
#ifdef ENABLE_FIRST_GENERATOR
            // FIRST GENERATOR ONLY: Release wave generator during silent period
            if(_realizer != -1) {
                WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
                wavegen->set_frequency(SILENT_FREQ, true);
                wavegen->set_frequency(SILENT_FREQ, false);
                wavegen->set_active_frequency(false);
            }
            // Release the resource for other stations to use during silence
            end();  // This calls Realization::end() which frees the realizer
#endif

#ifdef ENABLE_SECOND_GENERATOR
            // SECOND GENERATOR ONLY: Release second generator during silent period
            silence_second_generator();
            release_second_generator();
#endif

#ifdef ENABLE_DUAL_GENERATOR
            // DUAL GENERATOR MODE: Release both generators during silent period
            
            // Silence both generators first
            if(_realizer != -1) {
                WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
                wavegen->set_frequency(SILENT_FREQ, true);
                wavegen->set_frequency(SILENT_FREQ, false);
                wavegen->set_active_frequency(false);
            }
            
            silence_second_generator();
            
            // Release both generators
            end();  // Release first generator
            release_second_generator();  // Release second generator
#endif
            
            // No charge pulse when carrier turns off
            break;
              case STEP_PAGER_CHANGE_FREQ:
            // Transmitter stays on, but frequency needs to change
            realize();
            send_carrier_charge_pulse(_signal_meter);  // Send charge pulse on frequency change while on
            break;
            
        // LEAVE_ON and LEAVE_OFF don't require action since _active state doesn't change
        // and no frequency update is needed during silence
    }

    return true;
}

void SimPager2::generate_new_tone_pair()
{
    // Generate DTMF digit pairs for both generators
    generate_dtmf_digit();
}

void SimPager2::generate_dtmf_digit()
{
    // DTMF row and column frequency arrays
    static const float dtmf_rows[] = {DTMF_ROW_1, DTMF_ROW_2, DTMF_ROW_3, DTMF_ROW_4};
    static const float dtmf_cols[] = {DTMF_COL_1, DTMF_COL_2, DTMF_COL_3, DTMF_COL_4};
    
    // DTMF digit lookup table
    static const char dtmf_digits[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'}, 
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    
    // Generate first generator's DTMF digit (row frequency + column frequency)
    int row1 = random(4);  // Select random row (0-3)
    int col1 = random(4);  // Select random column (0-3)
    
    _current_tone_a_offset = dtmf_rows[row1];    // Row frequency for tone A
    _current_tone_b_offset = dtmf_cols[col1];    // Column frequency for tone B
    _current_dtmf_digit_1 = dtmf_digits[row1][col1];

#if defined(ENABLE_SECOND_GENERATOR) || defined(ENABLE_DUAL_GENERATOR)
    // Generate second generator's DTMF digit (column frequency + row frequency - reversed!)
    int row2 = random(4);  // Select random row (0-3)  
    int col2 = random(4);  // Select random column (0-3)
    
    // Reverse the assignment: second generator uses column for A, row for B
    _current_tone_a_offset_b = dtmf_cols[col2]; // Column frequency for tone A
    _current_tone_b_offset_b = dtmf_rows[row2]; // Row frequency for tone B  
    _current_dtmf_digit_2 = dtmf_digits[row2][col2];
#endif
}

void SimPager2::debug_print_tone_pair() const
{
    // Debug output not needed for Arduino build
}

void SimPager2::debug_test_dual_generator_acquisition()
{
    // DEBUG: Test if we can acquire both generators simultaneously
    
    // Test: Try to acquire first generator if we don't have it
    bool first_acquired = false;
    int original_first_realizer = _realizer;
    
    if (_realizer == -1) {
        if (common_begin(millis(), _fixed_freq)) {
            first_acquired = true;
        }
    } else {
        first_acquired = true;
    }
    
#if defined(ENABLE_SECOND_GENERATOR) || defined(ENABLE_DUAL_GENERATOR)
    // Test: Try to acquire second generator if we don't have it
    bool second_acquired = false;
    int original_second_realizer = _realizer_b;
    
    if (_realizer_b == -1) {
        if (acquire_second_generator()) {
            second_acquired = true;
        }
    } else {
        second_acquired = true;
    }
    
    // Cleanup: Release any generators we acquired during test
    if (first_acquired && original_first_realizer == -1) {
        end(); // This releases the first generator
    }
    
    if (second_acquired && original_second_realizer == -1) {
        release_second_generator();
    }
#endif
}

void SimPager2::end()
{
    // Call parent class end method
    SimTransmitter::end();
}

// SECOND GENERATOR HELPER METHODS
#if defined(ENABLE_SECOND_GENERATOR) || defined(ENABLE_DUAL_GENERATOR)
bool SimPager2::acquire_second_generator()
{
    if (_realizer_b != -1) {
        return true;  // Already have one
    }
    
    _realizer_b = _wave_gen_pool->get_realizer(_station_id);
    return (_realizer_b != -1);
}

void SimPager2::release_second_generator()
{
    if (_realizer_b != -1) {
        _wave_gen_pool->free_realizer(_realizer_b, _station_id);
        _realizer_b = -1;
    }
}

void SimPager2::silence_second_generator()
{
    if (_realizer_b != -1) {
        WaveGen *wavegen_b = _wave_gen_pool->access_realizer(_realizer_b);
        if (wavegen_b) {
            wavegen_b->set_frequency(SILENT_FREQ, true);
            wavegen_b->set_frequency(SILENT_FREQ, false);
            wavegen_b->set_active_frequency(false);
        }
    }
}
#endif
