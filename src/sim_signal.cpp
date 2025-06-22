
#include "vfo.h"
#include "wavegen.h"
#include "realizer_pool.h"
#include "sim_signal.h"

// mode is expected to be a derivative of VFO
SimSignal::SimSignal(WaveGenPool *realizer_pool) : Realization(realizer_pool){
    // _realizer = realizer;
    _phase = 0;
    _frequency = 0.0;
}

#define MAX_PHASE 36

void SimSignal::realize(){
    switch(_phase){
        case 1:
            // active = true;
            break;
        case 2:
            // active = true;
            break;
        case 3:
            // active = true;
            break;
        case 4:
            // active = false;
            break;
        case 5:
            // active = true;
            break;
        case 6:
            // active = false;
            break;
        case 7:
            // active = true;
            break;
        case 8:
            // active = true;
            break;
        case 9:
            // active = true;
            break;
        case 10:
            // active = false;
            break;
        case 11:
            // active = true;
            break;        case 12:
            // active = false;
            break;
        case 13:
            // active = false;
            break;
        case 14:
            // active = false;
            break;

        case 15:
            // active = false;
            break;

        case 16:
            // active = true;
            break;
        case 17:
            // active = true;
            break;
        case 18:
            // active = true;
            break;
        case 19:
            // active = false;
            break;
        case 20:
            // active = true;
            break;
        case 21:
            // active = true;
            break;
        case 22:
            // active = true;
            break;
        case 23:
            // active = false;
            break;
        case 24:
            // active = true;
            break;        case 25:
            // active = false;
            break;
        case 26:
            // active = true;
            break;
        case 27:
            // active = true;
            break;
        case 28:
            // active = true;
            break;
        case 29:
            // active = false;
            break;

        case 30:
            // active = false;
            break;
        case 31:
            // active = false;
            break;
        case 32:
            // active = false;
            break;
        case 33:
            // active = false;
            break;
        case 34:
            // active = false;
            break;
        case 35:
            // active = false;
            break;
        case 36:
            // active = false;
            break;
}

    // @@@
    // WaveGen  *wavegen = (WaveGen*)_realizer;
    // if(active && (_frequency <= 5000.0))
    // 	wavegen->set_frequency(_frequency);
    // else
    // 	// wavegen->set_frequency(10000000.0);
    // 	wavegen->set_frequency(0.1);
}

// returns true on successful update
bool SimSignal::update(Mode *mode){
    VFO *vfo = (VFO*)mode;
    _frequency = float(vfo->_frequency) + (vfo->_sub_frequency / 10.0);
    _frequency = abs(_frequency - 7010000.0);

    // bool active = false;
    // switch(_phase){
    //     case 0:
    //         active = true;
    //         break;
    //     case 1:
    //         active = true;
    //         break;
    //     case 2:
    //         active = false;
    //         break;
    //     case 3:
    //         active = true;
    //         break;
    //     case 4:
    //         active = false;
    //         break;
    //     case 5:
    //         active = true;
    //         break;
    //     case 6:
    //         active = false;
    //         break;
    //     case 7:
    //         active = false;
    //         break;
    //     case 8:
    //         active = false;
    //         break;
    // }

    // WaveGen  *wavegen = (WaveGen*)_realizer;
    // if(active & _frequency <= 5000.0)
    // 	wavegen->set_frequency(_frequency);
    // else
    // 	// wavegen->set_frequency(10000000.0);
    // 	wavegen->set_frequency(0.1);

    realize();

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
    _phase++;
    if(_phase > MAX_PHASE)
        _phase = 0;

    realize();

    // if(_active){
    //     _active = false;
    // } else {
    //     _active = true;
    // }

    // WaveGen  *wavegen = (WaveGen*)_realizer;
    // if(_active)
    // 	wavegen->set_frequency(_frequency);
    // else
    // 	wavegen->set_frequency(10000000.0);
}

