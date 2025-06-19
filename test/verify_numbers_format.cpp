#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

// Simple test to verify our Numbers Station format
void generate_number_group(char *buffer) {
    int digits[5];
    
    // Generate 5 random digits (0-9)
    for(int i = 0; i < 5; i++) {
        digits[i] = rand() % 10;
    }
    
    // Format as "X-X-X-X-X"
    snprintf(buffer, 20, "%d-%d-%d-%d-%d", 
             digits[0], digits[1], digits[2], digits[3], digits[4]);
}

int main() {
    std::cout << "Numbers Station Format Test" << std::endl;
    std::cout << "============================" << std::endl;
    
    char group1[20], group2[20];
    char transmission[80];
    
    // Generate sample transmission  
    generate_number_group(group1);
    generate_number_group(group2);
    
    snprintf(transmission, sizeof(transmission), "%s   %s   ", group1, group2);
    
    std::cout << "Sample transmission: " << transmission << std::endl;
    std::cout << "Length: " << strlen(transmission) << " characters" << std::endl;
    std::cout << "Contains only digits, dashes, and spaces: " << (strspn(transmission, "0123456789- ") == strlen(transmission) ? "YES" : "NO") << std::endl;
    std::cout << "18 WPM timing: " << (1000/18) << "ms per element" << std::endl;
    
    return 0;
}
