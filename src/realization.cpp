#include "realization.h"

Realization::Realization()
{
}

// returns true on success begin
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
