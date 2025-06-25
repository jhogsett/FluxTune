#include <stdio.h>

// Test default format first
#include "sim_station.h"

int main() {
    // Test that default format is correctly set
    printf("Default CQ format: '%s'\n", CQ_MESSAGE_FORMAT);
    
    // Verify it matches expected default
    const char* expected_default = "CQ CQ DE %s %s K    ";
    if (strcmp(CQ_MESSAGE_FORMAT, expected_default) == 0) {
        printf("✓ Default format is correct\n");
    } else {
        printf("✗ Default format mismatch!\n");
        return 1;
    }
    
    printf("✓ CQ format configuration test passed!\n");
    return 0;
}
