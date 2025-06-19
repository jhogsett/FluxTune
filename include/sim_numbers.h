#ifndef __SIM_NUMBERS_H__
#define __SIM_NUMBERS_H__

#include "async_morse.h"
#include "sim_transmitter.h"

#define NUMBERS_SPACE_FREQUENCY 0.1

/**
 * Simulated Numbers Station - transmits creepy 5-digit number groups in Morse code
 * Generates mysterious transmissions like: "GROUP 1... 7-4-9-2-1... GROUP 2... 8-8-3-1-5..."
 */
class SimNumbers : public SimTransmitter
{
public:
    SimNumbers(RealizerPool *realizer_pool);
    virtual bool begin(unsigned long time, float fixed_freq);
    
    virtual bool update(Mode *mode);
    virtual bool step(unsigned long time);

    void realize();

private:
    void generate_next_transmission();
    void generate_number_group(char *buffer, int group_number);
    
    AsyncMorse _morse;
    char _transmission_buffer[80];  // Buffer for current transmission
    int _current_group;             // Current group number
    bool _transmission_complete;    // Flag to generate new transmission
};

#endif
