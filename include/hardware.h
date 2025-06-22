#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define NUM_LEDS 2
#define FIRST_LED 9
#define LAST_LED 10

#define NUM_PANEL_LEDS 2
#define FIRST_PANEL_LED 9
#define LAST_PANEL_LED 10
#define WHITE_PANEL_LED 9   // Pin 9: White panel LED
#define BLUE_PANEL_LED 10   // Pin 10: Blue panel LED
#define PANEL_LOCK_LED_FULL_BRIGHTNESS 60

// ============================================================================
// DEVICE VARIANT CONFIGURATION
// Comment/uncomment one of these to match your hardware variant:
// ============================================================================
// #define DEVICE_VARIANT_GREEN_DISPLAY    // Original green display version
#define DEVICE_VARIANT_RED_DISPLAY      // Red display version with dimmer LEDs

// Device-specific scaling factors
#ifdef DEVICE_VARIANT_RED_DISPLAY
    // Red display variant: LEDs are much brighter, scale down significantly
    #define SIGNAL_METER_BRIGHTNESS_DIVISOR 5      // Divide by 5 for red display
    #define PANEL_LED_BRIGHTNESS_DIVISOR 5         // Divide by 5 for red display
#else
    // Green display variant (default): Original brightness
    #define SIGNAL_METER_BRIGHTNESS_DIVISOR 1      // No scaling for green display
    #define PANEL_LED_BRIGHTNESS_DIVISOR 1         // No scaling for green display
#endif

// // Arduino Nano pins for the button LEDs
// // digital pins
// #define NUM_BUTTON_LEDS 3
// #define FIRST_BUTTON_LED 6
// #define LAST_BUTTON_LED 8
// #define GREEN_BUTTON_LED 6
// #define AMBER_BUTTON_LED 7
// #define RED_BUTTON_LED 8

// Arduino Nano pins for arcade buttons - not used in FluxTune
// #define NUM_BUTTONS 4
// #define FIRST_BUTTON 2
// #define LAST_BUTTON 5
// #define ANY_BUTTON 2
// #define GREEN_BUTTON 3
// #define AMBER_BUTTON 4
// #define RED_BUTTON 5
// #define NUM_PLAY_BUTTONS 3
// #define FIRST_PLAY_BUTTON 3

// Arduino Nano pin for the vibration feedback motor MOSFET
// #define MOTOR_PIN 12

// Arduino Nano pin for the speaker coil MOSFET
// #define SPEAKER_PIN 13

#endif
