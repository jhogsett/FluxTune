#ifndef __VFO_H__
#define __VFO_H__

#include "mode.h"
#include "realization.h"

// need to define a set of bands

class VFO : public Mode
{
public:
    // constructor
    VFO(const char *title, float frequency, unsigned long step, Realization *realization);
    
    virtual void update_display(HT16K33Disp *display);
    virtual void update_realization();

    unsigned long _frequency;
    byte _sub_frequency;
    unsigned long _step;
    Realization *_realization;

private:
};

#endif // __VFO_H__
