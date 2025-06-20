#include "sim_transmitter.h"
#include "wavegen.h"
#include "vfo.h"
#include "saved_data.h"  // For option_bfo_offset

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    // Initialize common member variables
    _fixed_freq = 0.0;
    _enabled = false;
    _frequency = 0.0;
    _active = false;
    
    // Initialize dynamic station management state
    _station_state = DORMANT;
}

bool SimTransmitter::common_begin(unsigned long time, float fixed_freq)
{
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    
    return Realization::begin(time);
}

void SimTransmitter::common_frequency_update(Mode *mode)
{
    // Note: mode is expected to be a VFO object
    VFO *vfo = static_cast<VFO*>(mode);
    _vfo_freq = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    
    // Calculate raw frequency difference (used for signal meter - no BFO offset)
    float raw_frequency = _vfo_freq - _fixed_freq;
      // Add BFO offset for comfortable audio tuning
    // This shifts the audio frequency without affecting signal meter calculations
    _frequency = raw_frequency + option_bfo_offset;
}

bool SimTransmitter::check_frequency_bounds()
{
    WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
    
    if(_frequency > MAX_AUDIBLE_FREQ || _frequency < MIN_AUDIBLE_FREQ){
        if(_enabled){
            _enabled = false;
            wavegen->set_frequency(SILENT_FREQ, true);
            wavegen->set_frequency(SILENT_FREQ, false);
        }
        return false;  // Out of bounds
    } 
        
    if(!_enabled){
        _enabled = true;
    }
      return true;  // In bounds
}

void SimTransmitter::end()
{
    if(_realizer != -1) {
        _realizer_pool->free_realizer(_realizer);
        _realizer = -1;  // Reset to avoid double-free or invalid access
    }
}

void SimTransmitter::force_wave_generator_refresh()
{
    // Force wave generator hardware update regardless of cached state
    // This is needed when returning to SimRadio after application switches
    if(_realizer != -1) {
        WaveGen *wavegen = static_cast<WaveGen*>(_realizer_pool->access_realizer(_realizer));
        wavegen->force_refresh();
    }
}

// Dynamic station management methods
bool SimTransmitter::reinitialize(unsigned long time, float fixed_freq)
{
    // Reinitialize station with new frequency for dynamic management
    // This allows reusing dormant stations for new frequencies
    
    // Clean up any existing state
    if(_station_state == AUDIBLE && _realizer != -1) {
        end();  // Release realizer if currently assigned
    }
    
    // Set new parameters
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    _enabled = false;
    _active = false;
    _station_state = ACTIVE;  // Station is now active at new frequency
    
    // Subclasses should override this method to reinitialize their specific content
    // (e.g., new morse messages, different WPM, new pager content, etc.)
    
    return true;
}

void SimTransmitter::set_station_state(StationState new_state)
{
    StationState old_state = _station_state;
    _station_state = new_state;
    
    // Handle state transition logic
    if(old_state == AUDIBLE && new_state != AUDIBLE) {
        // Losing AD9833 generator - release it
        if(_realizer != -1) {
            end();  // This will free the realizer
        }
    }
    // Note: Gaining AD9833 generator (ACTIVE/SILENT -> AUDIBLE) will be handled
    // by the StationManager when it assigns a realizer to this station
}

StationState SimTransmitter::get_station_state() const
{
    return _station_state;
}

bool SimTransmitter::is_audible() const
{
    return _station_state == AUDIBLE;
}

float SimTransmitter::get_fixed_frequency() const
{
    return _fixed_freq;
}