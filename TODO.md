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
- [ ] **Real-device validation of signal meter**
  - Test capacitor-like behavior on actual hardware
  - Verify charge/discharge rates feel authentic
- [ ] **Audio output quality assessment**
  - Ensure BFO offset produces clean audio
  - Test for any audio artifacts or distortion

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
- [ ] **Code style consistency**
  - Standardize comment styles across files
  - Ensure consistent naming conventions

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

---

*Last Updated: June 20, 2025*
*Use this document to park minor issues and ideas to avoid distraction during major feature development.*
