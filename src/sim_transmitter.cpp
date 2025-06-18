#include "sim_transmitter.h"
#include "vfo.h"
#include "wavegen.h"

SimTransmitter::SimTransmitter(RealizerPool *realizer_pool) : Realization(realizer_pool)
{
    _fixed_freq = 0.0;
    _frequency = 0.0;
    _enabled = false;
    _active = false;
    _phase = 0;
}

bool SimTransmitter::begin_transmission(unsigned long time, float fixed_freq)
{
    _fixed_freq = fixed_freq;
    _frequency = 0.0;
    
    // Attempt to acquire a realizer from the pool
    if (!Realization::begin(time))
        return false;
    
    // Initialize the WaveGen to silent
    WaveGen *wavegen = get_wavegen();
    wavegen->set_frequency(SILENT_FREQ, false);
    wavegen->set_frequency(SILENT_FREQ, true);
    
    // Let derived class set up their specific digital mode
    setup_digital_mode();
    
    return true;
}

bool SimTransmitter::update(Mode *mode)
{
    // Calculate frequency difference from VFO
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = _frequency - _fixed_freq;
    
    // Update hardware realization first (this sets _enabled based on audible range)
    realize();
    
    return true;
}

bool SimTransmitter::step(unsigned long time)
{
    // Update the digital mode state machine
    step_digital_mode(time);
    
    // Update active transmission state
    _active = is_transmitting();
    
    // Apply the updated transmission state to the hardware
    if (_enabled) {
        realize();
    }
    
    return Realization::step(time);
}

void SimTransmitter::realize()
{
    WaveGen *wavegen = get_wavegen();
    
    // Check if frequency is in audible range
    if (!is_frequency_audible()) {
        if (_enabled) {
            set_silent();
        }
        return;
    }
    
    // Enable if we weren't already enabled
    if (!_enabled) {
        set_enabled();
        // Apply frequencies when first enabled
        apply_frequencies_to_wavegen(wavegen);
    }
    
    // Always update the transmission state (this is the key for digital mode keying)
    wavegen->set_active_frequency(_active);
}

void SimTransmitter::end()
{
    if (_enabled) {
        set_silent();
    }
    Realization::end();
}

// Helper methods
WaveGen* SimTransmitter::get_wavegen()
{
    return (WaveGen*)_realizer_pool->access_realizer(_realizer);
}

bool SimTransmitter::is_frequency_audible() const
{
    // Use absolute value since we care about the magnitude of the frequency difference
    float abs_freq = (_frequency < 0) ? -_frequency : _frequency;
    return (abs_freq <= MAX_AUDIBLE_FREQ && abs_freq >= MIN_AUDIBLE_FREQ);
}

void SimTransmitter::set_silent()
{
    _enabled = false;
    WaveGen *wavegen = get_wavegen();
    wavegen->set_frequency(SILENT_FREQ, true);
    wavegen->set_frequency(SILENT_FREQ, false);
}

void SimTransmitter::set_enabled()
{
    _enabled = true;
}
