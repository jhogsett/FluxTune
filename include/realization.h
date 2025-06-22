#ifndef __REALIZATION_H__
#define __REALIZATION_H__

#include "mode.h"
#include "realizer_pool.h"

// handles realization using a realizer
// 

class Mode;

class Realization
{
public:
    Realization(WaveGenPool *realizer_pool);

    virtual bool update(Mode *mode);

    virtual bool begin(unsigned long time);
    virtual bool step(unsigned long time);
    virtual void end();
      // Virtual method for wave generator refresh - default does nothing
    virtual void force_wave_generator_refresh() {}

    WaveGenPool *_realizer_pool;
    int _realizer;
};

#endif
