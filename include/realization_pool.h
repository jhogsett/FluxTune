
#ifndef __REALIZATION_POOL_H__
#define __REALIZATION_POOL_H__

#include "mode.h"
#include "realization.h"

// initialize with an array of realizers
// tracks whether they are in use
// can request 1 or more realizers

class RealizationPool
{
public:
    // pass array of realizer addresses, array of free/in-use bools, count of realizers 
    RealizationPool(Realization **realizations, bool *statuses,  int nrealizations);

    // // returns -1 if not available otherwise realizer index into array
    // int get_realization();

    // // multiplely gotten realizers must be freed individually
    // void free_realization(int nrealization);

    // Realization* access_realization(int nrealization);

    void update_pool(Mode *mode);

    // float ffrequency = float(_frequency) + (_sub_frequency / 10.0);

    // // WaveGen *wavegen = (WaveGen *)realization;
	// wavegen->_sig_gen->setFrequency((MD_AD9833::channel_t)0, float(_frequency));

private:
    Realization **_realizations;
    bool *_statuses;
    int _nrealizations;

};








// realization pool
// initialize with an array of realizations
// tracks current dial frequency
// queries realizations to see if they are able to start
// queries also to see if they should stop due to being out of range
// they'll stop on their own as well

#endif // __REALIZER_POOL_H__
