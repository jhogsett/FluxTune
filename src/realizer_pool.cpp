

#include <Arduino.h>
#include "realizer_pool.h"

// pass array of realizer addresses, array of free/in-use bools, count of realizers 
RealizerPool::RealizerPool(Realizer **realizers, bool *statuses,  int nrealizers){
    _realizers = realizers;
    _statuses = statuses;
    _nrealizers = nrealizers; 

    for(int i = 0; i < _nrealizers; i++){
        free_realizer(i);
    }   
}

int RealizerPool::get_realizer(){
    for(int i = 0; i < _nrealizers; i++){
        if(!_statuses[i]){
            _statuses[i] = true;

            // Serial.print("Get Realizer ");
            // Serial.println(i);

            return i;
        }
    }
    // Serial.println("No Free Realizer");
    return -1;
}

// multiplely gotten realizers must be freed individually
void RealizerPool::free_realizer(int nrealizer){
    _statuses[nrealizer] = false;

    // Serial.print("Free Realizer ");
    // Serial.println(nrealizer);

}

Realizer * RealizerPool::access_realizer(int nrealizer){

    // Serial.print("Access Realizer ");
    // Serial.println(nrealizer);

    return _realizers[nrealizer];
}
