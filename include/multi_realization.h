#ifndef __MULTI_REALIZATION_H__
#define __MULTI_REALIZATION_H__

#include "realization.h"

// Maximum number of wave generators a single station can use
#define MAX_GENERATORS_PER_STATION 4

// Base class for stations that require multiple wave generators
class MultiRealization : public Realization
{
public:
    MultiRealization(WaveGenPool *wave_gen_pool, int generator_count, int station_id = 0);

    virtual bool begin(unsigned long time) override;
    virtual void end() override;

    // Access a specific wave generator by index (0 to generator_count-1)
    WaveGen* access_generator(int index);
    
    // Get the number of generators this station uses
    int get_generator_count() const { return _generator_count; }
    
    // Check if all generators are allocated
    bool has_all_generators() const { return _allocated_count == _generator_count; }

protected:
    int _generator_count;           // Number of generators this station needs
    int _allocated_count;           // Number currently allocated
    int _realizer_indices[MAX_GENERATORS_PER_STATION];  // Indices of allocated generators
};

#endif
