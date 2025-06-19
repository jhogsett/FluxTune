#include "signal_meter.h"

#ifndef NATIVE_BUILD
extern PololuLedStrip<12> ledStrip;  // Defined in main.cpp
extern byte option_contrast;        // Defined in main.cpp
#endif

// Color definitions: Green → Yellow → Red (like analog signal meters)
#ifndef NATIVE_BUILD
rgb_color SignalMeter::_led_colors[LED_COUNT] = {
    { 0, 15, 0 },   // Green
    { 0, 15, 0 },   // Green
    { 0, 15, 0 },   // Green
    { 0, 15, 0 },   // Green
    { 15, 15, 0 },  // Yellow
    { 15, 15, 0 },  // Yellow
    { 15, 0, 0 }    // Red
};
#endif

SignalMeter::SignalMeter()
{
    _current_strength = 0;
}

void SignalMeter::init()
{
    clear();
}

void SignalMeter::update_signal_strength(int strength)
{
    // Clamp to valid range
    if (strength < 0) strength = 0;
    if (strength > 255) strength = 255;
    
    _current_strength = strength;
    write_leds();
}

void SignalMeter::clear()
{
    _current_strength = 0;
    write_leds();
}

void SignalMeter::write_leds()
{
#ifndef NATIVE_BUILD
    // Convert 0-255 strength to LED display
    int sample = _current_strength * 2;  // Scale to 0-510
    int on_leds = (sample / 73) + 1;     // How many full LEDs to light
    int remain = ((sample % 73) * 16) / 73;  // Brightness of partial LED
    
    // Ensure we don't exceed LED count
    if (on_leds > LED_COUNT) on_leds = LED_COUNT;
    if (on_leds < 0) on_leds = 0;
    
    // Copy base colors for lit LEDs
    memcpy(_led_buffer, _led_colors, on_leds * sizeof(rgb_color));
    
    // Clear remaining LEDs
    memset(_led_buffer + on_leds, 0, (LED_COUNT - on_leds) * sizeof(rgb_color));
    
    // Apply partial brightness to last LED if needed
    if (on_leds > 0) {
        _led_buffer[on_leds-1].red = (_led_buffer[on_leds-1].red * remain) / 16;
        _led_buffer[on_leds-1].green = (_led_buffer[on_leds-1].green * remain) / 16;
        _led_buffer[on_leds-1].blue = (_led_buffer[on_leds-1].blue * remain) / 16;
    }
    
    // Apply contrast adjustment
    for (int i = 0; i < LED_COUNT; i++) {
        _led_buffer[i].red = (_led_buffer[i].red * option_contrast) / 1;
        _led_buffer[i].green = (_led_buffer[i].green * option_contrast) / 1;
        _led_buffer[i].blue = (_led_buffer[i].blue * option_contrast) / 1;
    }
    
    // Write to hardware
    ledStrip.write(_led_buffer, LED_COUNT);
#endif
}
