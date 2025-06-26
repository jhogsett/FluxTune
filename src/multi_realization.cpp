#include "multi_realization.h"

MultiRealization::MultiRealization(WaveGenPool *wave_gen_pool, int generator_count, int station_id)
    : Realization(wave_gen_pool, station_id), _generator_count(generator_count), _allocated_count(0)
{
    // Initialize all indices to -1 (not allocated)
    for (int i = 0; i < MAX_GENERATORS_PER_STATION; i++) {
        _realizer_indices[i] = -1;
    }
    
    // Set the primary _realizer to -1 to indicate multi-generator mode
    _realizer = -1;
}

bool MultiRealization::begin(unsigned long time) {
    // If already have all generators, begin() is idempotent - just return success
    if (has_all_generators()) {
        return true;
    }
    
    // Attempt to acquire all required generators atomically
    _allocated_count = _wave_gen_pool->get_multiple_realizers(
        _generator_count, _realizer_indices, _station_id);
    
    if (_allocated_count == _generator_count) {
        // Success - set primary _realizer to first allocated generator for compatibility
        _realizer = _realizer_indices[0];
        return true;
    } else {
        // Failed to get all generators - make sure none are allocated
        if (_allocated_count > 0) {
            _wave_gen_pool->free_multiple_realizers(_allocated_count, _realizer_indices, _station_id);
            _allocated_count = 0;
        }
        _realizer = -1;
        return false;
    }
}

void MultiRealization::end() {
    if (_allocated_count > 0) {
        _wave_gen_pool->free_multiple_realizers(_allocated_count, _realizer_indices, _station_id);
        _allocated_count = 0;
        _realizer = -1;
        
        // Reset all indices
        for (int i = 0; i < MAX_GENERATORS_PER_STATION; i++) {
            _realizer_indices[i] = -1;
        }
    }
}

WaveGen* MultiRealization::access_generator(int index) {
    if (index >= 0 && index < _generator_count && index < _allocated_count) {
        return _wave_gen_pool->access_realizer(_realizer_indices[index]);
    }
    return nullptr;
}
