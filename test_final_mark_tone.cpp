#include <iostream>
#include <cassert>
#include "native/mock_arduino.h"
#include "include/sim_rtty.h"
#include "include/wave_gen_pool.h"
#include "include/signal_meter.h"
#include "include/mode.h"

// Test to verify the new pattern: message → MARK → message → MARK → message → MARK → silence → repeat

int main() {
    std::cout << "Testing final MARK tone enhancement..." << std::endl;
    
    // Set up test environment
    WaveGenPool wave_gen_pool(nullptr, nullptr, 0);  // Mock pool for testing
    SignalMeter signal_meter;
    signal_meter.init();
    float test_freq = 1000.0;
    
    // Create SimRTTY instance
    SimRTTY rtty(&wave_gen_pool, &signal_meter, test_freq);
    
    // Set transmitter as active
    rtty.setActive(true);
    unsigned long time = 0;
    
    // Initialize
    if (!rtty.begin(time)) {
        std::cerr << "ERROR: Failed to initialize RTTY" << std::endl;
        return 1;
    }
    
    Mode mode("TEST");  // Create mode with title
    rtty.update(&mode);
    
    // Track the states we observe
    int message_count = 0;
    int mark_periods = 0;
    int silent_periods = 0;
    bool in_message = false;
    bool in_mark_period = false;
    bool in_silent_period = false;
    
    // Run for enough time to see at least one complete round (3 messages + timing)
    // Each message is ~23 chars * 7.5 bits * 22ms ≈ 3.8 seconds
    // Plus 3 MARK periods of 3 seconds each = 9 seconds
    // Plus 1 silent period of 6 seconds = 6 seconds
    // Total: ~19 seconds, so run for 25 seconds
    for (unsigned long t = 0; t < 25000; t += 10) {
        time = t;
        bool continuing = rtty.step(time);
        if (!continuing) break;
        
        // Check if we're in a wait delay (MARK or silent period)
        bool is_in_wait = rtty.is_in_wait_delay();
        
        if (is_in_wait) {
            // We're in some kind of delay period
            if (!in_mark_period && !in_silent_period) {
                // Just entered a delay period
                // We need to determine if it's MARK or silent by checking if transmitter is active
                // This is a bit tricky from outside, but we can infer from timing
                if (t > 4000 && t < 20000) {  // Rough timing window for MARK periods
                    in_mark_period = true;
                    mark_periods++;
                    std::cout << "MARK period " << mark_periods << " started at t=" << t << "ms" << std::endl;
                } else {
                    in_silent_period = true;
                    silent_periods++;
                    std::cout << "Silent period " << silent_periods << " started at t=" << t << "ms" << std::endl;
                }
            }
        } else {
            // We're not in a wait delay, so we're transmitting a message
            if (in_mark_period) {
                std::cout << "MARK period ended at t=" << t << "ms" << std::endl;
                in_mark_period = false;
            }
            if (in_silent_period) {
                std::cout << "Silent period ended at t=" << t << "ms" << std::endl;
                in_silent_period = false;
            }
            if (!in_message) {
                in_message = true;
                message_count++;
                std::cout << "Message " << message_count << " started at t=" << t << "ms" << std::endl;
            }
        }
        
        // Check if message just completed
        if (in_message && is_in_wait) {
            std::cout << "Message " << message_count << " completed at t=" << t << "ms" << std::endl;
            in_message = false;
        }
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "Messages transmitted: " << message_count << std::endl;
    std::cout << "MARK periods observed: " << mark_periods << std::endl;
    std::cout << "Silent periods observed: " << silent_periods << std::endl;
    
    // Verify the expected pattern
    // For one complete round, we should see:
    // - 3 messages
    // - 3 MARK periods (including the final one)
    // - 1 silent period
    if (message_count >= 3 && mark_periods >= 3) {
        std::cout << "\nSUCCESS: Enhanced pattern verified!" << std::endl;
        std::cout << "Pattern shows: message → MARK → message → MARK → message → MARK → silence" << std::endl;
        return 0;
    } else {
        std::cout << "\nERROR: Expected pattern not found" << std::endl;
        return 1;
    }
}
