#include <MD_AD9833.h>
#include "wavegen.h"

WaveGen::WaveGen(MD_AD9833 * sig_gen)
{
    _sig_gen = sig_gen;
}

// // returns true on success begin
// bool WaveGen::begin(unsigned long time, float frequency){
// 	_sig_gen->begin();
// 	_sig_gen->setFrequency((MD_AD9833::channel_t)0, frequency);
// 	_sig_gen->setMode(MD_AD9833::MODE_SINE);
//     return true;
// }

// // call periodically to keep realization dynamic
// // returns true if it should keep going
// bool WaveGen::step(unsigned long time){
//     return false;
// }

// void WaveGen::internal_step(unsigned long time){
// }

// void WaveGen::end(){

// }

void WaveGen::update(float freqeuecy){

}
