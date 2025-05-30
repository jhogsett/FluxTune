#include <MD_AD9833.h>
#include "wavegen.h"

WaveGen::WaveGen(MD_AD9833 * sig_gen)
{
    _sig_gen = sig_gen;
}

void WaveGen::set_frequency(float frequency, bool main){
	_sig_gen->setFrequency((MD_AD9833::channel_t)(main ? 0 : 1), frequency);
}

void WaveGen::set_active_frequency(bool main){
	_sig_gen->setActiveFrequency((MD_AD9833::channel_t)(main ? 0 : 1));
}
