

#include "basic_types.h"
#include "realizer_pool.h"

// pass array of wave generator addresses, array of free/in-use bools, count of wave generators 
WaveGenPool::WaveGenPool(WaveGen **wavegens, bool *statuses,  int nwavegens){
    _realizers = wavegens;
    _statuses = statuses;
    _nrealizers = nwavegens;

    for(int i = 0; i < _nrealizers; i++){
        free_realizer(i);
    }   
}

int WaveGenPool::get_realizer(){
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
void WaveGenPool::free_realizer(int nrealizer){
    _statuses[nrealizer] = false;

    // Serial.print("Free Realizer ");
    // Serial.println(nrealizer);

}

WaveGen * WaveGenPool::access_realizer(int nrealizer){

    // Serial.print("Access Realizer ");
    // Serial.println(nrealizer);

    return _realizers[nrealizer];
}
