#ifndef __SIM_BEACON_H__
#define __SIM_BEACON_H__

#include "realization.h"
#include "realizer_pool.h"
#include "async_beacon.h"

#define SILENT_FREQ 0.1

class SimBeacon : public Realization
{
public:
    SimBeacon(RealizerPool *realizer_pool);
    
    bool begin(unsigned long time, float frequency, uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms);
    virtual bool update(class Mode *mode);
    virtual bool step(unsigned long time);
    virtual void end();
    
    void set_pattern(uint8_t pattern_type, unsigned long on_time_ms, unsigned long off_time_ms);

private:
    AsyncBeacon _beacon;
    uint8_t _pattern_type;
    unsigned long _on_time_ms;
    unsigned long _off_time_ms;
    bool _beacon_set;
};

#endif
