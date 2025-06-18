# FluxTune
Arduino Based Artificial Radio

## Development Environment

This project is developed on Windows using PowerShell. When running terminal commands:
- Use `;` instead of `&&` for command chaining (PowerShell syntax)
- Example: `cd ..; git status` instead of `cd .. && git status`

## Testing

This project includes comprehensive test suites for core components:

### AsyncMorse Tests
- 23 tests covering Morse code generation, timing, and behavior
- Tests initialization, dot/dash patterns, timing consistency, and edge cases

### AsyncRTTY Tests  
- 17 tests covering RTTY signal generation, timing, and behavior
- Tests initialization, state transitions, bit timing, repeat modes, and randomness

### Running Tests

The tests use the Unity test framework and can be run natively on PC:

```bash
# AsyncMorse tests
g++ -DNATIVE_BUILD -I. -Itest/unity/src -std=c++11 test/test_async_morse.cpp test/unity/src/unity.c src/async_morse.cpp src/utils.cpp src/buffers.cpp src/saved_data.cpp native/mock_arduino.cpp native/mock_wire.cpp native/mock_eeprom.cpp native/mock_ht16k33disp.cpp -o test_async_morse && ./test_async_morse

# AsyncRTTY tests
g++ -DNATIVE_BUILD -I. -Itest/unity/src -DUNITY_INCLUDE_DOUBLE test/test_async_rtty.cpp test/unity/src/unity.c src/async_rtty.cpp native/mock_arduino.cpp -o test_async_rtty && ./test_async_rtty
```

VS Code tasks are also available for building and running tests.
