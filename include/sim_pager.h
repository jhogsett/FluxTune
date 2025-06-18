#ifndef __SIM_PAGER_H__
#define __SIM_PAGER_H__

#include "async_pager.h"
#include "sim_transmitter.h"

// Pager tone frequencies (Hz offset from VFO)
#define PAGER_TONE_A_OFFSET 600.0     // First tone frequency offset
#define PAGER_TONE_B_OFFSET 1500.0    // Second tone frequency offset

class SimPager : public SimTransmitter
{
public:
    SimPager(RealizerPool *realizer_pool);
    
    virtual bool begin(unsigned long time, float fixed_freq);
    virtual bool update(Mode *mode);
    virtual bool step(unsigned long time);
    
    void realize();

private:
    AsyncPager _pager;
};

#endif
