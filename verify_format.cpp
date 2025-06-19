#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

void generate_number_group(char *buffer) {
    int digits[5];
    for(int i = 0; i < 5; i++) {
        digits[i] = rand() % 10;
    }
    snprintf(buffer, 6, "%d%d%d%d%d", digits[0], digits[1], digits[2], digits[3], digits[4]);
}

int main() {
    char group1[6], group2[6], transmission[80];
    generate_number_group(group1);
    generate_number_group(group2);
    snprintf(transmission, sizeof(transmission), "%s   %s   ", group1, group2);
    
    std::cout << "Numbers Station (no dashes): " << transmission << std::endl;
    std::cout << "Only digits and spaces: " << (strspn(transmission, "0123456789 ") == strlen(transmission) ? "YES" : "NO") << std::endl;
    return 0;
}
