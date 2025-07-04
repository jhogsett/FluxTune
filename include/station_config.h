#ifndef STATION_CONFIG_H
#define STATION_CONFIG_H

// Station Type Configuration
// Choose ONE configuration mode by uncommenting it

// ===== PRODUCTION CONFIGURATION =====
// Choose ONE configuration mode by uncommenting it

// ===== PRODUCTION CONFIGURATION =====
// #define CONFIG_MIXED_STATIONS    // Default: All different station types

// ===== DEVELOPMENT CONFIGURATION =====  
// #define CONFIG_DEV_LOW_RAM       // Development: Minimal RAM usage for development work
                                 // SAVES ~191 BYTES RAM: Disables RTTY and Pager stations
                                 // Use this for dynamic station pipelining development

// ===== DEMONSTRATION CONFIGURATION =====
// #define CONFIG_MIXED_STATIONS    // Uncomment to restore full functionality for demos

// ===== TEST CONFIGURATIONS =====
// #define CONFIG_FOUR_CW          // Four CW/Morse stations for CW testing
#define CONFIG_FIVE_CW          // Four CW/Morse stations for simulating Field Day traffic
// #define CONFIG_TEST_PERFORMANCE  // Single test station for measuring main loop performance
// #define CONFIG_FILE_PILE_UP     // Five CW/Morse stations simulating Scarborough Reef pile-up (BS77H variations)
// #define CONFIG_FOUR_NUMBERS     // Four Numbers stations for spooky testing
// #define CONFIG_FOUR_PAGER       // Four Pager stations for digital testing
// #define CONFIG_FOUR_RTTY        // Four RTTY stations for RTTY testing
// #define CONFIG_FOUR_JAMMER      // Four Jammer stations for interference testing
// #define CONFIG_MINIMAL_CW       // Single CW station (minimal memory)

// ===== LISTENING PLEASURE CONFIGURATION =====
// #define CONFIG_CW_CLUSTER       // Four CW stations clustered in 40m for listening pleasure
                                   // Frequencies: 7002, 7003.5, 7004.2, 7005.8 kHz
                                   // Speeds: 12, 16, 18, 22 WPM  
                                   // Designed to often overlap in reception for realistic band activity

// ===== MEMORY OPTIMIZATION OPTIONS =====

// Resource Debug - Enable minimal debug output for resource allocation testing
// Only use during resource contention testing - disable for production
// #define DEBUG_WAVE_GEN_POOL  // Uncomment to enable resource debug output
// #define DEBUG_STATION_RESOURCES  // Uncomment to enable station resource debug output

// RTTY Memory Optimization
// For minimal Flash usage, you can disable real Baudot encoding and just generate random bits
// The RTTY simulation will sound authentic but won't transmit actual text
// Saves ~128 bytes of Flash memory from the Baudot lookup table
// #define RTTY_RANDOM_BITS_ONLY  // Uncomment to save Flash memory

// EEPROM Table Storage - Advanced Memory Optimization
// Moves AsyncMorse and AsyncRTTY lookup tables from Flash to EEPROM
// Saves ~164 bytes of Flash at the cost of slower table lookups
// REQUIRES: Run utils/eeprom_table_loader.ino sketch first to program tables
// #define USE_EEPROM_TABLES  // Uncomment to use EEPROM-based table storage

// ===== CONFIGURATION IMPLEMENTATION =====
#ifdef CONFIG_MIXED_STATIONS
    // Production: Mixed station types (default)
    #define ENABLE_MORSE_STATION    // Basic CW/Morse station (SimStation)
    #define ENABLE_NUMBERS_STATION  // Numbers Station (SimNumbers) 
    #define ENABLE_PAGER_STATION    // Pager Station (SimPager)
    #define ENABLE_RTTY_STATION     // RTTY Station (SimRTTY)
    // NOTE: To enable jammer, comment out RTTY and uncomment below:
    // #define ENABLE_JAMMER_STATION   // Jammer Station (SimJammer) - replaces RTTY
#endif

#ifdef CONFIG_DEV_LOW_RAM
    // Development: Optimized for low RAM usage during development
    #define ENABLE_MORSE_STATION    // Basic CW/Morse station (SimStation) - essential for testing
    #define ENABLE_NUMBERS_STATION  // Numbers Station (SimNumbers) - moderate RAM usage
    #define ENABLE_TEST_STATION     // Test Station (SimTest) - for loop performance testing
    // #define ENABLE_PAGER_STATION    // Pager Station (SimPager) - disabled to save RAM
    // #define ENABLE_RTTY_STATION     // RTTY Station (SimRTTY) - disabled to save RAM 
    // #define ENABLE_JAMMER_STATION   // Jammer Station (SimJammer) - disabled to save RAM
#endif

#ifdef CONFIG_FOUR_CW
    // Test: Four CW stations with different messages/speeds
    #define ENABLE_FOUR_CW_STATIONS
    #define ENABLE_MORSE_STATION
    // Other stations disabled for focused CW testing
#endif

#ifdef CONFIG_FIVE_CW
    // Test: Four CW stations with different messages/speeds
    #define ENABLE_FOUR_CW_STATIONS
    #define ENABLE_MORSE_STATION
    // Other stations disabled for focused CW testing
#endif

#ifdef CONFIG_FILE_PILE_UP
    // Test: Five CW stations simulating Scarborough Reef pile-up (BS77H variations)
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

#ifdef CONFIG_FOUR_JAMMER
    // Test: Four Jammer stations for interference testing
    #define ENABLE_FOUR_JAMMER_STATIONS
    #define ENABLE_JAMMER_STATION
    // Other stations disabled for focused Jammer testing
#endif

#ifdef CONFIG_MINIMAL_CW
    // Minimal: Single CW station for memory testing
    #define ENABLE_MORSE_STATION
    // All other stations disabled
#endif

#ifdef CONFIG_CW_CLUSTER
    // Listening pleasure: Four CW stations clustered in 40m band
    #define ENABLE_CW_CLUSTER_STATIONS
    #define ENABLE_MORSE_STATION
    // Other stations disabled for focused CW listening
#endif

#ifdef CONFIG_TEST_PERFORMANCE
    // Performance testing: Single test station for measuring main loop speed
    #define ENABLE_TEST_STATION
    // All other stations disabled for clean performance measurement
#endif

#endif // STATION_CONFIG_H
