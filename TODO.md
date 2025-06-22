# FluxTune TODO List

A centralized list of minor issues, enhancements, and technical debt to address when time permits.

## 🔧 Code Quality & Technical Debt

### LED Handler Improvements
- [ ] **Add LED style for all activated during show period** (`led_handler.h:32`)
  - Currently missing a style option for when all LEDs are active during display periods
- [ ] **Add LED style for make-before-break showing** (`led_handler.h:33`)
  - Implement smooth transition style to avoid LED flicker during mode changes
- [ ] **Move LED constants into the class** (`led_handler.h:66`)
  - Clean up global constants, move them into appropriate class scope

### Code Organization
- [ ] **Review sprintf format warnings in VFO display** (`vfo.cpp`)
  - Fix buffer overflow warnings in frequency display formatting
  - Consider using safer string formatting functions

## 🚀 Feature Enhancements

### Signal Processing
- [ ] **Enhanced signal meter decay curves**
  - Experiment with different capacitor discharge models
  - Add configurable decay rates for different signal types

### Audio & RF Simulation
- [ ] **Realistic band noise simulation**
  - Add background static/noise that varies by frequency
  - Implement atmospheric noise patterns
- [ ] **QSB (fading) effects for distant stations**
  - Add realistic signal strength variation over time
  - Implement propagation-based fading patterns

### User Experience
- [ ] **BFO option display name clarity**
  - Consider renaming the BFO menu option to be more human-readable
  - Current technical name may not be clear to casual users
  - Possible alternatives: "Beat Frequency", "Audio Tuning", "Fine Tune", etc.
- [ ] **QRN/QRM Lightning/Static Detector LED + Audio Static Generator (Blue LED)**
  - Use second panel LED as atmospheric noise/interference indicator
  - **Audio static generator**: Hardware module to blend authentic background static and noise bursts into audio stream
  - **Phase 1 (Software)**: Random charge accumulation model - slowly builds "static charge" over 10-30 seconds
  - Quick bright flash and fade when discharging (mimics lightning/static bursts)
  - Blue LED indicates when static generator is active/bursting
  - Frequency dependent activity - more on lower HF frequencies (realistic)
  - Station interference - brief flickers when multiple stations are close together
  - Optional "storm periods" with increased activity for realism
  - Strong signals could suppress static (noise blanking effect)
  - **Phase 2 (Hardware)**: Research and integrate hardware noise generator modules
  - Blend static/atmospheric noise directly into audio output stream
  - LED and audio generation controlled by same software logic
  - Authentic QRN (atmospheric noise) and QRM (interference) simulation
  - Visual (LED) + Audio (static) representation of atmospheric conditions
  - Blue color suggests electrical/lightning activity
  - Complements green "station lock" LED perfectly
  - **Future hardware expansion**: LED logic designed to work with hardware static generator
  - Architecture allows software-controlled static blending with visual feedback
- [ ] **Station information display**
  - Show current station type and frequency when tuning
  - Add brief station descriptions or callsigns
- [ ] **Preset frequency memories**
  - Allow users to save favorite frequencies
  - Quick access to interesting station frequencies

### Numbers Station Enhancements
- [ ] **Additional authentic number patterns**
  - Research and implement other real-world number group patterns
  - Add phonetic alphabet stations ("ALPHA BRAVO CHARLIE...")
- [ ] **Variable transmission schedules**
  - Implement realistic on/off periods
  - Add time-based station availability

## 🧪 Testing & Validation

### Test Coverage
- [ ] **Unit tests for new BFO functionality**
  - Test BFO offset calculations and audio frequency changes
  - Validate EEPROM persistence of BFO settings
- [ ] **Integration tests for enhanced SimNumbers**
  - Test complete cycle: interval → numbers → ending → delay
  - Validate phase transitions and timing accuracy

### Hardware Testing
- [ ] **Test SimPager station on VHF frequencies**
  - Move simulated pager station to 146.52 MHz (2m amateur band)
  - Switch to VFO C and verify station works correctly in new position
  - Validate VFO object handles frequency transitions properly
  - Ensure pager content and timing remain authentic at VHF frequencies
- [ ] **Test SimRTTY station on HF frequencies**
  - Move simulated RTTY station to 14 MHz range (20m amateur band)
  - Switch to VFO B and verify station works correctly in new position
  - Validate RTTY decoding and display at HF frequencies
  - Ensure baudot character generation remains accurate
- [ ] **Real-device validation of signal meter**
  - Test capacitor-like behavior on actual hardware
  - Verify charge/discharge rates feel authentic
- [ ] **Audio output quality assessment**
  - Ensure BFO offset produces clean audio
  - Test for any audio artifacts or distortion

## ✅ Testing & Safety Nets (COMPLETED)

### Dynamic Station Pipelining Safety Net Tests
- [x] **Added comprehensive test coverage for critical components** 
  - test_signal_meter_behavior.cpp - Signal meter decay and charge behavior
  - test_charge_calculation.cpp - VFO frequency proximity calculation  
  - test_resource_management.cpp - WaveGenPool resource allocation logic
  - test_station_state_logic.cpp - Station state transition management
  - All tests use Unity framework with minimal dependencies
  - Tests found and validate critical algorithm behavior
  - Safety net is in place for dynamic station pipelining work

### Baseline Verification (COMPLETED)
- [x] **Verified existing tests still pass**
  - AsyncMorse tests: 23/23 pass
  - AsyncRTTY tests: 20/20 pass  
  - Signal meter integration tests: working
  - Clean codebase with no regressions after cleanup

