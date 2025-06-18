#include "mode.h"
#include "realizer_pool.h"
#include "realization.h"

Realization::Realization(RealizerPool *realizer_pool){
    _realizer_pool = realizer_pool;
    _realizer = -1;
    
    // Initialize lifecycle management
    _max_runs = 0;           // 0 = unlimited runs (current behavior)
    _current_runs = 0;
    _auto_lifecycle = false; // Disabled by default for backward compatibility
    _needs_restart = false;
    _restart_delay_ms = 5000; // 5 second delay between restart attempts
    _last_restart_attempt = 0;
}

// returns true on successful update
bool Realization::update(Mode *mode){
    return false;
}

// returns true on successful begin
bool Realization::begin(unsigned long time){
    // attempt to acquire a realizer
    _realizer = _realizer_pool->get_realizer();
    if(_realizer == -1)
        return false;
    return true;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool Realization::step(unsigned long time){
    // Check for automatic restart if needed
    if (_needs_restart) {
        if (time - _last_restart_attempt >= _restart_delay_ms) {
            _last_restart_attempt = time;
            if (try_restart(time)) {
                _needs_restart = false;
            }
        }
    }      // Check for automatic stopping
    if (_auto_lifecycle && is_active() && should_auto_stop()) {
        // Debug output disabled to prevent Arduino instability
        // #ifdef NATIVE_BUILD
        //     printf("Station stopping after %d runs (limit: %d)\n", _current_runs, _max_runs);
        // #else
        //     Serial.print("Station stopping after ");
        //     Serial.print(_current_runs);
        //     Serial.print(" runs (limit: ");
        //     Serial.print(_max_runs);
        //     Serial.println(")");
        // #endif
        end();
        _needs_restart = true; // Mark for restart
        return false; // Signal that this realization is done for now
    }
    
    return true;
}

// Lifecycle management methods
void Realization::set_run_limit(int max_runs) {
    _max_runs = max_runs;
    _auto_lifecycle = (max_runs > 0); // Enable auto lifecycle when limit is set
}

bool Realization::should_auto_stop() {
    return _auto_lifecycle && _max_runs > 0 && _current_runs >= _max_runs;
}

bool Realization::is_active() {
    return _realizer != -1;
}

bool Realization::needs_restart() {
    return _needs_restart;
}

// Default restart implementation - can be overridden by derived classes
bool Realization::try_restart(unsigned long time) {
    if (!is_active()) {
        _current_runs = 0; // Reset run count for new cycle
        // Debug output disabled to prevent Arduino instability
        // #ifdef NATIVE_BUILD
        //     printf("Station restarting (run count reset)\n");
        // #else
        //     Serial.println("Station restarting (run count reset)");
        // #endif
        return begin(time);
    }
    return false;
}

// void Realization::internal_step(unsigned long time){
// }

void Realization::end(){
    if(_realizer != -1) {
        _realizer_pool->free_realizer(_realizer);
        _realizer = -1;
    }
}
