#include "vfo.h"
#include "vfo_tuner.h"

VFO_Tuner::VFO_Tuner(Mode * mode) : ModeHandler(mode) {
    VFO *vfo = (VFO*) _mode;
}

// does mode-specific handling of the event to modify the mode
// returns true if event was consumed
bool VFO_Tuner::event_sink(int event, int event_data){
    VFO *vfo = (VFO*) _mode;

    Serial.println("--------------------");
    Serial.println(vfo->_frequency);
    Serial.println(vfo->_step);

    if(event == 1){
        vfo->_frequency += vfo->_step;
    } else if(event == -1){
        vfo->_frequency -= vfo->_step;
    }
    Serial.println(vfo->_frequency);
    Serial.println(vfo->_step);

    Serial.println("--------------------");

    return true;
}

// // periodic timing events for dynamic activities
// void VFO_Tuner::step(unsigned long time){

// }

void VFO_Tuner::update_display(HT16K33Disp *display){
    char buffer[20];

    // Serial.println("1");

    VFO *vfo = (VFO*) _mode;

    // Serial.println("2");

    // // vfo->render(NULL);
    // // _mode->render(NULL);

    // Serial.println("3");

    // Serial.println(buffer);

    // // // display->simple_show_string(buffer);
    // display->show_string(buffer);

    // Serial.println(vfo->_frequency);

    int intpart = int(vfo->_frequency);
    float decpart = vfo->_frequency - intpart;
    int decparti = int(decpart * 1000.0);

    // Serial.println(intpart);
    // Serial.println(decpart);
    // Serial.println(decparti);

    sprintf(buffer, "%3d.%03d", intpart, decparti);
    // Serial.println(buffer);
    display->show_string(buffer);

}
