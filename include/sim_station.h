#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "async_morse.h"
#include "sim_transmitter.h"

#define SPACE_FREQUENCY 0.1

class SimStation : public SimTransmitter
{
public:
    SimStation(RealizerPool *realizer_pool);
    virtual bool begin(unsigned long time, float fixed_freq, const char *message, int wpm);
    
    virtual bool update(Mode *mode);

    virtual bool step(unsigned long time);

    void realize();

    virtual void end();

    float _fixed_freq;
    bool _enabled;
    float _frequency;
    AsyncMorse _morse;
    bool _active;
    bool _changed;
};

#endif
