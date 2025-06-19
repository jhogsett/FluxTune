# Station Type Configuration

FluxTune supports multiple simulated station types that can be individually enabled or disabled to manage memory usage on Arduino devices.

## Station Types

| Station Type | Class | Description | Frequency |
|-------------|-------|-------------|-----------|
| CW/Morse | `SimStation` | Basic CW station with customizable message | 7002.0 kHz |
| Numbers | `SimNumbers` | Spooky numbers station with random number groups | 7002.7 kHz |
| Pager | `SimPager` | Pager simulation station | 7006.0 kHz |
| RTTY | `SimRTTY` | RTTY digital mode station | 7004.1 kHz |

## Configuration

Edit `include/station_config.h` to enable/disable station types:

```cpp
// Enable all stations (default)
#define ENABLE_MORSE_STATION    // Basic CW/Morse station
#define ENABLE_NUMBERS_STATION  // Numbers Station
#define ENABLE_PAGER_STATION    // Pager Station
#define ENABLE_RTTY_STATION     // RTTY Station

// Example: Enable only CW station for minimal memory usage
#define ENABLE_MORSE_STATION    // Basic CW/Morse station
// #define ENABLE_NUMBERS_STATION  // Numbers Station (DISABLED)
// #define ENABLE_PAGER_STATION    // Pager Station (DISABLED)
// #define ENABLE_RTTY_STATION     // RTTY Station (DISABLED)
```

## Memory Impact

Each station type consumes:
- Program memory (flash) for the station class code
- RAM for station instance and buffers
- Signal meter frequency entries

Disabling unused station types will:
- Reduce flash memory usage
- Reduce RAM consumption
- Simplify the RF environment for focused testing

## Notes

- Disabled stations are replaced with `nullptr` in arrays (no runtime impact)
- Station instances and initialization calls are conditionally compiled
- Signal meter automatically adapts to enabled station frequencies
- Uses fixed-size arrays (4) for simplicity - disabled stations become `nullptr`
- No complex dynamic array management - easy to understand and maintain

## Example Configurations

### Minimal CW Only
```cpp
#define ENABLE_MORSE_STATION    // Only CW station
// #define ENABLE_NUMBERS_STATION
// #define ENABLE_PAGER_STATION  
// #define ENABLE_RTTY_STATION
```

### Digital Modes Only
```cpp
// #define ENABLE_MORSE_STATION
// #define ENABLE_NUMBERS_STATION
#define ENABLE_PAGER_STATION    // Pager and RTTY only
#define ENABLE_RTTY_STATION
```

### Full Simulation (Default)
```cpp
#define ENABLE_MORSE_STATION    // All stations enabled
#define ENABLE_NUMBERS_STATION
#define ENABLE_PAGER_STATION
#define ENABLE_RTTY_STATION
```
