#ifndef __SIM_NUMBERS_H__
#define __SIM_NUMBERS_H__

#include "async_morse.h"
#include "sim_transmitter.h"

#define NUMBERS_SPACE_FREQUENCY 0.1

/**
 * Simulated Numbers Station - transmits creepy 5-digit number groups in Morse code
 * Generates mysterious transmissions like: "74921   88315   " (pure digits only, no punctuation)
 */
class SimNumbers : public SimTransmitter
{
public:
    SimNumbers(RealizerPool *realizer_pool);
    virtual bool begin(unsigned long time, float fixed_freq, int wpm = 18);
    
    virtual bool update(Mode *mode);
    virtual bool step(unsigned long time);

    void realize();

private:
    void generate_next_number_group();
    
    AsyncMorse _morse;
    char _group_buffer[7];          // Small buffer for single 5-digit group + space + null
    int _groups_sent;               // Count of groups sent in current cycle
    int _total_groups_per_cycle;    // Total groups to send per cycle (13 for creepiness)
    bool _in_inter_group_delay;     // True when waiting between groups
    unsigned long _next_group_time; // When to send next group
    bool _transmission_active;      // Track if morse is currently transmitting
    int _last_morse_state;          // Last morse state to detect completion
};

#endif
