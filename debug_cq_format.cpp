#include <stdio.h>

// Include the actual station configuration
#include "station_config.h"

#ifdef ENABLE_MORSE_STATION
#ifdef CONFIG_FOUR_FD
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "
#endif
// Now include the logic from sim_station.h
#ifndef CQ_MESSAGE_FORMAT
#define CQ_MESSAGE_FORMAT "CQ CQ DE %s %s K    "
#endif
#endif

int main() {
    printf("CONFIG_FOUR_FD defined: %s\n", 
#ifdef CONFIG_FOUR_FD
        "YES"
#else
        "NO"
#endif
    );
    
    printf("ENABLE_MORSE_STATION defined: %s\n", 
#ifdef ENABLE_MORSE_STATION
        "YES"
#else
        "NO"
#endif
    );
    
#ifdef ENABLE_MORSE_STATION
    printf("CQ_MESSAGE_FORMAT is: '%s'\n", CQ_MESSAGE_FORMAT);
    
    // Test a formatted message
    char test_message[50];
    sprintf(test_message, CQ_MESSAGE_FORMAT, "K1ABC", "K1ABC");
    printf("Generated message: '%s'\n", test_message);
#else
    printf("ENABLE_MORSE_STATION is not defined - CQ format not available\n");
#endif
    
    return 0;
}
