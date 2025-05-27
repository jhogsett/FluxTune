#include <MD_AD9833.h>
#include "realizer.h"

class WaveGen : public Realizer
{
public:
    WaveGen(MD_AD9833 * sig_gen);

    void set_frequency(float frequency);

    MD_AD9833 * _sig_gen;
};
