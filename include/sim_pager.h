#ifndef __SIM_PAGER_H__
#define __SIM_PAGER_H__

#include "async_pager.h"
#include "sim_transmitter.h"

// Pager tone frequency range (Hz offset from VFO) - authentic industry standard
#define PAGER_TONE_MIN_OFFSET 300.0    // Minimum tone frequency offset (industry standard)
#define PAGER_TONE_MAX_OFFSET 3000.0   // Maximum tone frequency offset (industry standard)
#define PAGER_TONE_MIN_SEPARATION 50.0  // Minimum separation between tones (reduced for 300-3000 Hz range)

class SimPager : public SimTransmitter
{
public:
    SimPager(RealizerPool *realizer_pool);
    
    virtual bool begin(unsigned long time, float fixed_freq);
    virtual bool update(Mode *mode);    virtual bool step(unsigned long time);
      void realize();
      // Debug method to display current tone pair
    void debug_print_tone_pair() const;
    
    // Method to generate new tone pairs for testing
    void generate_new_tone_pair();

private:
    AsyncPager _pager;
    float _current_tone_a_offset;
    float _current_tone_b_offset;
};

#endif
