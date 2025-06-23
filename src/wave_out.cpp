
#include "vfo.h"
#include "wavegen.h"
#include "wave_gen_pool.h"
#include "wave_out.h"

// mode is expected to be a derivative of VFO
WaveOut::WaveOut(WaveGenPool *waveGenPool) : Realization(waveGenPool){
    // _realizer = realizer;
}

// returns true on successful update
bool WaveOut::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    // WaveGen  *wavegen = (WaveGen*)_realizer;
    WaveGen *wavegen = (WaveGen*)_wave_gen_pool->access_realizer(_realizer);

    float frequencyf = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

	wavegen->set_frequency(frequencyf);

    return true;
}