## 📚 Documentation

### User Documentation
- [ ] **BFO operation guide**
  - Document how to adjust BFO for optimal reception
  - Explain the purpose and effect of BFO offset
- [ ] **Station guide with frequency recommendations**
  - List interesting frequencies for each station type
  - Provide tuning tips for best experience

### Technical Documentation
- [ ] **Signal meter architecture documentation**
  - Document the capacitor-like accumulator design
  - Explain charge pulse timing and proximity calculations
- [ ] **Numbers station authenticity research**
  - Document research sources for realistic patterns
  - Maintain list of authentic features implemented

## 🐛 Minor Bugs & Issues

### Low Priority Fixes
- [ ] **Memory optimization opportunities**
  - Review buffer sizes and memory usage
  - Optimize for Arduino memory constraints
- [ ] **Code style consistency**  - Standardize comment styles across files
  - Ensure consistent naming conventions

## 🐛 Known Issues (Parked for Monitoring)

### Signal Meter Anomalies
- [ ] **Intermittent blue flash anomaly** 
  - **Trigger**: Tuning to Pager station on 2-meter band (VFO C)        
  - **Behavior**: Signal meter displays bright blue flash then stops responding
  - **Recovery**: Switch VFOs and tune to different station - meter resumes normal operation
  - **Frequency**: Rare/intermittent - needs more observation to determine pattern
  - **Status**: Parked until more data available

## 💡 Future Ideas (Blue Sky)

### Advanced Features
- [ ] **Multiple simultaneous stations**
  - Simulate realistic band conditions with multiple signals
  - Add station interference and mixing effects
- [ ] **Realistic propagation modeling**
  - Implement day/night propagation changes
  - Add seasonal and solar cycle effects
- [ ] **SDR-style waterfall display**
  - Visual frequency spectrum display
  - Real-time signal activity visualization

### Hardware Expansion
- [ ] **External antenna simulation**
  - Variable signal strength based on "antenna" selection
  - Directional antenna effects
- [ ] **S-meter integration**
  - Physical S-meter display for signal strength
  - Authentic meter movement and ballistics

---

## 📋 Completed Recently
- ✅ **Capacitor-like signal meter** - Implemented authentic charge/discharge behavior
- ✅ **Adjustable BFO offset** - Added user-controlled 0-2000Hz BFO with EEPROM persistence  
- ✅ **Enhanced SimNumbers station** - Added interval signals ("FT") and ending sequence ("00000")
- ✅ **Signal meter carrier integration** - Charge pulses only on actual carrier events
- ✅ **Numbers station authenticity research** - Documented real-world patterns and behaviors
- ✅ **Station initialization refactoring** - Moved all config parameters to constructors for dynamic station management
- ✅ **Fixed frequency units bug** - Corrected MHz/Hz mismatch that prevented stations from being audible
- ✅ **StationManager baseline integration** - Added basic StationManager class with working encoder functionality
- ✅ **Constructor code quality improvements** - Fixed initializer lists and added override specifiers per PR review
- ✅ **Station begin/end safety** - Made begin() and end() idempotent and safe for repeated calls to support dynamic pipelining
- ✅ **Member variable cleanup** - Removed redundant _stored_fixed_freq variables, using only base class _fixed_freq
- ✅ **Fixed frequency drift wave generator update** - Fixed bug where frequency drift didn't update audio until next tuning knob turn
- ✅ **AsyncMorse repeat functionality removal** - Simplified AsyncMorse to single-message only, stations handle their own repetition
- ✅ **SimStation operator frustration drift** - Working drift after 3-8 random CQ cycles with realistic ±75 Hz frequency changes

## 🚧 In Progress: Dynamic Station Pipelining
- ✅ **Basic StationManager integration** - Class instantiated and basic method calls working
- ✅ **Station begin/end consistency** - All stations now support safe restart via end() followed by begin()
- ⏸️ **Advanced StationManager features** - Distance calculations, state management, generator assignment (paused for mental break)
- 📋 **Need to implement**: Full state machine (DORMANT→ACTIVE→AUDIBLE→SILENT), frequency-based activation/deactivation

## 🎯 Fun Station Enhancements (Mental Diversion)
- ✅ **Numbers station frequency drift** - Add slight frequency changes between transmission cycles for creepiness
- ✅ **SimStation random callsigns** - Generate random callsigns per session with doubled digits to avoid real callsigns
- ✅ **SimStation operator frustration drift** - Station QSYs (changes frequency) after random number of CQ cycles
- [ ] **QSO Station (Two-Station Conversation)** - New station type simulating realistic amateur radio conversation
  - Two stations (different callsigns/locations) having brief QSO
  - Authentic exchange: callsigns, signal reports (59/599), QTH (location), name exchange
  - Realistic timing: CQ call, response, back-and-forth conversation, 73s and sign-off
  - Duration: 2-3 minutes typical for casual contact
  - Both stations on same frequency, alternating transmissions
  - Operator characteristics: speech patterns, CW speed variations, slight timing differences
  - Mix of CW and voice modes for variety
  - Could simulate "nets" (multiple stations checking in)
  - Frequency changes when QSO completes (stations QSY to other frequencies)
- [ ] **Pager station realistic timing** - Add authentic pager transmission patterns and delays
- [ ] **CW station QSB simulation** - Add subtle signal strength variation to simulate propagation
- [ ] **RTTY station frequency stability** - Add slight frequency wobble like old mechanical equipment

---

*Last Updated: June 21, 2025*
*Use this document to park minor issues and ideas to avoid distraction during major feature development.*
