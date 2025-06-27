# Silent Frequency Experimentation Feature

## Overview

This temporary development feature allows easy experimentation with different "silent frequency" values to study their effect on background noise in the FluxTune simulated radio.

## Background

When simulated stations are not transmitting, the AD9833 wave generators are set to a "silent frequency" instead of being turned completely off. This is because:

- **0.0 MHz**: Causes audible POP sounds when generators turn on/off
- **0.1 MHz**: Creates a 10-second sine wave cycle that is inaudible to the human ear but introduces subtle background noise that may be beneficial for radio simulation realism

## Usage

### Enabling the Feature

1. In `include/station_config.h`, uncomment:
   ```cpp
   #define ENABLE_SILENT_FREQ_CONFIG  // Temporary: Enable configurable silent frequency
   ```

2. Adjust the frequency value:
   ```cpp
   #define SILENT_FREQ_HZ 0.1  // Change this value to experiment
   ```

### Experimentation Values

Try these values to observe different background noise characteristics:

- **0.05**: Lower frequency, longer cycle, less background noise
- **0.1**: Default value, good balance (10-second cycle)
- **0.2**: Higher frequency, shorter cycle, slightly more background noise  
- **0.5**: Even higher frequency, more noticeable background activity
- **1.0**: 1 MHz, very short cycle, minimal impact on background

### Disabling the Feature

Comment out the define to return to the default behavior:
```cpp
// #define ENABLE_SILENT_FREQ_CONFIG  // Temporary: Enable configurable silent frequency
```

## Implementation Details

The feature affects these components:
- **AD9833 initialization** in `main.cpp` (startup frequency)
- **SILENT_FREQ** constant in `sim_transmitter.h` (general silent frequency)
- **SPACE_FREQUENCY** in `sim_station.h` (CW station spaces)
- **NUMBERS_SPACE_FREQUENCY** in `sim_numbers.h` (Numbers station spaces)

## Temporary Nature

This is designed as a temporary experimentation tool. Once optimal values are determined, the feature can be disabled and the chosen value can be hardcoded back into the constants.

## Testing Notes

**VERIFIED WORKING**: Feature successfully tested with 350.0 Hz - audible tone clearly heard during silent periods across all station types.

When experimenting, listen for:
- Changes in background "hiss" or "noise floor"
- Any audible artifacts when stations start/stop transmitting
- Overall "band activity" feeling and realism
- Performance impact (higher frequencies may require more processing)
