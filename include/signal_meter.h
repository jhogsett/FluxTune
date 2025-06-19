#ifndef __SIGNAL_METER_H__
#define __SIGNAL_METER_H__

#ifndef NATIVE_BUILD
#include <Arduino.h>
#include <PololuLedStrip.h>
#endif

// Signal Meter - 7 WS2812 LEDs showing signal strength
// Similar architecture to HT16K33Disp display
class SignalMeter
{
public:
    SignalMeter();
    
    void init();
    void update_signal_strength(int strength);  // 0-255 signal strength value
    void clear();
    
private:
    void write_leds();
    
    static const int LED_COUNT = 7;
    int _current_strength;
    
#ifndef NATIVE_BUILD
    rgb_color _led_buffer[LED_COUNT];
    static rgb_color _led_colors[LED_COUNT];
#endif
};

#endif // __SIGNAL_METER_H__
