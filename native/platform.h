#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// Platform detection and configuration
// This header determines whether we're compiling for Arduino or native platform

#if defined(ARDUINO) || defined(__AVR__) || !defined(NATIVE_BUILD)
    // Arduino platform
    #define PLATFORM_ARDUINO 1
    #define PLATFORM_NATIVE 0
      // Use real Arduino headers
    #include <Arduino.h>
    #include <Wire.h>
    #include <EEPROM.h>
    #include <Encoder.h>
    #include <MD_AD9833.h>
    #include <PololuLedStrip.h>
    #include <HT16K33Disp.h>
    
#else
    // Native platform (PC compilation)
    #define PLATFORM_ARDUINO 0
    #define PLATFORM_NATIVE 1
      // Use mock headers    #include "mock_arduino.h"
    #include "mock_wire.h"
    #include "mock_eeprom.h"
    #include "mock_encoder.h"
    #include "mock_md_ad9833.h"
    #include "mock_pololu_led_strip.h"
    #include "mock_ht16k33disp.h"
    
#endif

#endif // __PLATFORM_H__
