#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef

// Test station state management logic - critical for dynamic station pipelining
// This tests the state transition logic in isolation

// Extract station state enum for testing
enum StationState {
    DORMANT,     // No frequency assigned, minimal memory usage
    ACTIVE,      // Frequency assigned, tracking VFO proximity  
    AUDIBLE,     // Active + has AD9833 generator assigned
    SILENT       // Active but no AD9833 (>4 stations in range)
};

// Simple station state manager for testing
class TestStationState {
private:
    StationState state;
    bool has_realizer;
    float frequency;
    bool active_flag;
    
public:
    TestStationState() : state(DORMANT), has_realizer(false), frequency(0.0), active_flag(false) {}
    
    void set_state(StationState new_state) {
        StationState old_state = state;
        state = new_state;
        
        // Handle state transition logic
        if (old_state == AUDIBLE && new_state != AUDIBLE) {
            // Losing AD9833 generator
            has_realizer = false;
        }
        if (new_state == AUDIBLE) {
            // Gaining AD9833 generator
            has_realizer = true;
        }
    }
    
    StationState get_state() const { return state; }
    bool is_audible() const { return state == AUDIBLE; }
    bool has_generator() const { return has_realizer; }
    
    void set_frequency(float freq) { frequency = freq; }
    float get_frequency() const { return frequency; }
    
    void set_active(bool active) { active_flag = active; }
    bool is_active() const { return active_flag; }
    
    // Validate that the station is in a consistent state
    bool is_consistent() const {
        // AUDIBLE stations must have realizers
        if (state == AUDIBLE && !has_realizer) return false;
        
        // Only AUDIBLE stations should have realizers
        if (state != AUDIBLE && has_realizer) return false;
        
        // DORMANT stations should not be active
        if (state == DORMANT && active_flag) return false;
        
        return true;
    }
};

TestStationState* test_station;

void setUp(void) {
    test_station = new TestStationState();
}

void tearDown(void) {
    delete test_station;
}

void test_station_initial_state(void) {
    // Station should start in DORMANT state
    TEST_ASSERT_EQUAL(DORMANT, test_station->get_state());
    TEST_ASSERT_FALSE(test_station->is_audible());
    TEST_ASSERT_FALSE(test_station->has_generator());
    TEST_ASSERT_FALSE(test_station->is_active());
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_dormant_to_active_transition(void) {
    // Transition from DORMANT to ACTIVE
    test_station->set_state(ACTIVE);
    
    TEST_ASSERT_EQUAL(ACTIVE, test_station->get_state());
    TEST_ASSERT_FALSE(test_station->is_audible());
    TEST_ASSERT_FALSE(test_station->has_generator());
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_active_to_audible_transition(void) {
    // ACTIVE -> AUDIBLE (gets AD9833 generator)
    test_station->set_state(ACTIVE);
    test_station->set_state(AUDIBLE);
    
    TEST_ASSERT_EQUAL(AUDIBLE, test_station->get_state());
    TEST_ASSERT_TRUE(test_station->is_audible());
    TEST_ASSERT_TRUE(test_station->has_generator());
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_audible_to_silent_transition(void) {
    // AUDIBLE -> SILENT (loses AD9833 but stays active)
    test_station->set_state(AUDIBLE);
    test_station->set_state(SILENT);
    
    TEST_ASSERT_EQUAL(SILENT, test_station->get_state());
    TEST_ASSERT_FALSE(test_station->is_audible());
    TEST_ASSERT_FALSE(test_station->has_generator());
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_audible_to_dormant_transition(void) {
    // AUDIBLE -> DORMANT (complete deactivation)
    test_station->set_state(AUDIBLE);
    test_station->set_state(DORMANT);
    
    TEST_ASSERT_EQUAL(DORMANT, test_station->get_state());
    TEST_ASSERT_FALSE(test_station->is_audible());
    TEST_ASSERT_FALSE(test_station->has_generator());
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_frequency_management(void) {
    // Test frequency assignment
    test_station->set_frequency(7002000.0);
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, test_station->get_frequency());
    
    // Frequency should persist across state changes
    test_station->set_state(ACTIVE);
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, test_station->get_frequency());
    
    test_station->set_state(AUDIBLE);
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, test_station->get_frequency());
}

void test_station_active_flag_independence(void) {
    // Active flag should be independent of state transitions
    test_station->set_active(true);
    TEST_ASSERT_TRUE(test_station->is_active());
    
    // Should remain active through state changes (except DORMANT check)
    test_station->set_state(ACTIVE);
    test_station->set_active(true);
    TEST_ASSERT_TRUE(test_station->is_active());
    
    test_station->set_state(AUDIBLE);
    TEST_ASSERT_TRUE(test_station->is_active());
}

void test_station_multiple_state_transitions(void) {
    // Test complex state transition sequence
    
    // DORMANT -> ACTIVE -> AUDIBLE -> SILENT -> DORMANT
    test_station->set_state(ACTIVE);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(AUDIBLE);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(SILENT);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(DORMANT);
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_state_consistency_validation(void) {
    // Test that all valid states pass consistency check
    test_station->set_state(DORMANT);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(ACTIVE);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(AUDIBLE);
    TEST_ASSERT_TRUE(test_station->is_consistent());
    
    test_station->set_state(SILENT);
    TEST_ASSERT_TRUE(test_station->is_consistent());
}

void test_station_realizer_management(void) {
    // Test realizer assignment tracking
    
    // Initially no realizer
    TEST_ASSERT_FALSE(test_station->has_generator());
    
    // Only AUDIBLE state should have realizer
    test_station->set_state(ACTIVE);
    TEST_ASSERT_FALSE(test_station->has_generator());
    
    test_station->set_state(AUDIBLE);
    TEST_ASSERT_TRUE(test_station->has_generator());
    
    test_station->set_state(SILENT);
    TEST_ASSERT_FALSE(test_station->has_generator());
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_station_initial_state);
    RUN_TEST(test_station_dormant_to_active_transition);
    RUN_TEST(test_station_active_to_audible_transition);
    RUN_TEST(test_station_audible_to_silent_transition);
    RUN_TEST(test_station_audible_to_dormant_transition);
    RUN_TEST(test_station_frequency_management);
    RUN_TEST(test_station_active_flag_independence);
    RUN_TEST(test_station_multiple_state_transitions);
    RUN_TEST(test_station_state_consistency_validation);
    RUN_TEST(test_station_realizer_management);
    
    return UNITY_END();
}
