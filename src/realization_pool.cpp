#include "basic_types.h"
#include "realization.h"
#include "realization_pool.h"

// pass array of realizer addresses, array of free/in-use bools, count of realizers 
RealizationPool::RealizationPool(Realization **realizations, bool *statuses,  int nrealizations){
    _realizations = realizations;
    _statuses = statuses;
    _nrealizations = nrealizations; 
    _hardware_dirty = false;  // Initialize as clean

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
// void WaveGenPool::free_realizer(int nrealizer){
//     _statuses[nrealizer] = false;

//     // Serial.print("Free Realizer ");
//     // Serial.println(nrealizer);

// }

// Realizer * WaveGenPool::access_realizer(int nrealizer){

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
