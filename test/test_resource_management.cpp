#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef

// Test WaveGenPool resource management - critical for dynamic station pipelining
// This tests the resource allocation logic independently

// Mock WaveGen for testing
class MockWaveGen {
public:
    bool in_use;
    float current_frequency;
    
    MockWaveGen() : in_use(false), current_frequency(0.0) {}
    
    void set_frequency(float freq, bool main=true) { current_frequency = freq; }
    void set_active_frequency(bool main) {}
    void force_refresh() {}
    void mark_in_use(bool used) { in_use = used; }
};

// Simple resource pool logic extracted for testing
class TestWaveGenPool {
private:
    MockWaveGen* realizers[4];
    bool statuses[4];
    int count;
    
public:
    TestWaveGenPool() : count(4) {
        for (int i = 0; i < count; i++) {
            realizers[i] = new MockWaveGen();
            statuses[i] = false;
        }
    }
    
    ~TestWaveGenPool() {
        for (int i = 0; i < count; i++) {
            delete realizers[i];
        }
    }
    
    int get_realizer() {
        for (int i = 0; i < count; i++) {
            if (!statuses[i]) {
                statuses[i] = true;
                realizers[i]->mark_in_use(true);
                return i;
            }
        }
        return -1;  // None available
    }
    
    void free_realizer(int index) {
        if (index >= 0 && index < count) {
            statuses[index] = false;
            realizers[index]->mark_in_use(false);
        }
    }
    
    MockWaveGen* access_realizer(int index) {
        if (index >= 0 && index < count) {
            return realizers[index];
        }
        return nullptr;
    }
    
    int get_available_count() {
        int available = 0;
        for (int i = 0; i < count; i++) {
            if (!statuses[i]) available++;
        }
        return available;
    }
};

TestWaveGenPool* test_pool;

void setUp(void) {
    test_pool = new TestWaveGenPool();
}

void tearDown(void) {
    delete test_pool;
}

void test_realizer_pool_initialization(void) {
    // All realizers should be available initially
    TEST_ASSERT_EQUAL(4, test_pool->get_available_count());
}

void test_realizer_pool_get_first_available(void) {
    // Should get first realizer (index 0)
    int realizer_idx = test_pool->get_realizer();
    TEST_ASSERT_EQUAL(0, realizer_idx);
    
    // Should have one less available
    TEST_ASSERT_EQUAL(3, test_pool->get_available_count());
}

void test_realizer_pool_get_sequential(void) {
    // Get realizers sequentially
    int idx1 = test_pool->get_realizer();
    int idx2 = test_pool->get_realizer();
    int idx3 = test_pool->get_realizer();
    int idx4 = test_pool->get_realizer();
    
    TEST_ASSERT_EQUAL(0, idx1);
    TEST_ASSERT_EQUAL(1, idx2);
    TEST_ASSERT_EQUAL(2, idx3);
    TEST_ASSERT_EQUAL(3, idx4);
    
    // All should be allocated
    TEST_ASSERT_EQUAL(0, test_pool->get_available_count());
}

void test_realizer_pool_exhaustion(void) {
    // Allocate all realizers
    for (int i = 0; i < 4; i++) {
        test_pool->get_realizer();
    }
    
    // Try to get one more - should fail
    int idx = test_pool->get_realizer();
    TEST_ASSERT_EQUAL(-1, idx);
}

void test_realizer_pool_free_and_reuse(void) {
    // Get first realizer
    int idx1 = test_pool->get_realizer();
    TEST_ASSERT_EQUAL(0, idx1);
    
    // Free it
    test_pool->free_realizer(idx1);
    TEST_ASSERT_EQUAL(4, test_pool->get_available_count());
    
    // Should be able to get it again
    int idx2 = test_pool->get_realizer();
    TEST_ASSERT_EQUAL(0, idx2);
}

void test_realizer_pool_access_realizer(void) {
    // Test accessing realizers
    MockWaveGen* r0 = test_pool->access_realizer(0);
    MockWaveGen* r1 = test_pool->access_realizer(1);
    
    TEST_ASSERT_NOT_NULL(r0);
    TEST_ASSERT_NOT_NULL(r1);
    TEST_ASSERT_NOT_EQUAL(r0, r1);  // Should be different objects
}

void test_realizer_pool_bounds_checking(void) {
    // Test invalid indices
    MockWaveGen* invalid1 = test_pool->access_realizer(-1);
    MockWaveGen* invalid2 = test_pool->access_realizer(100);
    
    TEST_ASSERT_NULL(invalid1);
    TEST_ASSERT_NULL(invalid2);
    
    // Free invalid indices should not crash
    test_pool->free_realizer(-1);
    test_pool->free_realizer(100);
    
    // Pool should still be intact
    TEST_ASSERT_EQUAL(4, test_pool->get_available_count());
}

void test_realizer_pool_mixed_operations(void) {
    // Complex scenario: allocate, free, reallocate
    int idx1 = test_pool->get_realizer();  // Get 0
    int idx2 = test_pool->get_realizer();  // Get 1
    
    test_pool->free_realizer(idx1);        // Free 0
    
    int idx3 = test_pool->get_realizer();  // Should reuse 0
    TEST_ASSERT_EQUAL(0, idx3);
    
    // Should have 2 allocated (indices 0 and 1), 2 free
    TEST_ASSERT_EQUAL(2, test_pool->get_available_count());
}

void test_realizer_pool_resource_tracking(void) {
    // Test that realizer states are properly tracked
    int idx = test_pool->get_realizer();
    MockWaveGen* realizer = test_pool->access_realizer(idx);
    
    TEST_ASSERT_TRUE(realizer->in_use);
    
    test_pool->free_realizer(idx);
    
    TEST_ASSERT_FALSE(realizer->in_use);
}

void test_realizer_pool_frequency_setting(void) {
    // Test that we can set frequencies on allocated realizers
    int idx = test_pool->get_realizer();
    MockWaveGen* realizer = test_pool->access_realizer(idx);
    
    realizer->set_frequency(7002000.0);
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, realizer->current_frequency);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_realizer_pool_initialization);
    RUN_TEST(test_realizer_pool_get_first_available);
    RUN_TEST(test_realizer_pool_get_sequential);
    RUN_TEST(test_realizer_pool_exhaustion);
    RUN_TEST(test_realizer_pool_free_and_reuse);
    RUN_TEST(test_realizer_pool_access_realizer);
    RUN_TEST(test_realizer_pool_bounds_checking);
    RUN_TEST(test_realizer_pool_mixed_operations);
    RUN_TEST(test_realizer_pool_resource_tracking);
    RUN_TEST(test_realizer_pool_frequency_setting);
    
    return UNITY_END();
}
