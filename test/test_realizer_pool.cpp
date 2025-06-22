#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "realizer_pool.h"

// Mock wave generators for testing
class MockWaveGen {
public:
    bool is_allocated;
    
    MockWaveGen() : is_allocated(false) {}
    
    // Mock the WaveGen interface methods
    void set_frequency(float frequency, bool main=true) {}
    void set_active_frequency(bool main) {}
    void force_refresh() {}
};

MockWaveGen realizer1, realizer2, realizer3;
WaveGen* test_realizers[3] = {reinterpret_cast<WaveGen*>(&realizer1), 
                              reinterpret_cast<WaveGen*>(&realizer2), 
                              reinterpret_cast<WaveGen*>(&realizer3)};
bool test_statuses[3] = {false, false, false};

void setUp(void) {
    // Reset all realizers to free state
    for (int i = 0; i < 3; i++) {
        test_statuses[i] = false;
        reinterpret_cast<MockWaveGen*>(test_realizers[i])->is_allocated = false;
    }
}

void tearDown(void) {
    // Clean up
}

void test_realizer_pool_initialization(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Pool should initialize without errors
    TEST_ASSERT_TRUE(true);
}

void test_realizer_pool_get_first_available(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Should get first realizer (index 0)
    int realizer_idx = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, realizer_idx);
    
    // First realizer should now be marked as in use
    TEST_ASSERT_TRUE(test_statuses[0]);
}

void test_realizer_pool_get_multiple_realizers(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Get all realizers sequentially
    int idx1 = pool.get_realizer();
    int idx2 = pool.get_realizer();
    int idx3 = pool.get_realizer();
    
    TEST_ASSERT_EQUAL(0, idx1);
    TEST_ASSERT_EQUAL(1, idx2);
    TEST_ASSERT_EQUAL(2, idx3);
    
    // All should be marked as in use
    TEST_ASSERT_TRUE(test_statuses[0]);
    TEST_ASSERT_TRUE(test_statuses[1]);
    TEST_ASSERT_TRUE(test_statuses[2]);
}

void test_realizer_pool_exhaustion(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Get all available realizers
    int idx1 = pool.get_realizer();
    int idx2 = pool.get_realizer();
    int idx3 = pool.get_realizer();
    
    // Try to get one more - should return -1 (not available)
    int idx4 = pool.get_realizer();
    TEST_ASSERT_EQUAL(-1, idx4);
}

void test_realizer_pool_free_and_reuse(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Get first realizer
    int idx1 = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, idx1);
    TEST_ASSERT_TRUE(test_statuses[0]);
    
    // Free it
    pool.free_realizer(idx1);
    TEST_ASSERT_FALSE(test_statuses[0]);
    
    // Should be able to get it again
    int idx2 = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, idx2);
    TEST_ASSERT_TRUE(test_statuses[0]);
}

void test_realizer_pool_access_realizer(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Test accessing realizers by index
    WaveGen* r0 = pool.access_realizer(0);
    WaveGen* r1 = pool.access_realizer(1);
    WaveGen* r2 = pool.access_realizer(2);
    
    TEST_ASSERT_EQUAL(&realizer1, r0);
    TEST_ASSERT_EQUAL(&realizer2, r1);
    TEST_ASSERT_EQUAL(&realizer3, r2);
}

void test_realizer_pool_mixed_operations(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // Get two realizers
    int idx1 = pool.get_realizer();  // Should be 0
    int idx2 = pool.get_realizer();  // Should be 1
    
    TEST_ASSERT_EQUAL(0, idx1);
    TEST_ASSERT_EQUAL(1, idx2);
    
    // Free the first one
    pool.free_realizer(idx1);
    
    // Get another - should reuse index 0
    int idx3 = pool.get_realizer();
    TEST_ASSERT_EQUAL(0, idx3);
    
    // Status should show 0 and 1 in use, 2 free
    TEST_ASSERT_TRUE(test_statuses[0]);
    TEST_ASSERT_TRUE(test_statuses[1]);
    TEST_ASSERT_FALSE(test_statuses[2]);
}

void test_realizer_pool_bounds_checking(void) {
    WaveGenPool pool(test_realizers, test_statuses, 3);
    
    // These operations should not crash (basic bounds safety)
    pool.free_realizer(-1);     // Invalid index
    pool.free_realizer(100);    // Out of bounds
    
    // Access operations might return null or handle gracefully
    // The exact behavior depends on implementation, but should not crash
    pool.access_realizer(-1);
    pool.access_realizer(100);
    
    TEST_ASSERT_TRUE(true);  // If we get here, no crashes occurred
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_realizer_pool_initialization);
    RUN_TEST(test_realizer_pool_get_first_available);
    RUN_TEST(test_realizer_pool_get_multiple_realizers);
    RUN_TEST(test_realizer_pool_exhaustion);
    RUN_TEST(test_realizer_pool_free_and_reuse);
    RUN_TEST(test_realizer_pool_access_realizer);
    RUN_TEST(test_realizer_pool_mixed_operations);
    RUN_TEST(test_realizer_pool_bounds_checking);
    
    return UNITY_END();
}
