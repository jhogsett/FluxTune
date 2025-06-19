#ifndef __SIM_STATION_H__
#define __SIM_STATION_H__

#include "async_morse.h"
#include "sim_transmitter.h"

class SignalMeter; // Forward declaration

#define SPACE_FREQUENCY 0.1

class SimStation : public SimTransmitter
{
public:
    SimStation(RealizerPool *realizer_pool, SignalMeter *signal_meter);
    virtual bool begin(unsigned long time, float fixed_freq, const char *message, int wpm);
    
    virtual bool update(Mode *mode);
    virtual bool step(unsigned long time);    void realize();

    AsyncMorse _morse;
    bool _changed;
    SignalMeter *_signal_meter;
    
private:
    void send_carrier_charge_pulse();
};

#endif
