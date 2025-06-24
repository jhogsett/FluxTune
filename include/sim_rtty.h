#ifndef __SIM_RTTY_H__
#define __SIM_RTTY_H__

#include "sim_transmitter.h"
#include "async_rtty.h"

class SignalMeter; // Forward declaration

#define MARK_FREQ_SHIFT 170.0
#define RTTY_WAIT_SECONDS 8      // Wait time between messages
#define RTTY_IDLE_SECONDS 4      // Duration of idle pattern before next message

class SimRTTY : public SimTransmitter
{
public:
    SimRTTY(WaveGenPool *wave_gen_pool, SignalMeter *signal_meter, float fixed_freq);    virtual bool begin(unsigned long time) override;
    
    virtual bool update(Mode *mode) override;
    virtual bool step(unsigned long time) override;
    
    void realize();
    
private:
    void start_next_message();
    
    AsyncRTTY _rtty;
    int _phase;
    SignalMeter *_signal_meter;     // Pointer to signal meter for charge pulses    // Message cycling state
    bool _in_wait_delay;            // True when waiting between messages
    unsigned long _next_message_time;  // Time to start next message
    int _message_repeat_count;      // How many times to repeat current message
    int _current_repeat;            // Current repetition number
};

#endif
