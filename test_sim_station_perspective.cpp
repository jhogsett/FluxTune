// Test that sim_station.cpp gets the correct CQ format
#include <stdio.h>

// Simulate the same includes as sim_station.cpp
#include "station_config.h"
#ifdef CONFIG_FOUR_FD
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "
#endif

// Include sim_station.h to get the fallback logic
#include "sim_station.h"

int main() {
    printf("From sim_station compilation perspective:\n");
    printf("CONFIG_FOUR_FD defined: %s\n", 
#ifdef CONFIG_FOUR_FD
           "YES"
#else
           "NO"
#endif
    );
    
    printf("CQ_MESSAGE_FORMAT is: '%s'\n", CQ_MESSAGE_FORMAT);
    
    // Test a formatted message
    char test_message[50];
    sprintf(test_message, CQ_MESSAGE_FORMAT, "W1AW", "W1AW");
    printf("Generated message: '%s'\n", test_message);
    
    return 0;
}
