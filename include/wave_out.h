#ifndef __WAVE_OUT_H__
#define __WAVE_OUT_H__

#include "realization.h"
#include "wave_gen_pool.h"

class WaveOut : public Realization
{
public:
    WaveOut(WaveGenPool *waveGenPool);
    
    virtual bool update(Mode *mode);

};

#endif
