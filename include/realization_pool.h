#ifndef __REALIZATION_POOL_H__
#define __REALIZATION_POOL_H__

#include "mode.h"
#include "realization.h"

// Forward declaration
class StationManager;

// initialize with an array of realizers
// tracks whether they are in use
// can request 1 or more realizers

class RealizationPool
{
public:
    // pass array of realizer addresses, array of free/in-use bools, count of realizers 
    RealizationPool(Realization **realizations, bool *statuses,  int nrealizations);

    bool begin(unsigned long time);
    bool step(unsigned long time);
    void end();

    void update(Mode *mode);
    void force_sim_transmitter_refresh();  // Force hardware refresh for SimTransmitter objects
    void mark_dirty();  // Mark hardware state as unknown - triggers refresh on next update
    
    // Dynamic station management integration
    void set_station_manager(StationManager* station_manager);
    void set_current_vfo_frequency(float vfo_freq);

private:
    Realization **_realizations;
    bool *_statuses;
    int _nrealizations;
    bool _hardware_dirty;  // True when hardware state is unknown and needs refresh
    
    // Dynamic station management
    StationManager* _station_manager;
    float _current_vfo_freq;
    bool _vfo_freq_changed;
};








// realization pool
// initialize with an array of realizations
// tracks current dial frequency
// queries realizations to see if they are able to start
// queries also to see if they should stop due to being out of range
// they'll stop on their own as well

#endif // __REALIZER_POOL_H__
