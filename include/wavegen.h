#include <MD_AD9833.h>
#include "realizer.h"

class WaveGen : public Realizer
{
public:
    WaveGen(MD_AD9833 * sig_gen);

    void set_frequency(float frequency, bool main=true);
    void set_active_frequency(bool main);

    MD_AD9833 * _sig_gen;
    float _frequency;
    bool _main;
};
