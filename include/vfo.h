#ifndef __VFO_H__
#define __VFO_H__

#include "mode.h"
#include "realization.h"

// need to define a set of bands

class VFO : public Mode
{
public:
    // constructor
    VFO(const char *title, unsigned long frequency, unsigned long step, int band);
    
    virtual void update_display(HT16K33Disp *display);
    virtual void update_realization(Realization *realization);

    unsigned long _frequency;
    unsigned long _step;
    int _band;
    
private:
};

#endif // __VFO_H__
