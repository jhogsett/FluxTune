#ifndef __MODE_H__
#define __MODE_H__

#include <HT16K33Disp.h>

// generic mode, has data and methods specific to a mode

// a mode has public methods/data
// modified by a mode handler
// has a method to update the display


class Mode
{
public:
    Mode();

    virtual void render(char *buffer);


private:
};

#endif // __MODE_H__
