# Station Type Configuration

FluxTune supports multiple simulated station types that can be individually enabled or disabled to manage memory usage on Arduino devices. Additionally, it provides special testing configurations with four instances of the same station type.

## Station Types

| Station Type | Class | Description | Frequency Range |
|-------------|-------|-------------|----------------|
| CW/Morse | `SimStation` | Basic CW station with customizable message | 7002-7005 kHz |
| Numbers | `SimNumbers` | Spooky numbers station with random number groups | 7002.7-7005.7 kHz |
| Pager | `SimPager` | Pager simulation station | 7006-7009 kHz |
| RTTY | `SimRTTY` | RTTY digital mode station | 7004.1-7007.1 kHz |

## Configuration Modes

Edit `include/station_config.h` and uncomment ONE configuration:

### Production Configuration
```cpp
#define CONFIG_MIXED_STATIONS    // Default: All different station types
```
- One of each station type (CW, Numbers, Pager, RTTY)
- Best for normal operation and demonstration
- Diverse RF environment for realistic tuning experience

### Listening Pleasure Configuration
```cpp
#define CONFIG_CW_CLUSTER       // Four CW stations clustered in 40m for listening pleasure
```
- Four CW stations clustered in the low part of the 40m band
- Frequencies: 7002, 7003.5, 7004.2, 7005.8 kHz
- Speeds: 12, 16, 18, 22 WPM
- Designed so multiple stations are often audible simultaneously
- Each station has operator frustration drift and WPM variations
- Perfect for realistic amateur radio listening experience

### Testing Configurations

#### Four CW Stations
```cpp
#define CONFIG_FOUR_CW          // Four CW/Morse stations for CW testing
```
- Four CW stations with different callsigns and speeds (11, 15, 20, 25 WPM)
- Frequencies: 7002, 7003, 7004, 7005 kHz
- Perfect for testing CW reception and timing variations

#### Four Numbers Stations  
```cpp
#define CONFIG_FOUR_NUMBERS     // Four Numbers stations for spooky testing
```
- Four Numbers stations with different speeds (12, 15, 18, 22 WPM)
- Frequencies: 7002.7, 7003.7, 7004.7, 7005.7 kHz
- Create an intense spooky atmosphere for Numbers station testing

#### Four Pager Stations
```cpp
#define CONFIG_FOUR_PAGER       // Four Pager stations for digital testing
```
- Four Pager stations on different frequencies
- Frequencies: 7006, 7007, 7008, 7009 kHz
- Test pager signal decoding and interference patterns

#### Four RTTY Stations
```cpp
#define CONFIG_FOUR_RTTY        // Four RTTY stations for RTTY testing
```
- Four RTTY stations on different frequencies
- Frequencies: 7004.1, 7005.1, 7006.1, 7007.1 kHz
- Test RTTY decoding with multiple simultaneous signals

#### Minimal Configuration
```cpp
#define CONFIG_MINIMAL_CW       // Single CW station (minimal memory)
```
- Single CW station only
- Minimal memory footprint for constrained environments

## Usage

1. **Choose Configuration**: Edit `station_config.h` and uncomment the desired config
2. **Build**: Compile the project with your chosen configuration  
3. **Test**: The signal meter will automatically adapt to the active station frequencies
4. **Tune**: Use the VFO to tune across the frequency range and observe signal strength

## Configuration Benefits

- **Mixed Stations**: Best for testing and demonstration - diverse RF environment
- **CW Cluster**: Realistic amateur radio listening experience with overlapping signals
- **Focused Testing**: Test specific protocols without interference from other types
- **Signal Density**: Multiple stations create realistic crowded band conditions
- **Speed Variations**: Different WPM settings test timing sensitivity
- **Frequency Spread**: Stations spread across the band for tuning practice

## Memory Impact

Each station configuration uses approximately the same total memory but allocates it differently:
- **Mixed**: Moderate memory per station type, full feature set (79.7% Flash)
- **CW Cluster**: Lower memory usage, focused on CW stations only (68.5% Flash)
- **Four of Same**: Higher memory for one station type, others completely excluded  
- **Minimal**: Lowest total memory usage

## Notes

- Disabled stations are replaced with `nullptr` in arrays (no runtime impact)
- Station instances and initialization calls are conditionally compiled
- Signal meter automatically adapts to enabled station frequencies
- Uses fixed-size arrays (4) for simplicity - disabled stations become `nullptr`
- No complex dynamic array management - easy to understand and maintain
