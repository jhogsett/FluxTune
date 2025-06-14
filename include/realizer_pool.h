
#ifndef __REALIZER_POOL_H__
#define __REALIZER_POOL_H__

#include "realizer.h"

// initialize with an array of realizers
// tracks whether they are in use
// can request 1 or more realizers

class RealizerPool
{
public:
    // pass array of realizer addresses, array of free/in-use bools, count of realizers 
    RealizerPool(Realizer **realizers, bool *statuses,  int nrealizers);

    // returns -1 if not available otherwise realizer index into array
    int get_realizer();

    // multiplely gotten realizers must be freed individually
    void free_realizer(int nrealizer);

    Realizer * access_realizer(int nrealizer);

private:
    Realizer **_realizers;
    bool *_statuses;
    int _nrealizers;

};








// realization pool
// initialize with an array of realizations
// tracks current dial frequency
// queries realizations to see if they are able to start
// queries also to see if they should stop due to being out of range
// they'll stop on their own as well

#endif // __REALIZER_POOL_H__
