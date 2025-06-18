#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "realization.h"
#include "realizer_pool.h"
#include "async_morse.h"

#define SPACE_FREQUENCY 0.1

class SimStation : public Realization
{
public:
    SimStation(RealizerPool *realizer_pool);
    
    bool begin(unsigned long time, float fixed_freq, const char *message, int wpm);
    virtual bool update(class Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    void set_message(const char *message, int wpm);

private:
    float _fixed_freq;
    float _frequency;
    bool _enabled;
    AsyncMorse _morse;
    const char *_message;
    int _wpm;
    bool _message_set;
};

#endif
