#ifndef MOCK_REALIZATION_POOL_H
#define MOCK_REALIZATION_POOL_H

class Realization;
class WaveGen;

// Mock for WaveGenPool (not RealizationPool!)
class WaveGenPool {
public:
    WaveGenPool(WaveGen **wavegens, bool *statuses, int nwavegens) {}
    int get_realizer() { return 0; }  // Always return first wave generator for testing
    void free_realizer(int realizer_index) {}
    
    // Add any other required methods as needed
};

#endif // MOCK_REALIZATION_POOL_H
