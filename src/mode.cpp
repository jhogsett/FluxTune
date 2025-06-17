#ifdef NATIVE_BUILD
#include "../native/platform.h"
#else
#include <HT16K33Disp.h>
#endif
#include "mode.h"

Mode::Mode(const char *title){
    _title = title;
}

void Mode::update_display(HT16K33Disp *display){

}

void Mode::update_realization(){
    
}
