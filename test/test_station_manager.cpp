#include "unity.h"
#include "../native/mock_arduino.h"   // Must be first for byte typedef
#include "station_manager.h"
#include "signal_meter.h"

// Mock realizer pool that satisfies dependencies
// We don't need the mock file, just declare minimal functionality

// Mock components for testing
SignalMeter mock_signal_meter;

// Create minimal mock stations that satisfy the interface
class MockSimTransmitter : public SimTransmitter {
public:
    float test_freq;
    bool test_active;
    
    MockSimTransmitter(float freq) : SimTransmitter(nullptr), test_freq(freq), test_active(false) {
        _fixed_freq = freq;
    }
    
    virtual bool step(unsigned long time) override { return true; }
    virtual bool begin(unsigned long time) override { return true; }
    virtual bool update(Mode *mode) override { return true; }
      // Override active flag methods
    void setActive(bool active) { test_active = active; }
    bool isActive() const { return test_active; }
    float get_fixed_frequency() const { return test_freq; }
};

// Test stations for StationManager  
MockSimTransmitter test_cw1(7002000.0);
MockSimTransmitter test_cw2(7003000.0);
MockSimTransmitter test_numbers1(7004000.0);
MockSimTransmitter test_numbers2(7005000.0);

SimTransmitter* test_station_array[MAX_STATIONS] = {
    &test_cw1, &test_cw2, &test_numbers1, &test_numbers2, nullptr, nullptr
};

void setUp(void) {
    mock_signal_meter.init();
}

void tearDown(void) {
    // Reset all stations to dormant state
    for (int i = 0; i < MAX_STATIONS; i++) {
        if (test_station_array[i]) {
            test_station_array[i]->setActive(false);
        }
    }
}

void test_station_manager_initialization(void) {
    StationManager manager(test_station_array);
    
    // All stations should start dormant/inactive
    TEST_ASSERT_EQUAL(0, manager.getActiveStationCount());
    
    // All station pointers should be properly stored
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_NOT_NULL(manager.getStation(i));
    }
    
    // Null entries should return nullptr
    TEST_ASSERT_NULL(manager.getStation(4));
    TEST_ASSERT_NULL(manager.getStation(5));
}

void test_station_manager_station_activation(void) {
    StationManager manager(test_station_array);
    
    // Initially no active stations
    TEST_ASSERT_EQUAL(0, manager.getActiveStationCount());
    
    // Manually activate some stations for testing
    test_cw1.setActive(true);
    test_numbers1.setActive(true);
    
    // Should now have 2 active stations
    TEST_ASSERT_EQUAL(2, manager.getActiveStationCount());
}

void test_station_manager_basic_operations(void) {
    StationManager manager(test_station_array);
    
    // Test that update operations don't crash
    manager.updateStations(7000000);
    manager.allocateAD9833();
    manager.recycleDormantStations(7000000);
    
    // Should complete without errors
    TEST_ASSERT_TRUE(true);
}

void test_station_manager_frequency_access(void) {
    StationManager manager(test_station_array);
    
    // Test that we can access station frequencies
    SimTransmitter* station = manager.getStation(0);
    TEST_ASSERT_NOT_NULL(station);
    TEST_ASSERT_EQUAL_FLOAT(7002000.0, station->get_fixed_frequency());
    
    station = manager.getStation(2);
    TEST_ASSERT_NOT_NULL(station);
    TEST_ASSERT_EQUAL_FLOAT(7004000.0, station->get_fixed_frequency());
}

void test_station_manager_bounds_checking(void) {
    StationManager manager(test_station_array);
    
    // Test bounds checking
    TEST_ASSERT_NULL(manager.getStation(-1));
    TEST_ASSERT_NULL(manager.getStation(MAX_STATIONS));
    TEST_ASSERT_NULL(manager.getStation(100));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_station_manager_initialization);
    RUN_TEST(test_station_manager_station_activation);
    RUN_TEST(test_station_manager_basic_operations);
    RUN_TEST(test_station_manager_frequency_access);
    RUN_TEST(test_station_manager_bounds_checking);
    
    return UNITY_END();
}
