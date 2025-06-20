#include "mode.h"
#include "realizer_pool.h"
#include "realization.h"

Realization::Realization(RealizerPool *realizer_pool){
    _realizer_pool = realizer_pool;
    _realizer = -1;
}

// returns true on successful update
bool Realization::update(Mode *mode){
    return false;
}

// returns true on successful begin
bool Realization::begin(unsigned long time){
    // attempt to acquire a realizer
    _realizer = _realizer_pool->get_realizer();
    if(_realizer == -1)
        return false;
    return true;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool Realization::step(unsigned long time){
    // if(time >= _next_internal_step){
    //     _next_internal_step = time + _period;
    //     internal_step(time);
    // }
    return true;
}

// void Realization::internal_step(unsigned long time){
// }

void Realization::end(){
    if(_realizer != -1)
        _realizer_pool->free_realizer(_realizer);
}
