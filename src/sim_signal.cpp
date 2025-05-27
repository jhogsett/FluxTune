
#include "vfo.h"
#include "wavegen.h"
#include "sim_signal.h"

// mode is expected to be a derivative of VFO
SimSignal::SimSignal(Realizer *realizer) : Realization(realizer){
    _realizer = realizer;
    _active = false;
    _frequency = 0.0;
}

// returns true on successful update
bool SimSignal::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

    _frequency = abs(_frequency - 7010000.0);

    /// fix

    WaveGen  *wavegen = (WaveGen*)_realizer;
    if(_active)
    	wavegen->set_frequency(_frequency);
    else
    	wavegen->set_frequency(10000000.0);

    // float frequencyf = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);

    return true;
}

// // returns true on successful begin
// void SimSignal::begin(unsigned long time){
// }

// // call periodically to keep realization dynamic
// // returns true if it should keep going
// bool SimSignal::step(unsigned long time){
//     return false;
// }

// void SimSignal::end(){

// }

void SimSignal::internal_step(unsigned long time){
    if(_active){
        _active = false;
    } else {
        _active = true;
    }

    WaveGen  *wavegen = (WaveGen*)_realizer;
    if(_active)
    	wavegen->set_frequency(_frequency);
    else
    	wavegen->set_frequency(10000000.0);
}

