#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "sim_transmitter.h"
#include "async_morse.h"

#define SPACE_FREQUENCY 0.1

class SimStation : public SimTransmitter
{
public:
    SimStation(RealizerPool *realizer_pool);
    
    bool begin(unsigned long time, float fixed_freq, const char *message, int wpm);
    virtual bool update(class Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    void set_message(const char *message, int wpm);

private:
    // Keep remaining data members for now (removing _enabled as it's now inherited)
    AsyncMorse _morse;
    const char *_message;
    int _wpm;
    bool _message_set;
};

#endif
