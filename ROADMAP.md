# FluxTune Development Roadmap

## Overview
This document outlines the planned development goals and technical improvements for the FluxTune project.

## Recently Completed ✅
- **SimTransmitter Refactoring Phase 1**: Successfully extracted common functionality from SimStation and SimRTTY into a base SimTransmitter class
- **Device Timing Verification**: Confirmed perfect CW and RTTY timing preservation throughout refactoring
- **Code Deduplication**: Eliminated ~40 lines of duplicate code while improving maintainability

## Current Work 🚧
- **SimTransmitter Refactoring Phase 2**: Continue consolidating remaining common patterns and logic

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
