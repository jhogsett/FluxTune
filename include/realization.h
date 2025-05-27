#ifndef __REALIZATION_H__
#define __REALIZATION_H__

#include "mode.h"
#include "realizer.h"

// handles realization using a realizer
// 

class Mode;

class Realization
{
public:
    Realization(Realizer *realizer);

    virtual bool update(Mode *mode);

    virtual bool begin(unsigned long time);
    virtual bool step(unsigned long time);
    virtual void end();

    virtual void internal_step(unsigned long time);

    Realizer *_realizer;
    unsigned long _started;
    unsigned long _next_step;
};

#endif
