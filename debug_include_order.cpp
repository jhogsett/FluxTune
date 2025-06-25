#include <stdio.h>

// Simulate the exact include order from main.cpp
#include "station_config.h"

int main() {
    printf("After including station_config.h:\n");
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

    // Now simulate the conditional define from main.cpp
#ifdef ENABLE_MORSE_STATION
#ifdef CONFIG_FOUR_FD
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "
    printf("CQ_MESSAGE_FORMAT defined in main logic: YES\n");
#endif
#endif

    printf("CQ_MESSAGE_FORMAT defined before sim_station.h: %s\n", 
#ifdef CQ_MESSAGE_FORMAT
           "YES"
#else
           "NO"
#endif
    );

    // Now include sim_station.h logic
#ifndef CQ_MESSAGE_FORMAT
#define CQ_MESSAGE_FORMAT "CQ CQ DE %s %s K    "
    printf("CQ_MESSAGE_FORMAT defined by sim_station.h fallback: YES\n");
#else
    printf("CQ_MESSAGE_FORMAT already defined, sim_station.h fallback skipped\n");
#endif

#ifdef CQ_MESSAGE_FORMAT
    printf("Final CQ_MESSAGE_FORMAT: '%s'\n", CQ_MESSAGE_FORMAT);
#else
    printf("CQ_MESSAGE_FORMAT is NOT defined!\n");
#endif
    
    return 0;
}
