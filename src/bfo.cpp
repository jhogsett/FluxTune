#include "saved_data.h"
#include "bfo.h"
#include "utils.h"
#include "buffers.h"

BFO::BFO(const char *title) : Option(title)
{
}

void BFO::next_option(){
    option_bfo_offset += BFO_INCREMENT;
    if(option_bfo_offset > MAX_BFO_OFFSET)
        option_bfo_offset = MAX_BFO_OFFSET;
}

void BFO::prev_option(){
    option_bfo_offset -= BFO_INCREMENT;
    if(option_bfo_offset < MIN_BFO_OFFSET)
        option_bfo_offset = MIN_BFO_OFFSET;
}

void BFO::update_display(HT16K33Disp *display){

#ifndef DISABLE_DISPLAY_OPERATIONS

    // No hardware adjustment needed for BFO (unlike contrast which affects display hardware)
    sprintf(display_text_buffer, "%d Hz", option_bfo_offset);
    display->scroll_string(display_text_buffer, 1, 1);
#endif
}
