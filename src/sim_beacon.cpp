#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_beacon.h"

// mode is expected to be a derivative of VFO
SimBeacon::SimBeacon(RealizerPool *realizer_pool) : Realization(realizer_pool){
    _pattern_type = BEACON_PATTERN_SIMPLE;
    _on_time_ms = 1000;
    _off_time_ms = 1000;
    _beacon_set = false;
}

bool SimBeacon::begin(unsigned long time, float frequency, uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms) {
    set_frequency(frequency);
    set_pattern(pattern_type, on_time_ms, off_time_ms);
    _beacon.start_beacon(_pattern_type, _on_time_ms, _off_time_ms, true);
    return true;
}

void SimBeacon::set_pattern(uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms) {
    _pattern_type = pattern_type;
    _on_time_ms = on_time_ms;
    _off_time_ms = off_time_ms;
    _beacon_set = true;
}

bool SimBeacon::step(unsigned long time) {
    if (!_beacon_set) {
        return false;
    }
    
    int beacon_result = _beacon.step_beacon(time);
    bool is_transmitting = (beacon_result == STEP_BEACON_TURN_ON || beacon_result == STEP_BEACON_LEAVE_ON);
    
    // Update the audio output based on beacon state
    if (is_transmitting) {
        set_audio_frequency(get_audible_frequency());
    } else {
        set_audio_frequency(SILENT_FREQ);
    }
    
    return true;
}

void SimBeacon::end() {
    set_audio_frequency(SILENT_FREQ);
    _beacon_set = false;
}

// returns true on successful update
bool SimBeacon::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    float raw_frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    float audible_frequency = abs(raw_frequency - 7010000.0);
    
    set_frequency(audible_frequency);
    return true;
}
