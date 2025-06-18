# FluxTune Development Roadmap

## Author's Vision
- A simulated Ham Radio receiver, with frequency display, a large realistic tuning knob, a signal strength meter, a _mode change_ knob, a _volume control_ knob, two panel LEDs, and an audio speaker.
- While turning the tuning knob, it should be possible to encounter simulated transmitting stations at different frequencies, with their apprent audible (listening) frequency changing as the tuning knob is turned, perfectly simulating the experience of tuning in stations on a real Ham Radio receiver.

## Author's Hardware Description
- The device and software is powered by an Arduino Nano and based on the Platformio foundation.
- The simulated signal output is by way of four AD9833 digital signal generators that can be utilized independently. The devices use the SPI pins 11 and 13 as well as the "FSYNC" (or SPI Load) pins 8, A0 (14), A1 (15) and A2 (16). The outputs are coupled to an audio amplifier connected to the audio speaker.
- The tuning and mode change knobs are connected to rotary encoder switches connected to the Arduino Nano (pins 2-4 and 5-7). When the mode change knob is _turned_ it modifies the current mode, for example, going from VFO A to VFO B. When the mode change knob is _pushed_ it changes modes entires, going from "SimRadio" to "Wave Gen" mode (for future use) and then to "Settings" mode (for setting the contrast).
- The two panel LEDs are connected to the PWM-capable pins 9 and 10 to allow their brightness to be controlled.
- The signal strength meter is a 3D-printed simulated panel meter powered by a strip of 7 WS-2812 addressable LEDs connected to pin 12.
- The display uses two four-digit 14-segment alphanumeric displays powered by the HK16K33 LED driver chip and connected to the Arduino Nano via I2C pins A4 and A5. It's sequential I2C addresses 0x70 and 0x71 for a single 8-digit display

## Author's Wish List
- While tuning, it should be possible to continue to discover new stations. In the background, while the user is tuning, a _realization scheduler_ should be deactivating any no longer needed and out of range realizations (and freeing the related realizer resources), and activating new realizatins in time to tune into them and hear them while tuning. Ideally it should be hypothetically possible to turn the tuning knob continuously forever and always hear new stations popping up. Despite the limitation of only four signal generators, that ought to be enough to create a realistic experience like that.
- It should be easy to add new types of transmitting stations with their own modulation types (within the limits of the AD9833 signal generator).
- It should be possible to add whole new conceptual types of transmitting stations, for example a WWV simulation minus the speech, the sound of DTMF dialing like someone's making a call, pager signals like heard on a scanner, the "Russian Woodpecker".
- The CW and RTTY stations should generate new random callsigns and content, within the limits of available Arduino memory space




## Overview
This document outlines the planned development goals and technical improvements for the FluxTune project.

## Recently Completed ✅
- **SimTransmitter Refactoring Phase 1**: Successfully extracted common functionality from SimStation and SimRTTY into a base SimTransmitter class
- **Device Timing Verification**: Confirmed perfect CW and RTTY timing preservation throughout refactoring
- **Code Deduplication**: Eliminated ~40 lines of duplicate code while improving maintainability
- **Analog Pager Station Implementation**: Added realistic dual-tone pager simulation with authentic timing patterns
- **Authentic Pager Timing**: Implemented asymmetric tone durations (1.5s/0.8s) with inter-tone gaps, based on real two-tone sequential paging systems
- **Random Tone Generation**: Added DTMF-like tone pairs (650-1650 Hz range) with minimum 200 Hz separation, generated fresh for each page cycle

## Current Work 🚧
- **Further Pager Realism**: Research and implement additional authentic paging protocols and timing variations

## Short-Term Goals (Next 1-3 months) 🎯

### Code Architecture & Maintainability
- [ ] Complete SimTransmitter refactoring by moving remaining common initialization and update logic
- [ ] Evaluate and potentially refactor "life-cycle management" code (if needed)
- [ ] Review and optimize memory usage patterns
- [ ] Improve code documentation and inline comments

### Testing & Quality
- [ ] Expand native test coverage
- [ ] Add automated device testing procedures
- [ ] Implement regression testing for timing-critical functionality
- [ ] Set up continuous integration (if desired)

### Performance Optimization
- [ ] Profile memory usage and optimize where beneficial
- [ ] Review timing-critical code paths for efficiency
- [ ] Evaluate power consumption optimizations

## Medium-Term Goals (3-6 months) 🌟

### Feature Enhancements
- [ ] [Add your feature goals here]
- [ ] [Add your feature goals here]
- [ ] [Add your feature goals here]

### User Experience
- [ ] [Add UX improvement goals here]
- [ ] [Add UX improvement goals here]

### Hardware Support
- [ ] [Add hardware expansion goals here]
- [ ] [Add hardware expansion goals here]

## Long-Term Vision (6+ months) 🚀

### Major Features
- [ ] [Add major feature goals here]
- [ ] [Add major feature goals here]

### Architecture Evolution
- [ ] [Add architectural evolution goals here]
- [ ] [Add architectural evolution goals here]

### Platform Expansion
- [ ] [Add platform expansion goals here]
- [ ] [Add platform expansion goals here]

## Technical Debt & Maintenance 🔧
- [ ] Review deprecated code in `deprecated/` folder
- [ ] Evaluate and clean up backup files and temporary artifacts
- [ ] Standardize naming conventions across codebase
- [ ] Improve build system and dependency management

## Notes
- All timing-critical changes must be verified on actual hardware
- Maintain backwards compatibility where possible
- Document breaking changes thoroughly
- Prioritize code maintainability and readability

---
*Last updated: June 18, 2025*
*This roadmap is a living document and will be updated as priorities evolve.*
