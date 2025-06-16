#ifdef NATIVE_BUILD
#include "../native/platform.h"
#else
#include <MD_AD9833.h>
#endif
#include "wavegen.h"

WaveGen::WaveGen(MD_AD9833 * sig_gen)
{
    _sig_gen = sig_gen;
	_frequency = 0.0;
	_main = false;
}

void WaveGen::set_frequency(float frequency, bool main){
	if(frequency != _frequency || _main != main){
		// Serial.println("set freq");
		_sig_gen->setFrequency((MD_AD9833::channel_t)(main ? 0 : 1), frequency);
		_frequency = frequency;
		_main = main;
	}
}

void WaveGen::set_active_frequency(bool main){
	if(_main != main){
		// Serial.println("set act freq");
		_sig_gen->setActiveFrequency((MD_AD9833::channel_t)(main ? 0 : 1));
		_main = main;
	}
}
