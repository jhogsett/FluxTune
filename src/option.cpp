#include "option.h"

Option::Option(const char *title) : Mode(title)
{
}

void Option::next_option()
{
}

void Option::prev_option()
{
}

void Option::update_display(HT16K33Disp *display){
    display->scroll_string("test");
}
