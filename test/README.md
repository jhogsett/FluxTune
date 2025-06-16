# FluxTune Testing with Unity

This directory contains unit tests for FluxTune using the Unity testing framework.

## Setup

1. Download Unity from: https://github.com/ThrowTheSwitch/Unity
2. Extract to `test/unity/` directory
3. Run tests with: `make test` or compile individual test files

## Test Structure

- `test_utils.cpp` - Tests for utility functions
- `test_morse.cpp` - Tests for Morse code functionality  
- `test_vfo.cpp` - Tests for VFO operations
- `test_saved_data.cpp` - Tests for data persistence
- `unity/` - Unity framework files

## Running Tests

### Native (Windows)
```bash
make test
```

### PlatformIO
```bash
pio test
```

## Writing Tests

Example test:
```cpp
#include "unity.h"
#include "utils.h"

void test_time_conversion() {
    long seconds = time_to_seconds(30, 15, 12); // 12:15:30
    TEST_ASSERT_EQUAL(44130, seconds);
    
    uint8_t hour, minute, second;
    seconds_to_time(seconds, second, minute, hour);
    TEST_ASSERT_EQUAL(12, hour);
    TEST_ASSERT_EQUAL(15, minute);
    TEST_ASSERT_EQUAL(30, second);
}
```
