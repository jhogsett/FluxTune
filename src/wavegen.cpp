#include <MD_AD9833.h>
#include "wavegen.h"

WaveGen::WaveGen(MD_AD9833 * sig_gen)
{
    _sig_gen = sig_gen;
}

void WaveGen::set_frequency(float frequency){
	_sig_gen->setFrequency((MD_AD9833::channel_t)0, frequency);
}
