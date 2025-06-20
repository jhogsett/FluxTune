#ifndef MOCK_REALIZATION_POOL_H
#define MOCK_REALIZATION_POOL_H

class Realization;
class Realizer;

// Mock for RealizerPool (not RealizationPool!)
class RealizerPool {
public:
    RealizerPool(Realizer **realizers, bool *statuses, int nrealizers) {}
    int get_realizer() { return 0; }  // Always return first realizer for testing
    void free_realizer(int realizer_index) {}
    
    // Add any other required methods as needed
};

#endif // MOCK_REALIZATION_POOL_H
