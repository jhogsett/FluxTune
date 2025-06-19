#ifndef STATION_CONFIG_H
#define STATION_CONFIG_H

// Station Type Configuration
// Comment out any station type to disable it and save memory

#define ENABLE_MORSE_STATION    // Basic CW/Morse station (SimStation)
#define ENABLE_NUMBERS_STATION  // Numbers Station (SimNumbers) 
#define ENABLE_PAGER_STATION    // Pager Station (SimPager)
#define ENABLE_RTTY_STATION     // RTTY Station (SimRTTY)

// Example: To keep only the CW station, comment out the others:
// #define ENABLE_MORSE_STATION    // Basic CW/Morse station (SimStation)
// // #define ENABLE_NUMBERS_STATION  // Numbers Station (SimNumbers) 
// // #define ENABLE_PAGER_STATION    // Pager Station (SimPager)
// // #define ENABLE_RTTY_STATION     // RTTY Station (SimRTTY)

#endif // STATION_CONFIG_H
