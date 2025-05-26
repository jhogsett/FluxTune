#include "mode.h"
#include "realizer.h"
#include "realization.h"

Realization::Realization(Realizer *realizer){
    _realizer = realizer;
}

// returns true on successful update
bool Realization::update(Mode *mode){
    return false;
}

// returns true on successful begin
bool Realization::begin(unsigned long time){
    return false;
}

// call periodically to keep realization dynamic
// returns true if it should keep going
bool Realization::step(unsigned long time){
    return false;
}

void Realization::internal_step(unsigned long time){
}

void Realization::end(){

}
