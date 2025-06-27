# Critical Bug Fix Pattern: Wave Generator Pool Access Validation

## Overview

This document describes a critical bug fix pattern discovered during resource management debugging in the FluxTune project. The bug caused device resets and memory corruption when stations attempted to access invalid wave generator pool resources.

## Root Cause

**Problem**: Stations were calling `access_realizer(_realizer)` even when `_realizer == -1`, causing memory corruption.

**Why it happened**: The `get_realizer()` method returns -1 when no wave generators are available, but station code didn't validate this before calling `access_realizer()`.

**Impact**: Accessing `_realizers[-1]` caused array bounds violations, leading to memory corruption and device resets, especially during mode switching when resources were being dynamically allocated/freed.

## Debug Evidence

Before fix, serial output showed:
```
STARTUP
G0G1G2G3M...F3...MA-1A-1MA-1A-1...A-1A-1A-1[DEVICE RESET]
```

- `G0G1G2G3` = Getting realizers 0,1,2,3 
- `F3` = Freeing realizer 3
- `A-1A-1A-1` = **CRITICAL BUG**: Accessing invalid realizer -1 repeatedly
- Device reset follows due to memory corruption

After fix, serial output shows:
```
STARTUP  
G0G1G2G3M...F3...G3...F2...G2...F0...G0
```
- Clean resource allocation/freeing without crashes
- No `A-1` accesses (the bug is eliminated)

## The Fix Pattern

### 1. Always Check Realizer Validity

**Before accessing any realizer, ALWAYS check:**
```cpp
if(_realizer == -1) {
    return [appropriate_return_value];  // No realizer available
}
```

### 2. Validate WaveGen Pointer After Access

**After calling `access_realizer()`, ALWAYS check for nullptr:**
```cpp
WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
if(wavegen == nullptr) {
    // CRITICAL: This should never happen if _realizer != -1!
#ifdef DEBUG_CRASH_INVESTIGATION
    Serial.print("C");  // Critical failure marker
    Serial.print(_realizer);
#endif
    return [appropriate_return_value];
}
```

### 3. Complete Fix Template

```cpp
bool SomeStation::some_method() {
    // Step 1: Check if we have a valid realizer
    if(_realizer == -1) {
        return false;  // No realizer available
    }
    
    // Step 2: Access the realizer  
    WaveGen *wavegen = _wave_gen_pool->access_realizer(_realizer);
    
    // Step 3: Validate the returned pointer
    if(wavegen == nullptr) {
        // CRITICAL: This should never happen if _realizer != -1!
#ifdef DEBUG_CRASH_INVESTIGATION
        Serial.print("C");
        Serial.print(_realizer);
#endif
        return false;  // Invalid realizer
    }
    
    // Step 4: Safe to use wavegen
    wavegen->set_frequency(_frequency);
    return true;
}
```

## Required Changes

### 1. Add Debug Include

Add to all station `.cpp` files:
```cpp
#include "station_config.h"
```

### 2. Station Methods Requiring This Pattern

Apply this pattern to **ALL** methods that call `access_realizer()`:

- `begin()` methods
- `realize()` methods  
- `update()` methods
- Any other method accessing `_wave_gen_pool->access_realizer(_realizer)`

### 3. Files Modified

- ✅ `src/wave_gen_pool.cpp` - Enhanced bounds checking
- ✅ `src/sim_transmitter.cpp` - Base class fixes
- ✅ `src/sim_station.cpp` - CW station fixes
- ✅ `src/sim_pager.cpp` - Pager station fixes
- ✅ `src/sim_numbers.cpp` - Numbers station fixes
- ✅ `src/sim_rtty.cpp` - RTTY station fixes  
- ✅ `src/sim_jammer.cpp` - Jammer station fixes
- ✅ `include/station_config.h` - Debug flag added

## Debug Infrastructure

### Master Debug Flag

Control all crash investigation debug output with:
```cpp
// In station_config.h
#define DEBUG_CRASH_INVESTIGATION  // Enable crash debug output
// #define DEBUG_CRASH_INVESTIGATION  // Disable for production
```

### Debug Output Meanings

- `G[n]` = Got realizer n
- `F[n]` = Freed realizer n  
- `X` = No realizers available (get_realizer returned -1)
- `A[n]` = Accessing invalid realizer n (should only see A-1 if bug exists)
- `E[n]` = Error: Attempt to free invalid realizer n
- `C[n]` = **CRITICAL**: Realizer n != -1 but access_realizer returned nullptr
- `M` = Mode switch occurred
- `.` = Alive marker (every 1000 main loops)

### Memory Savings

When `DEBUG_CRASH_INVESTIGATION` is disabled:
- **Flash savings**: ~344 bytes
- **RAM savings**: ~22 bytes

## Prevention Guidelines

### For New Station Classes

1. **Always inherit from SimTransmitter** (gets base protection)
2. **Override methods carefully** - apply the pattern in all overrides
3. **Test with resource contention** - Use `CONFIG_FIVE_CW_RESOURCE_TEST`

### For New Features

1. **Never call `access_realizer()` directly** without validation
2. **Always check return values** from resource allocation methods
3. **Use the debug infrastructure** during development

### Code Review Checklist

- [ ] Does the code call `access_realizer()`?
- [ ] Is there a `_realizer != -1` check before the call?
- [ ] Is there a `nullptr` check after the call?
- [ ] Are appropriate return values provided for failure cases?
- [ ] Is the `#ifdef DEBUG_CRASH_INVESTIGATION` pattern used?

## Testing

### Reproduce the Bug

1. Enable `CONFIG_FIVE_CW_RESOURCE_TEST` (5 stations, 4 wave generators)
2. Comment out the fix pattern in any station file
3. Upload and monitor - should see `A-1` pattern and crashes

### Verify the Fix

1. Apply the complete fix pattern
2. Monitor debug output for clean resource management
3. Perform extensive mode switching - no crashes should occur

## Related Issues

- Device resets during mode switching
- Memory corruption in resource-constrained scenarios  
- Stations going silent unexpectedly
- Wave generator pool inconsistencies

---

**This pattern is MANDATORY for all code that accesses wave generator pool resources.**

When in doubt, apply this pattern - it's better to be defensive than to cause device resets.
