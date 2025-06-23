#ifndef __VFO_REALIZER_H__
#define __VFO_REALIZER_H__

#include <MD_AD9833.h>
#include "realization.h"
#include "vfo.h"

// receives a VFO-derived mode 

// JH! This class appears unused
class VFO_Realizer
{
public:
    VFO_Realizer();

    void realize(VFO *vfo);

private:
    Realization *_realization;    
};
// JH! 

#endif