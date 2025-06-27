#define NATIVE_BUILD
#include "include/sim_pager.h"
#include "include/wave_gen_pool.h"
#include "include/signal_meter.h"
#include "src/sim_pager.cpp"
#include "src/wave_gen_pool.cpp"
#include "src/sim_transmitter.cpp"
#include "src/realization.cpp"
#include "src/async_pager.cpp"
#include "src/vfo.cpp"
#include "src/wavegen.cpp"
#include "src/utils.cpp"
#include "native/mock_arduino.cpp"
#include "native/mock_ht16k33disp.cpp"
#include <iostream>

class MockSignalMeter {
public:
    void charge_pulse(float freq) { /* Mock implementation */ }
};

void test_pager_resource_management() {
    std::cout << "Testing SimPager resource management..." << std::endl;
    
    WaveGenPool pool;
    MockSignalMeter signal_meter;
    float fixed_freq = 3500.0f;
    
    SimPager pager(&pool, &signal_meter, fixed_freq);
    
    std::cout << "Initial state - pool available: " << pool.get_available_count() << std::endl;
    
    // Start pager - should allocate a wave generator
    bool started = pager.begin(0);
    std::cout << "Pager started: " << (started ? "YES" : "NO") << std::endl;
    std::cout << "After begin - pool available: " << pool.get_available_count() << std::endl;
    
    // Simulate some time progression to trigger state changes
    unsigned long time = 0;
    int step_count = 0;
    
    while(step_count < 20) {  // Limit iterations to prevent infinite loop
        time += 100;  // Advance time by 100ms each step
        pager.step(time);
        
        // Check resource usage periodically
        if(step_count % 5 == 0) {
            std::cout << "Step " << step_count << " (time=" << time << "ms) - pool available: " 
                      << pool.get_available_count() << std::endl;
        }
        
        step_count++;
        
        // If we've gone beyond a full pager cycle, break
        if(time > 15000) break;  // 15 seconds should be enough for a full cycle
    }
    
    std::cout << "Test completed - final pool available: " << pool.get_available_count() << std::endl;
}

int main() {
    test_pager_resource_management();
    return 0;
}
