#define PLATFORM_NATIVE
#include <iostream>
#include <cstring>

int main() {
    char buffer[6];
    
    // Test what the sprintf generates
    sprintf(buffer, "%d%d%d%d%d", 1, 2, 3, 4, 5);
    
    std::cout << "Generated: '" << buffer << "'\n";
    std::cout << "Length: " << strlen(buffer) << "\n";
    std::cout << "Buffer size: " << sizeof(buffer) << "\n";
    
    // Check each character
    for(int i = 0; i < strlen(buffer); i++) {
        std::cout << "buffer[" << i << "] = '" << buffer[i] << "' (ASCII " << (int)buffer[i] << ")\n";
    }
    
    return 0;
}
