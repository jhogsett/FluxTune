
#include "saved_data.h"
#include "contrast.h"
#include "utils.h"

Contrast::Contrast(const char *title) : Option(title)
{
}


void Contrast::next_option(){
    option_contrast++;
    if(option_contrast > MAX_CONTRAST)
        option_contrast = MAX_CONTRAST;
}

void Contrast::prev_option(){
    option_contrast--;
    if(option_contrast < MIN_CONTRAST)
        option_contrast = MIN_CONTRAST;
}

void Contrast::update_display(HT16K33Disp *display){
	const byte display_brightnesses[] = {(unsigned char)option_contrast, (unsigned char)option_contrast};
	display->init(display_brightnesses);
    char buffer[9];
    sprintf(buffer, "Level %d", option_contrast);
    display->scroll_string(buffer, 1, 1);
}
