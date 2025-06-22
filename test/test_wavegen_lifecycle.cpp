#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "realizer_pool.h"

// Test WaveGen allocation lifecycle - focused on pool behavior
// This test documents the current allocation pattern before implementing 
// per-message-cycle allocation/freeing for dynamic station pipelining

// Mock wave generators for testing
class MockWaveGen {
public:
    bool in_use;
    MockWaveGen() : in_use(false) {}
    void set_frequency(float frequency, bool main=true) {}
    void set_active_frequency(bool main) {}
    void force_refresh() {}
};

MockWaveGen test_wavegens[4];
WaveGen* test_wavegen_ptrs[4] = {
    reinterpret_cast<WaveGen*>(&test_wavegens[0]),
    reinterpret_cast<WaveGen*>(&test_wavegens[1]),
    reinterpret_cast<WaveGen*>(&test_wavegens[2]),
    reinterpret_cast<WaveGen*>(&test_wavegens[3])
};
bool test_statuses[4];

void setUp(void) {
    // Reset all WaveGens to free state
    for (int i = 0; i < 4; i++) {
        test_statuses[i] = false;
        test_wavegens[i].in_use = false;
    }
}

void tearDown(void) {
    // Clean up
}

void test_basic_pool_allocation_deallocation_cycle(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    // Test the allocation/deallocation cycle that we want to implement for stations
    
    // Initially, all WaveGens should be available
    int available_count = 0;
    for (int i = 0; i < 4; i++) {
        if (!test_statuses[i]) available_count++;
    }
    TEST_ASSERT_EQUAL(4, available_count);
    
    // Simulate message cycle: allocate → use → free
    int wavegen1 = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, wavegen1);  // Should get first available
    
    // Pool should show 1 allocated, 3 available
    available_count = 0;
    for (int i = 0; i < 4; i++) {
        if (!test_statuses[i]) available_count++;
    }
    TEST_ASSERT_EQUAL(3, available_count);
    
    // Simulate end of message cycle: free the WaveGen
    pool.free_realizer(wavegen1);
    
    // Should be back to 4 available
    available_count = 0;
    for (int i = 0; i < 4; i++) {
        if (!test_statuses[i]) available_count++;
    }
    TEST_ASSERT_EQUAL(4, available_count);
    
    // Should be able to allocate again (possibly same index)
    int wavegen2 = pool.get_realizer();
    TEST_ASSERT_NOT_EQUAL(-1, wavegen2);  // Should succeed
}

void test_multiple_stations_sharing_wavegen_pool(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    // Simulate multiple stations requesting WaveGens for their message cycles
    // This tests the dynamic sharing scenario we want to enable
    
    // Station 1 starts transmitting
    int station1_wavegen = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, station1_wavegen);
    
    // Station 2 starts transmitting  
    int station2_wavegen = pool.get_realizer();
    TEST_ASSERT_EQUAL(1, station2_wavegen);
    
    // Station 1 finishes message, frees WaveGen
    pool.free_realizer(station1_wavegen);
    
    // Station 3 can now get Station 1's freed WaveGen
    int station3_wavegen = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, station3_wavegen);  // Should reuse index 0
    
    // Verify current allocation state: station2 (index 1) and station3 (index 0)
    TEST_ASSERT_TRUE(test_statuses[0]);   // station3
    TEST_ASSERT_TRUE(test_statuses[1]);   // station2  
    TEST_ASSERT_FALSE(test_statuses[2]);  // available
    TEST_ASSERT_FALSE(test_statuses[3]);  // available
    
    // Clean up
    pool.free_realizer(station2_wavegen);
    pool.free_realizer(station3_wavegen);
    
    // All should be available again
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_FALSE(test_statuses[i]);
    }
}

void test_pool_exhaustion_and_recovery(void) {
    WaveGenPool pool(test_wavegen_ptrs, test_statuses, 4);
    
    // Test what happens when all WaveGens are allocated
    // This simulates heavy station activity
    
    int wavegens[4];
    
    // Allocate all WaveGens
    for (int i = 0; i < 4; i++) {
        wavegens[i] = pool.get_realizer();
        TEST_ASSERT_EQUAL(i, wavegens[i]);  // Should get sequential indices
    }
    
    // Try to allocate one more - should fail
    int extra_wavegen = pool.get_realizer();
    TEST_ASSERT_EQUAL(-1, extra_wavegen);  // Pool exhausted
    
    // Free one WaveGen (simulate message cycle completion)
    pool.free_realizer(wavegens[1]);  // Free index 1
    
    // Should now be able to allocate again
    int recovered_wavegen = pool.get_realizer();
    TEST_ASSERT_EQUAL(1, recovered_wavegen);  // Should reuse freed index
    
    // Clean up remaining allocations
    pool.free_realizer(wavegens[0]);
    pool.free_realizer(recovered_wavegen);  // This is index 1
    pool.free_realizer(wavegens[2]);
    pool.free_realizer(wavegens[3]);
    
    // Verify all are free
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_FALSE(test_statuses[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_pool_allocation_deallocation_cycle);
    RUN_TEST(test_multiple_stations_sharing_wavegen_pool);
    RUN_TEST(test_pool_exhaustion_and_recovery);
    
    return UNITY_END();
}
