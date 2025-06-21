#include "basic_types.h"
#include "realization.h"
#include "realization_pool.h"
#include "station_manager.h"

// pass array of realizer addresses, array of free/in-use bools, count of realizers 
RealizationPool::RealizationPool(Realization **realizations, bool *statuses,  int nrealizations){
    _realizations = realizations;
    _statuses = statuses;
    _nrealizations = nrealizations; 
    _hardware_dirty = false;  // Initialize as clean
    
    // Dynamic station management initialization
    _station_manager = nullptr;
    _current_vfo_freq = 0.0f;
    _vfo_freq_changed = false;

    // for(int i = 0; i < _nrealizations; i++){
    //     free_realization(i);
    // }   
}

bool RealizationPool::begin(unsigned long time){
    return true;
}

bool RealizationPool::step(unsigned long time){
    for(byte i = 0; i < _nrealizations; i++){
        if(!_realizations[i]->step(time))
            return false;
    }
    
    // Check if VFO frequency has changed and update StationManager
    if(_vfo_freq_changed && _station_manager != nullptr) {
        // Convert float frequency to uint32_t in Hz for StationManager
        uint32_t vfo_freq_hz = (uint32_t)(_current_vfo_freq * 1000000.0f); // Convert MHz to Hz
        _station_manager->updateStations(vfo_freq_hz);
        _vfo_freq_changed = false; // Clear the flag
    }
    
    return true;
}

void RealizationPool::end(){
}


// int RealizationPool::get_realization(){
//     for(int i = 0; i < _nrealizations; i++){
//         if(!_statuses[i]){
//             _statuses[i] = true;

//             // Serial.print("Get Realizer ");
//             // Serial.println(i);

//             return i;
//         }
//     }
//     // Serial.println("No Free Realizer");
//     return -1;
// }

// // multiplely gotten realizers must be freed individually
// void RealizerPool::free_realizer(int nrealizer){
//     _statuses[nrealizer] = false;

//     // Serial.print("Free Realizer ");
//     // Serial.println(nrealizer);

// }

// Realizer * RealizerPool::access_realizer(int nrealizer){

//     // Serial.print("Access Realizer ");
//     // Serial.println(nrealizer);

//     return _realizers[nrealizer];
// }

void RealizationPool::update(Mode *mode){
    for(byte i = 0; i < _nrealizations; i++){
        _realizations[i]->update(mode);
    }
    
    // If hardware state is dirty (unknown), force a refresh
    if(_hardware_dirty) {
        force_sim_transmitter_refresh();
        _hardware_dirty = false;  // Clear the dirty flag
    }
}

void RealizationPool::force_sim_transmitter_refresh(){
    // Force wave generator hardware refresh for all SimTransmitter objects
    // This is called when switching to SimRadio to ensure wave generators
    // are properly synchronized with their software state
    for(byte i = 0; i < _nrealizations; i++){
        // Use virtual method instead of dynamic_cast for Arduino compatibility
        _realizations[i]->force_wave_generator_refresh();
    }
}

void RealizationPool::mark_dirty(){
    // Mark hardware state as unknown - will trigger refresh on next update
    // This is called when switching applications that may affect hardware state
    _hardware_dirty = true;
}

// Dynamic station management methods
void RealizationPool::set_station_manager(StationManager* station_manager) {
    _station_manager = station_manager;
}

void RealizationPool::set_current_vfo_frequency(float vfo_freq) {
    if (_current_vfo_freq != vfo_freq) {
        _current_vfo_freq = vfo_freq;
        _vfo_freq_changed = true; // Flag that frequency has changed
    }
}
