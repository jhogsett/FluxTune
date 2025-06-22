# FluxTune Development Guidelines

## Environment Setup

### Windows/PowerShell Users - IMPORTANT
When running build commands in PowerShell, use semicolon (`;`) for command chaining, **NOT** `&&`:

```powershell
# ✅ CORRECT - Use semicolon
cd test; g++ -DNATIVE_BUILD ... -o test.exe; ./test.exe

# ❌ WRONG - This will fail in PowerShell
cd test && g++ -DNATIVE_BUILD ... -o test.exe && ./test.exe
```

**Why:** `&&` is bash syntax for conditional execution. PowerShell uses `;` for unconditional command chaining.

## Build Commands

### Native Tests
```powershell
# AsyncMorse tests
cd test; g++ -DNATIVE_BUILD -I../include -I../native -Iunity/src -std=c++11 -Wall -o test_async_morse.exe test_async_morse.cpp unity/src/unity.c ../src/async_morse.cpp ../src/utils.cpp ../src/buffers.cpp ../src/saved_data.cpp ../native/mock_arduino.cpp ../native/mock_wire.cpp ../native/mock_eeprom.cpp ../native/mock_ht16k33disp.cpp; ./test_async_morse.exe

# AsyncRTTY tests  
g++ -DNATIVE_BUILD -I. -Itest/unity/src -DUNITY_INCLUDE_DOUBLE test/test_async_rtty.cpp test/unity/src/unity.c src/async_rtty.cpp native/mock_arduino.cpp -o test_async_rtty; ./test_async_rtty
```

### VS Code Tasks
Prefer using the VS Code tasks when available:
- `Test: AsyncMorse Quick` - Build AsyncMorse tests
- `Test: Run AsyncMorse` - Run AsyncMorse tests
- `Test: AsyncRTTY Quick` - Build AsyncRTTY tests
- `Test: Run AsyncRTTY` - Run AsyncRTTY tests

## Architecture Notes

### Encoder Bug Fix
The phantom encoder movements bug was fixed by:
1. Resetting the diff accumulator in `EncoderHandler::diff()` 
2. Making encoder A and B processing symmetric
3. Removing aggressive event purging after mode changes

### Station Management
- Stations are managed by `RealizationPool` (stepping) and `WaveGenPool` (audio channels)
- All stations should be initialized in `loop()` and added to the realization array
- Frequency constants are defined at the top of `main.cpp`

## Common Pitfalls

1. **Command chaining in PowerShell** - Use `;` not `&&`
2. **Encoder purging** - Avoid aggressive purging, it was causing phantom movements
3. **Station initialization** - Ensure all stations are in both the realization array AND initialized with `.begin()`
