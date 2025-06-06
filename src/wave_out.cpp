
#include "vfo.h"
#include "wavegen.h"
#include "wave_out.h"

// mode is expected to be a derivative of VFO
WaveOut::WaveOut(Realizer *realizer) : Realization(realizer){
    _realizer = realizer;
}

// returns true on successful update
bool WaveOut::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    WaveGen  *wavegen = (WaveGen*)_realizer;

    float frequencyf = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

	wavegen->_sig_gen->setFrequency((MD_AD9833::channel_t)0, frequencyf);

    return true;
}

// // returns true on successful begin
// bool Realization::begin(unsigned long time){
//     return false;
// }

// // call periodically to keep realization dynamic
// // returns true if it should keep going
// bool Realization::step(unsigned long time){
//     return false;
// }

// void Realization::internal_step(unsigned long time){
// }

// void Realization::end(){

// }
