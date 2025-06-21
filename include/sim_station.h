#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "async_morse.h"
#include "sim_transmitter.h"

class SignalMeter; // Forward declaration

#define SPACE_FREQUENCY 0.1

class SimStation : public SimTransmitter
{
public:
    SimStation(RealizerPool *realizer_pool, SignalMeter *signal_meter, float fixed_freq, const char *message, int wpm);
    virtual bool begin(unsigned long time) override;
    
    virtual bool update(Mode *mode) override;
    virtual bool step(unsigned long time) override;void realize();    AsyncMorse _morse;
    bool _changed;
    SignalMeter *_signal_meter;
    const char *_stored_message;    // Stored message from constructor
    int _stored_wpm;                // Stored WPM from constructor
};

#endif
