#include <MD_AD9833.h>
#include "realizer.h"

class WaveGen : public Realizer
{
public:
    WaveGen(MD_AD9833 * sig_gen);

    // virtual bool begin(unsigned long time, float frequency);
    // virtual bool step(unsigned long time);
    // virtual void end();

    // virtual void internal_step(unsigned long time);

    void update(float freqeuecy);

    MD_AD9833 * _sig_gen;

};
