# FluxTune Feature Ideas & Development Roadmap

## Overview
This document captures fun enhancement ideas for FluxTune, organized by implementation complexity and hardware requirements.

---

## 🟢 **Ready to Implement** (Current Hardware + Single Wave Generator)

### 1. Numbers Station (Morse Variant) 🎯 **HIGH PRIORITY**
- **Concept**: Creepy 5-character Morse code sequences instead of voice
- **Implementation**: 
  - Random groups like "7-4-9-2-1" sent in slow, deliberate Morse
  - Different timing cadences (slow/methodical vs rapid-fire)
  - Various formats: "Group 1... 7-4-9-2-1... Group 2... 8-8-3-1-5..."
- **Hardware**: Single wave generator ✅
- **Code Impact**: Extend `AsyncMorse` with number group generation
- **Atmospheric Factor**: Very high - mysterious and engaging

### 2. Soviet Woodpecker Radar 🎯 **HIGH PRIORITY**
- **Concept**: Rhythmic tapping/clicking interference pattern
- **Implementation**: 
  - Use existing jammer code as foundation (from git branch)
  - Characteristic 10Hz repetition rate with harmonics
  - Sweeps across multiple frequencies
- **Hardware**: Single wave generator ✅
- **Code Impact**: Adapt jammer infrastructure for specific timing patterns
- **Historical Factor**: Authentic and recognizable

### 3. Aviation/Marine Beacon Network
- **Concept**: Automated beacon identifiers (3-letter callsigns)
- **Implementation**:
  - "VOR AAA" (pause) "VOR AAA" style transmissions
  - Different beacon types: VOR, NDB, ILS, marine
  - Realistic timing intervals (every 10-30 seconds)
- **Hardware**: Single wave generator ✅
- **Code Impact**: New beacon class extending `SimStation`

### 4. Weather Station Network
- **Concept**: Automated weather data in Morse
- **Implementation**:
  - "WX STATION K9ABC... TEMP 72... WIND 15 NORTH... QRT"
  - Random but realistic weather data
  - Various reporting intervals
- **Hardware**: Single wave generator ✅
- **Code Impact**: Weather data generator + Morse transmission

---

## 🟡 **Requires Dual Wave Generator Architecture** (Medium-Term)

### 5. DTMF Phone Dialer 🎯 **VERY HIGH PRIORITY**
- **Concept**: Phone dialing sounds on random frequencies
- **Requirements**: 
  - Two simultaneous sine waves for DTMF tones
  - Architecture to coordinate dual generators
- **Implementation Phases**:
  1. Extend `WaveGen` to support coordinated dual-tone output
  2. Create `AsyncDTMF` class for tone pair generation
  3. Add `SimPhone` station type
- **Sounds**: Dial tone, busy signal, number sequences, hang-up clicks
- **Nostalgic Factor**: Extremely high

### 6. Enhanced Pager Diversity
- **Concept**: More complex pager protocols
- **Requirements**: Multiple tone combinations, sequential patterns
- **Current Status**: Basic dual-tone already implemented
- **Extensions**: 5-tone sequences, different timing protocols

---

## 🔴 **Requires Hardware Enhancements** (Future)

### 7. Atmospheric Noise & QRM
- **Limitation**: No audio level control in current hardware
- **Requirements**: Variable amplitude control for realistic fading
- **Alternative**: Could simulate with frequency modulation effects

### 8. Band Conditions Simulation
- **Limitation**: No audio level control for fading effects
- **Requirements**: Real-time amplitude modulation
- **Alternative**: On/off keying to simulate complete signal loss

---

## 🎮 **Interactive & Gaming Features** (Software Only)

### 9. Fox Hunt Mode
- **Concept**: Hidden transmitters appear randomly, player finds them
- **Implementation**: 
  - Special stations that appear/disappear
  - Score tracking for identified callsigns
  - Progressive difficulty
- **Hardware**: Current setup ✅
- **Code Impact**: Game state management, scoring system

### 10. Morse Learning Assistant
- **Concept**: Practice mode with built-in teaching
- **Implementation**:
  - Character-by-character learning mode
  - Speed progression
  - Built-in decoder practice
- **Hardware**: Current setup ✅
- **Code Impact**: Educational mode infrastructure

---

## 🏗️ **Architecture Improvements Needed**

### For DTMF and Advanced Features:
1. **Dual Wave Generator Coordination**
   - Modify `WaveGen` to handle coordinated dual outputs
   - Update `SimTransmitter` base class for multi-tone support
   - Ensure proper cleanup and resource management

2. **Enhanced Realization Pool**
   - Support for complex multi-generator stations
   - Better resource allocation for demanding features

3. **Audio Effect Framework**
   - Foundation for future audio processing
   - Modulation and timing effect infrastructure

---

## 🎯 **Recommended Implementation Order**

1. **Numbers Station (Morse)** - Immediate win, high impact
2. **Soviet Woodpecker** - Leverage existing jammer code
3. **Aviation Beacons** - Expand station variety easily
4. **DTMF Architecture** - Major enhancement, enables many features
5. **DTMF Phone Dialer** - Flagship dual-tone feature
6. **Fox Hunt Game** - Interactive engagement
7. **Weather Stations** - Content expansion

---

## 📝 **Notes**
- All features should maintain the authentic radio experience
- Hardware limitations guide implementation priorities
- Focus on features that enhance the "tuning across the bands" experience
- Maintain clean architecture for future extensibility

*Document created: June 19, 2025*
*Status: Living document - update as features are implemented*
