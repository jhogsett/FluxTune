#include "basic_types.h"
#include "wave_gen_pool.h"

// pass array of wave generator addresses, array of free/in-use bools, count of wave generators 
WaveGenPool::WaveGenPool(WaveGen **wavegens, bool *statuses,  int nwavegens){
    _realizers = wavegens;
    _statuses = statuses;
    _nrealizers = nwavegens;

    for(int i = 0; i < _nrealizers; i++){
        free_realizer(i, 0);  // Initialize with station_id 0
    }   
}

int WaveGenPool::get_realizer(int station_id){
    for(int i = 0; i < _nrealizers; i++){
        if(!_statuses[i]){
            _statuses[i] = true;
            return i;
        }
    }
    return -1;
}

// multiplely gotten realizers must be freed individually
void WaveGenPool::free_realizer(int nrealizer, int station_id){
    _statuses[nrealizer] = false;
}

WaveGen * WaveGenPool::access_realizer(int nrealizer){
    return _realizers[nrealizer];
}

int WaveGenPool::get_available_count(){
    int available = 0;
    for(int i = 0; i < _nrealizers; i++){
        if(!_statuses[i]){
            available++;
        }
    }
    return available;
}

// Allocate multiple wave generators atomically (all-or-nothing)
// Returns number of generators allocated (0 if failed, count if successful)
int WaveGenPool::get_multiple_realizers(int count, int* allocated_indices, int station_id) {
    // First, check if we have enough available generators
    if (get_available_count() < count) {
        return 0;  // Not enough available
    }
    
    // Find the required number of free generators
    int found = 0;
    for (int i = 0; i < _nrealizers && found < count; i++) {
        if (!_statuses[i]) {
            allocated_indices[found] = i;
            found++;
        }
    }
    
    // If we couldn't find enough, fail
    if (found < count) {
        return 0;
    }
    
    // Allocate all the found generators
    for (int i = 0; i < count; i++) {
        _statuses[allocated_indices[i]] = true;
    }
    
    return count;  // Success
}

// Free multiple wave generators at once
void WaveGenPool::free_multiple_realizers(int count, int* indices, int station_id) {
    for (int i = 0; i < count; i++) {
        if (indices[i] >= 0 && indices[i] < _nrealizers) {
            _statuses[indices[i]] = false;
        }
    }
}
