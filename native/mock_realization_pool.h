#ifndef MOCK_REALIZATION_POOL_H
#define MOCK_REALIZATION_POOL_H

class Realization;

class RealizationPool {
public:
    RealizationPool(Realization **realizations, bool *statuses, int nrealizations) {}
    bool begin(unsigned long time) { return true; }
    bool step(unsigned long time) { return true; }
    void end() {}
    
    // Add any other required methods as needed
};

#endif // MOCK_REALIZATION_POOL_H
