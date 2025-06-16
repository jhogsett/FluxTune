#ifndef MOCK_REALIZATION_H
#define MOCK_REALIZATION_H

#include "mock_arduino.h"

class Realization {
public:
    Realization() {}
    virtual ~Realization() {}
    
    virtual bool begin(unsigned long time) { return true; }
    virtual bool step(unsigned long time) { return true; }
    virtual void end() {}
    
    // Add any other required methods as needed
};

#endif // MOCK_REALIZATION_H
