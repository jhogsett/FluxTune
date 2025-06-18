#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "sim_transmitter.h"
#include "async_morse.h"

#define SPACE_FREQUENCY 0.1
#define WAIT_SECONDS 2

class SimStation : public SimTransmitter
{
public:
    SimStation(RealizerPool *realizer_pool);
    
    bool begin(unsigned long time, float fixed_freq, const char *message, int wpm);
    virtual bool update(class Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    
    // Override restart to restore station parameters
    virtual bool try_restart(unsigned long time);    void set_message(const char *message, int wpm);
    
    // Simple diagnostic mode: increment WPM and frequency after each cycle
    void enable_cycle_diagnostics(bool enable = true) { _cycle_diagnostics = enable; }

private:
    // Keep remaining data members for now (removing _enabled as it's now inherited)
    AsyncMorse _morse;
    const char *_message;
    int _wpm;
    bool _message_set;
    
    // Store initialization parameters for restart
    float _init_fixed_freq;
    const char *_init_message;
    int _init_wpm;
    bool _init_params_set;      // Track transmission cycles for auto-stopping
    bool _transmission_complete;
    unsigned long _last_cycle_time;  // Per-instance tracking    // Simple cycle diagnostics (RAM-efficient)
    bool _cycle_diagnostics;
    
    void check_transmission_cycle();
    // Diagnostic methods removed due to RAM constraints
    // char _diagnostic_message[120];
    // bool _use_diagnostic_message;
    // void build_diagnostic_message();
};

#endif
