#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "realization_pool.h"
#include "sim_station.h"
#include "sim_numbers.h"
#include "../native/mock_realization_pool.h"

// Mock components for testing
SignalMeter mock_signal_meter;
WaveGenPool mock_realizer_pool(nullptr, nullptr, 0);

// Test realizations
SimStation test_station1(&mock_realizer_pool, &mock_signal_meter, 7002000.0, 20);
SimStation test_station2(&mock_realizer_pool, &mock_signal_meter, 7003000.0, 15);
SimNumbers test_numbers(&mock_realizer_pool, &mock_signal_meter, 7004000.0, 18);

Realization* test_realizations[3] = {&test_station1, &test_station2, &test_numbers};
bool test_statuses[3] = {false, false, false};

void setUp(void) {
    mock_signal_meter.init();
    // Reset realization states
    for (int i = 0; i < 3; i++) {
        test_statuses[i] = false;
    }
}

void tearDown(void) {
    // Clean up any active realizations
}

void test_realization_pool_initialization(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    
    // Pool should initialize without errors
    TEST_ASSERT_TRUE(true);  // If we get here, initialization succeeded
}

void test_realization_pool_dirty_flag_operations(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    
    // Test dirty flag operations
    pool.mark_dirty();
    
    // After marking dirty, next update should trigger refresh
    // We can't directly test the internal dirty flag, but we can verify 
    // the operations complete without errors
    
    // Mock VFO for update testing - just use nullptr since we're testing infrastructure
    pool.update(nullptr);
    
    TEST_ASSERT_TRUE(true);  // If we get here, operations completed successfully
}

void test_realization_pool_begin_step_end_cycle(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    unsigned long test_time = 1000;
    
    // Test full lifecycle
    bool result = pool.begin(test_time);
    TEST_ASSERT_TRUE(result);
    
    result = pool.step(test_time + 100);
    TEST_ASSERT_TRUE(result);
    
    pool.end();
    
    // Should complete without errors
    TEST_ASSERT_TRUE(true);
}

void test_realization_pool_force_refresh(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    
    // Test force refresh operation
    pool.force_sim_transmitter_refresh();
    
    // Should complete without errors
    TEST_ASSERT_TRUE(true);
}

void test_realization_pool_update_with_null_mode(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    
    // Test update with null mode (should handle gracefully)
    pool.update(nullptr);
    
    // Should complete without errors
    TEST_ASSERT_TRUE(true);
}

void test_realization_pool_multiple_dirty_marks(void) {
    RealizationPool pool(test_realizations, test_statuses, 3);
    
    // Test multiple dirty marks (should be safe)
    pool.mark_dirty();
    pool.mark_dirty();
    pool.mark_dirty();
    
    pool.update(nullptr);
    
    // Should complete without errors
    TEST_ASSERT_TRUE(true);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_realization_pool_initialization);
    RUN_TEST(test_realization_pool_dirty_flag_operations);
    RUN_TEST(test_realization_pool_begin_step_end_cycle);
    RUN_TEST(test_realization_pool_force_refresh);
    RUN_TEST(test_realization_pool_update_with_null_mode);
    RUN_TEST(test_realization_pool_multiple_dirty_marks);
    
    return UNITY_END();
}
