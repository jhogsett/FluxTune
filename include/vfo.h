#ifndef __VFO_H__
#define __VFO_H__

#include "mode.h"

// need to define a set of bands

class VFO : public Mode
{
public:
    // constructor
    VFO(float frequency, float step, int band);
    
    virtual void render(char *buffer);

    float _frequency;
    float _step;
    int _band;
    
private:
};

#endif // __VFO_H__
