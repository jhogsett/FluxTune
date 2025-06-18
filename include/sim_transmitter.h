#ifndef __SIM_TRANSMITTER_H__
#define __SIM_TRANSMITTER_H__

#include "realization.h"
#include "realizer_pool.h"
#include "mode.h"

// Common frequency constraints for all simulated transmitters
#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SILENT_FREQ 0.1

/**
 * Base class for all simulated transmitting stations.
 * Provides common infrastructure for frequency management, WaveGen control,
 * and audible range handling shared by all digital mode simulators.
 */
class SimTransmitter : public Realization
{
public:
    SimTransmitter(RealizerPool *realizer_pool);
    
    // Common interface for all transmitters
    virtual bool begin_transmission(unsigned long time, float fixed_freq);
    virtual bool update(Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    
    // Template method for frequency realization - derived classes can override
    virtual void realize();
    
    // Hook methods for derived classes to implement mode-specific behavior
    virtual void setup_digital_mode() = 0;           // Initialize the digital mode (Morse, RTTY, etc.)
    virtual void step_digital_mode(unsigned long time) = 0;  // Update digital mode state
    virtual bool is_transmitting() = 0;              // Check if currently transmitting
    virtual void apply_frequencies_to_wavegen(class WaveGen *wavegen) = 0; // Set mode-specific frequencies

protected:
    // Common state shared by all transmitters
    float _fixed_freq;       // Target frequency for this station
    float _frequency;        // Current frequency difference from VFO
    bool _enabled;           // True when frequency is in audible range
    bool _active;            // True when actively transmitting
    int _phase;              // General-purpose phase/state counter
    
    // Helper methods for common operations
    WaveGen* get_wavegen();
    bool is_frequency_audible() const;
    void set_silent();
    void set_enabled();
};

#endif
