#ifdef NATIVE_BUILD
#include "../native/platform.h"
#else
#include <MD_AD9833.h>
#endif
#include "wavegen.h"
#include "vfo.h"
#include "buffers.h"
#include "signal_meter.h"
#include "station_config.h"

VFO::VFO(const char *title, float frequency, unsigned long step, RealizationPool *realization_pool) : Mode(title)
{
    _frequency = long(frequency);
    _sub_frequency = int((frequency - _frequency) * 10.0);
    _step = step;
    _realization_pool = realization_pool;
}

// step needs to be in 0.1Hz units
// when step is 0.1Hz, use xxxxxxx.y format
void VFO::update_display(HT16K33Disp *display){
    if(_frequency >= 10000000L){
        // Display as 2450.0000 in MHz
        int megpart = _frequency / 1000000L;
        long decpart = _frequency - (megpart * 1000000L);
        int decparti = decpart / 100L;
        
        sprintf(display_text_buffer, "%4d.%04d", megpart, decparti);
        
    } else if(_frequency >= 1000000L) {
        // Display 7,015,089 as 7015.089 in KHz
        int megpart = _frequency / 1000000L;
        long remainder = _frequency - (megpart * 1000000L);
        int kilpart = remainder / 1000L;
        remainder = remainder - (kilpart * 1000L);
        int unipart = remainder;
        
        sprintf(display_text_buffer, " %1d.%03d.%03d", megpart, kilpart, unipart);
        
    } else {
        // Display in Hz
        sprintf(display_text_buffer, "%8ld", _frequency);    }

#ifndef DISABLE_DISPLAY_OPERATIONS
    display->show_string(display_text_buffer);
#endif
}

void VFO::update_realization(){
    _realization_pool->update(this);
    // for(byte i = 0; i < _nrealizations; i++){
    //     _realizations[i]->update(this);
    // }

    // float ffrequency = float(_frequency) + (_sub_frequency / 10.0);

    // // WaveGen *wavegen = (WaveGen *)realization;
	// wavegen->_sig_gen->setFrequency((MD_AD9833::channel_t)0, float(_frequency));
}

void VFO::force_transmitter_refresh(){
    // Force hardware refresh for all SimTransmitter objects
    // This ensures wave generators are properly updated when switching to SimRadio
    _realization_pool->force_sim_transmitter_refresh();
}

void VFO::mark_hardware_dirty(){
    // Mark hardware state as unknown - will trigger refresh on next update
    _realization_pool->mark_dirty();
}

void VFO::update_signal_meter(SignalMeter *signal_meter) {
    // Calculate signal strength based on proximity to active stations
    // For now, use a simple calculation based on station frequencies    // Known station frequencies (based on configuration)
    const float station_frequencies[] = {
#if defined(ENABLE_FOUR_CW_STATIONS)
        7002000.0,  // CW station 1
        7003000.0,  // CW station 2
        7004000.0,  // CW station 3
        7005000.0   // CW station 4
#elif defined(ENABLE_FOUR_NUMBERS_STATIONS)
        7002700.0,  // Numbers station 1
        7003700.0,  // Numbers station 2
        7004700.0,  // Numbers station 3
        7005700.0   // Numbers station 4
#elif defined(ENABLE_FOUR_PAGER_STATIONS)
        7006000.0,  // Pager station 1
        7007000.0,  // Pager station 2
        7008000.0,  // Pager station 3
        7009000.0   // Pager station 4
#elif defined(ENABLE_FOUR_RTTY_STATIONS)
        7004100.0,  // RTTY station 1
        7005100.0,  // RTTY station 2
        7006100.0,  // RTTY station 3
        7007100.0   // RTTY station 4
#else
        // Default mixed or minimal configuration
#ifdef ENABLE_MORSE_STATION
        7002000.0,  // CW station
#endif
#ifdef ENABLE_NUMBERS_STATION
        7002700.0,  // Numbers station  
#endif
#ifdef ENABLE_PAGER_STATION
        7006000.0,  // Pager station
#endif
#ifdef ENABLE_RTTY_STATION
        7004100.0   // RTTY station
#endif
#endif
    };
    const int num_stations = sizeof(station_frequencies) / sizeof(station_frequencies[0]);
    
    float vfo_freq = float(_frequency) + (_sub_frequency / 10.0);
    int max_strength = 0;    // Find the strongest signal based on frequency proximity
    for (int i = 0; i < num_stations; i++) {
        float freq_diff = abs(vfo_freq - station_frequencies[i]);
        
        // Signal strength calculation:
        // - Maximum strength (255) when exactly tuned (freq_diff = 0)
        // - Strength decreases with distance
        // - Audible range is roughly ±2500 Hz for typical receivers
        
        int strength = 0;
        if (freq_diff <= 2500.0) {
            // Linear falloff within audible range
            strength = (int)(255.0 * (1.0 - (freq_diff / 2500.0)));
        }
        
        if (strength > max_strength) {
            max_strength = strength;
        }
    }
    
    signal_meter->update_signal_strength(max_strength);
}
