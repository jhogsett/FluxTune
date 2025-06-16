# Deprecated Files

This folder contains code that has been removed from the main FluxTune project.

## Files

### morse.h / morse.cpp
The original synchronous Morse code implementation. This has been replaced by `async_morse.h` / `async_morse.cpp` which provides non-blocking Morse code generation.

If synchronous Morse code is needed in the future, a simple blocking wrapper can be created around the async implementation.

### chime.h / chime.cpp
Clock chime functionality that was part of the removed arcade game features. This included:
- Beep chimes
- Hour count chimes  
- Morse code time announcements

These files were removed as part of the arcade game cleanup.

## Restoring Files

If any of these files need to be restored:
1. Copy them back to their original locations (`include/` and `src/`)
2. Add them back to the build files (`CMakeLists.txt`, `Makefile`, `platformio.ini`)
3. Update any necessary include statements in source files
