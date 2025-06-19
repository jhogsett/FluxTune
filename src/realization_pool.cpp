

#include "basic_types.h"
#include "realization.h"
#include "realization_pool.h"
#include "sim_transmitter.h"  // For dynamic_cast to update wave generators

// pass array of realizer addresses, array of free/in-use bools, count of realizers 
RealizationPool::RealizationPool(Realization **realizations, bool *statuses,  int nrealizations){
    _realizations = realizations;
    _statuses = statuses;
    _nrealizations = nrealizations; 

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
    
    // After updating frequency calculations, ensure wave generators are updated
    // This is critical when returning to SimRadio from other applications
    for(byte i = 0; i < _nrealizations; i++){
        // Check if this is a SimTransmitter (SimStation, SimPager, SimRTTY)
        SimTransmitter *sim_tx = dynamic_cast<SimTransmitter*>(_realizations[i]);
        if(sim_tx != nullptr) {
            sim_tx->realize();  // Update wave generators with new frequencies
        }
    }
}
