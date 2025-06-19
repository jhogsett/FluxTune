#ifndef STATION_CONFIG_H
#define STATION_CONFIG_H

// Station Type Configuration
// Choose ONE configuration mode by uncommenting it

// ===== PRODUCTION CONFIGURATION =====
#define CONFIG_MIXED_STATIONS    // Default: All different station types

// ===== TEST CONFIGURATIONS =====
// #define CONFIG_FOUR_CW          // Four CW/Morse stations for CW testing
// #define CONFIG_FOUR_NUMBERS     // Four Numbers stations for spooky testing
// #define CONFIG_FOUR_PAGER       // Four Pager stations for digital testing
// #define CONFIG_FOUR_RTTY        // Four RTTY stations for RTTY testing
// #define CONFIG_MINIMAL_CW       // Single CW station (minimal memory)

// ===== CONFIGURATION IMPLEMENTATION =====
#ifdef CONFIG_MIXED_STATIONS
    // Production: Mixed station types (default)
    #define ENABLE_MORSE_STATION    // Basic CW/Morse station (SimStation)
    #define ENABLE_NUMBERS_STATION  // Numbers Station (SimNumbers) 
    #define ENABLE_PAGER_STATION    // Pager Station (SimPager)
    #define ENABLE_RTTY_STATION     // RTTY Station (SimRTTY)
#endif

#ifdef CONFIG_FOUR_CW
    // Test: Four CW stations with different messages/speeds
    #define ENABLE_FOUR_CW_STATIONS
    #define ENABLE_MORSE_STATION
    // Other stations disabled for focused CW testing
#endif

#ifdef CONFIG_FOUR_NUMBERS
    // Test: Four Numbers stations with different frequencies
    #define ENABLE_FOUR_NUMBERS_STATIONS
    #define ENABLE_NUMBERS_STATION
    // Other stations disabled for focused Numbers testing
#endif

#ifdef CONFIG_FOUR_PAGER
    // Test: Four Pager stations
    #define ENABLE_FOUR_PAGER_STATIONS
    #define ENABLE_PAGER_STATION
    // Other stations disabled for focused Pager testing
#endif

#ifdef CONFIG_FOUR_RTTY
    // Test: Four RTTY stations
    #define ENABLE_FOUR_RTTY_STATIONS
    #define ENABLE_RTTY_STATION
    // Other stations disabled for focused RTTY testing
#endif

#ifdef CONFIG_MINIMAL_CW
    // Minimal: Single CW station for memory testing
    #define ENABLE_MORSE_STATION
    // All other stations disabled
#endif

#endif // STATION_CONFIG_H
