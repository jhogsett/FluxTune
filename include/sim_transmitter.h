#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"

// Common constants for simulated transmitters
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

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

protected:    // Common utility methods
    bool check_frequency_bounds();  // Returns true if frequency is in audible range
    bool common_begin(unsigned long time, float fixed_freq);  // Common initialization logic
    void common_frequency_update(Mode *mode);  // Common frequency calculation (mode must be VFO)
    
#ifdef PLATFORM_NATIVE
    // Test-only method to set VFO frequency directly
    void set_vfo_frequency_for_test(float vfo_freq) { _vfo_freq = vfo_freq; }
#endif

    // Common member variables
    float _fixed_freq;  // Target frequency for this station
    bool _enabled;      // True when frequency is in audible range
    float _frequency;   // Current frequency difference from VFO
    float _vfo_freq;    // Current VFO frequency (for signal meter charge calculation)
    bool _active;       // True when transmitter should be active
};

#endif