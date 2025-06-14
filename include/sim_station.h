#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "async_morse.h"
#include "realization.h"
#include "realizer_pool.h"

#define MAX_AUDIBLE_FREQ 5000.0
#define MIN_AUDIBLE_FREQ 150.0
#define SPACE_FREQUENCY 0.1
#define SILENT_FREQ 0.1

class SimStation : public Realization
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
