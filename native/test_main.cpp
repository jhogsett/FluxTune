#include "platform.h"
#include <iostream>

// Include FluxTune headers
#include "utils.h"
#include "saved_data.h"
#include "displays.h"

int main() {
    std::cout << "FluxTune Native Test Build" << std::endl;
    std::cout << "Platform: " << (PLATFORM_NATIVE ? "Native" : "Arduino") << std::endl;
    
    // Test basic Arduino functions
    std::cout << "\nTesting mock Arduino functions:" << std::endl;
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
      unsigned long start_time = millis();
    std::cout << "Start time: " << start_time << "ms" << std::endl;
    
    // Test basic FluxTune functionality
    std::cout << "\nTesting FluxTune components:" << std::endl;
    
    // Test load_f_string function
    char* loaded = load_f_string(F("Hello World"), NULL);
    std::cout << "Loaded F-string: " << loaded << std::endl;
    
    // Test saved data functionality
    std::cout << "\nTesting saved data:" << std::endl;
    std::cout << "About to call load_save_data()..." << std::endl;
    load_save_data();
    std::cout << "Saved data loaded successfully" << std::endl;
    
    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
}
