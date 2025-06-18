#ifndef __REALIZATION_H__
#define __REALIZATION_H__

#include "mode.h"
#include "realizer_pool.h"

// handles realization using a realizer
// 

class Mode;

class Realization
{
public:
    Realization(RealizerPool *realizer_pool);

    virtual bool update(Mode *mode);

    virtual bool begin(unsigned long time);
    virtual bool step(unsigned long time);
    virtual void end();    // Automatic lifecycle management
    void set_run_limit(int max_runs);
    bool should_auto_stop();
    bool is_active();
    bool needs_restart();
    virtual bool try_restart(unsigned long time);
    
    // Diagnostic methods
    int get_current_runs() { return _current_runs; }
    int get_max_runs() { return _max_runs; }
    bool get_auto_lifecycle() { return _auto_lifecycle; }

    RealizerPool *_realizer_pool;
    int _realizer;

protected:
    // Lifecycle state
    int _max_runs;           // How many times to run before auto-stopping (0 = unlimited)
    int _current_runs;       // How many times it has run
    bool _auto_lifecycle;    // Enable automatic start/stop management
    bool _needs_restart;     // True when stopped and should restart when possible
    unsigned long _restart_delay_ms;  // Minimum time between restart attempts
    unsigned long _last_restart_attempt; // When we last tried to restart
};

#endif
