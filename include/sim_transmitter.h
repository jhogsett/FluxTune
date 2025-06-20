#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

// Station states for dynamic station management
enum StationState {
    DORMANT,     // No frequency assigned, minimal memory usage
    ACTIVE,      // Frequency assigned, tracking VFO proximity  
    AUDIBLE,     // Active + has AD9833 generator assigned
    SILENT       // Active but no AD9833 (>4 stations in range)
};

// Common constants for simulated transmitters
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

// BFO (Beat Frequency Oscillator) offset for comfortable audio tuning
// This shifts the audio frequency without affecting signal meter calculations
// Now dynamically adjustable via option_bfo_offset (0-2000 Hz)
// #define BFO_OFFSET 700.0   // Replaced by dynamic option_bfo_offset

/**
 * Base class for simulated transmitters (CW/RTTY).
 * Provides common functionality and interface for station simulation.
 */
class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);
    
    virtual bool step(unsigned long time) = 0;  // Pure virtual - must be implemented by derived classes
    virtual void end();  // Common cleanup logic
    virtual void force_wave_generator_refresh() override;  // Override base class method

    // Dynamic station management methods
    virtual bool reinitialize(unsigned long time, float fixed_freq);  // Reinitialize with new frequency
    void set_station_state(StationState new_state);  // Change station state
    StationState get_station_state() const;  // Get current station state
    bool is_audible() const;  // True if station has AD9833 generator assigned
    float get_fixed_frequency() const;  // Get station's target frequency
    void setActive(bool active);
    bool isActive() const;

#ifdef NATIVE_BUILD
    // Test-only method to set VFO frequency directly
    void set_vfo_frequency_for_test(float vfo_freq) { _vfo_freq = vfo_freq; }
#endif

protected:    // Common utility methods
    bool check_frequency_bounds();  // Returns true if frequency is in audible range
    bool common_begin(unsigned long time, float fixed_freq);  // Common initialization logic
    void common_frequency_update(Mode *mode);  // Common frequency calculation (mode must be VFO)    // Common member variables
    float _fixed_freq;  // Target frequency for this station
    bool _enabled;      // True when frequency is in audible range
    float _frequency;   // Current frequency difference from VFO
    float _vfo_freq;    // Current VFO frequency (for signal meter charge calculation)
    bool _active;       // True when transmitter should be active
    
    // Dynamic station management state
    StationState _station_state;  // Current state in dynamic management system
};

#endif