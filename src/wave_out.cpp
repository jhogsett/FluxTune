
#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "wave_out.h"

// mode is expected to be a derivative of VFO
WaveOut::WaveOut(WaveGenPool *waveGenPool) : Realization(waveGenPool){
    // _realizer = realizer;
}

// returns true on successful update
bool WaveOut::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    // WaveGen  *wavegen = (WaveGen*)_realizer;
    WaveGen *wavegen = (WaveGen*)_realizer_pool->access_realizer(_realizer);

    float frequencyf = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

	wavegen->set_frequency(frequencyf);

    return true;
}

// // returns true on successful begin
// bool Realization::begin(unsigned long time){
//     // attempt to acquire a realizer
//     _realizer = _realizer_pool->get_realizer();
//     if(_realizer == -1)
//         return false;

//     return true;
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
