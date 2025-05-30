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

    virtual void begin(unsigned long time);
    virtual bool step(unsigned long time);
    virtual void end();

    Realizer *_realizer;
};

#endif
