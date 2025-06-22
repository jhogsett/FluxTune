# Commits Preserved for Reference

This document lists commits that were removed from `main` but preserved in the `backup-commits-before-reset` branch for future reference or cherry-picking.

## Reset Information
- **Known Good State**: 3b0198c (station lock LED and contrast fixes working)
- **Backup Branch**: `backup-commits-before-reset`
- **Reset Date**: June 21, 2025

## Preserved Commits (newest to oldest)

### dc68b1b - Add signal meter anomaly documentation and clean up device variant config
- Contains documentation updates
- May have useful device variant configuration cleanup
- **Status**: Review for potential documentation improvements

### 5022185 - Merge PR #22: Add CW station drift and random callsign features  
- Contains CW station drift functionality
- Random callsign generation features
- **Status**: May want to cherry-pick drift features later

### a5af7f1 - WIP: Attempted panel LED revert - need to go back to working state
- Work-in-progress attempt to fix panel LED issues
- **Status**: Likely not needed (we have working state at 3b0198c)

### eeea50f - added cw station drift and round mgmt
- CW station drift implementation
- Round management features  
- **Status**: May want to cherry-pick drift features later

## How to Access These Commits

```bash
# Switch to backup branch to examine commits
git checkout backup-commits-before-reset

# View specific commit
git show <commit-hash>

# Cherry-pick a specific commit (from main branch)
git cherry-pick <commit-hash>

# Create a new feature branch from a specific commit
git checkout -b feature/drift-features <commit-hash>
```

## Detailed Feature Analysis

### 🎯 **HIGH PRIORITY** - Features to Cherry-Pick

#### 1. CW Station Drift and Random Callsigns (from eeea50f + 5022185)
**Value**: Adds realistic amateur radio behavior simulation
**Files affected**: 
- `src/sim_station.cpp` - operator frustration drift logic
- `include/sim_station.h` - new drift state variables
- `include/async_morse.h` - simplified API (removed repeat/wait)
- `src/async_morse.cpp` - cleaner state management

**Key Features**:
- **Operator Frustration Drift**: After 3-8 CQ cycles with no response, operator QSYs ±75Hz
- **Random Callsign Generation**: Creates fictional callsigns with doubled digits (W00ABC, K55XYZ) 
- **Realistic CQ Timing**: Proper wait delays between CQ cycles
- **AsyncMorse Simplification**: Removed complex repeat/wait logic, cleaner API

**Implementation Notes**:
- Uses `STEP_MORSE_MESSAGE_COMPLETE` return value for cycle management
- Generates session-based callsigns (stay same during session, change on restart)
- Safe fictional callsign format avoids real amateur radio callsigns

#### 2. Signal Meter Anomaly Documentation (from dc68b1b)
**Value**: Documents a known intermittent issue for future debugging
**Files affected**: `TODO.md`

**Content**:
```markdown
## 🐛 Known Issues (Parked for Monitoring)

### Signal Meter Anomalies
- [ ] **Intermittent blue flash anomaly** 
  - **Trigger**: Tuning to Pager station on 2-meter band (VFO C)        
  - **Behavior**: Signal meter displays bright blue flash then stops responding
  - **Recovery**: Switch VFOs and tune to different station - meter resumes normal operation
  - **Frequency**: Rare/intermittent - needs more observation to determine pattern
  - **Status**: Parked until more data available
```

### ⚠️ **AVOID** - Problematic Changes

#### 1. Hardware.h Device Variant Removal (from dc68b1b)
**Issue**: This commit removed the device variant configuration we need
**Current working config**:
```cpp
// #define DEVICE_VARIANT_GREEN_DISPLAY    // Original green display version
#define DEVICE_VARIANT_RED_DISPLAY      // Red display version with dimmer LEDs

#ifdef DEVICE_VARIANT_RED_DISPLAY
    #define SIGNAL_METER_BRIGHTNESS_DIVISOR 5
    #define PANEL_LED_BRIGHTNESS_DIVISOR 5
#else
    #define SIGNAL_METER_BRIGHTNESS_DIVISOR 1
    #define PANEL_LED_BRIGHTNESS_DIVISOR 1
#endif
```
**Action**: Keep current working configuration, don't cherry-pick this change

#### 2. WIP Panel LED Revert (from a5af7f1)
**Issue**: Work-in-progress attempt that may have broken functionality
**Status**: Our current state (3b0198c) has working panel LED - don't need this

### 📋 **Recommended Action Plan**

1. **First Priority**: Cherry-pick the CW station drift and random callsign features
   - Creates more realistic and engaging amateur radio simulation
   - Well-implemented with proper state management
   - Adds significant value to the user experience

2. **Second Priority**: Add the signal meter anomaly documentation to TODO.md
   - Helps with future debugging of intermittent issues
   - Good engineering practice to document known anomalies

3. **Keep Current**: Maintain working device variant configuration
   - Current hardware.h is confirmed working on both device variants
   - Don't regress this functionality

## Cherry-Pick Commands

```bash
# Cherry-pick CW drift features (may need conflict resolution)
git cherry-pick eeea50f

# Cherry-pick documentation (should be clean)
git show dc68b1b -- TODO.md | git apply

# Manual verification of device variant config (keep current working version)
```
