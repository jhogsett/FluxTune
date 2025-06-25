#include <stdio.h>

// Include the actual station config like main.cpp does
#include "station_config.h"

// Now test the same logic as main.cpp
#ifdef ENABLE_MORSE_STATION
#ifdef CONFIG_FOUR_FD
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "
#endif
// Include the fallback logic from sim_station.h
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
    
#ifdef CQ_MESSAGE_FORMAT
    printf("CQ_MESSAGE_FORMAT is: '%s'\n", CQ_MESSAGE_FORMAT);
    
    // Test a formatted message
    char test_message[50];
    sprintf(test_message, CQ_MESSAGE_FORMAT, "K1ABC", "K1ABC");
    printf("Generated message: '%s'\n", test_message);
#else
    printf("CQ_MESSAGE_FORMAT is NOT defined!\n");
#endif
    
    return 0;
}
