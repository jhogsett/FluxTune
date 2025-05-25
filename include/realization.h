#ifndef __REALIZATION_H__
#define __REALIZATION_H__

class Realization
{
public:
    Realization();

    virtual bool begin(unsigned long time);
    virtual bool step(unsigned long time);
    virtual void end();

    virtual void internal_step(unsigned long time);

    unsigned long _started;
    unsigned long _next_step;
};

#endif
