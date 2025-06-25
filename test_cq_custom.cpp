#include <stdio.h>

// Test custom format by defining it before including the header
#define CQ_MESSAGE_FORMAT "CQ FD CQ FD DE %s %s K    "
#include "sim_station.h"

int main() {
    // Test that custom format is correctly set
    printf("Custom CQ format: '%s'\n", CQ_MESSAGE_FORMAT);
    
    // Verify it matches expected custom format
    const char* expected_custom = "CQ FD CQ FD DE %s %s K    ";
    if (strcmp(CQ_MESSAGE_FORMAT, expected_custom) == 0) {
        printf("✓ Custom format override works correctly\n");
    } else {
        printf("✗ Custom format override failed!\n");
        return 1;
    }
    
    printf("✓ Field Day CQ format test passed!\n");
    return 0;
}
