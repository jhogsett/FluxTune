#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "async_morse.h"
#include "realization.h"

class SimStation : public Realization
{
public:
    SimStation(Realizer *realizer);
    
    virtual bool update(Mode *mode);

    // virtual void begin(unsigned long time);
    virtual bool step(unsigned long time);
    // virtual void end();

    void realize();

    float _frequency;
    AsyncMorse _morse;
    bool _active;
};

#endif
