cmake_minimum_required(VERSION 3.10)
project(FluxTune)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(include)
include_directories(lib/HT16K33Disp)
include_directories(lib/Randomizer)
include_directories(native)

# Define platform for conditional compilation
add_definitions(-DPLATFORM_NATIVE=1)

# Source files
set(SOURCES
    # Core source files
    src/buffers.cpp
    src/chime.cpp
    src/contrast.cpp
    src/contrast_handler.cpp
    src/displays.cpp
    src/event_dispatcher.cpp
    src/leds.cpp
    src/led_handler.cpp
    src/mode.cpp
    src/mode_handler.cpp
    src/morse.cpp
    src/option.cpp
    src/options_presenter.cpp
    src/option_handler.cpp
    src/realization.cpp
    src/realization_pool.cpp
    src/realizer.cpp
    src/realizer_pool.cpp
    src/saved_data.cpp
    src/seeding.cpp
    src/sim_rtty.cpp
    src/sim_signal.cpp
    src/sim_station.cpp
    src/sleep_mode.cpp
    src/speaker.cpp
    src/utils.cpp
    src/vfo.cpp
    src/vfo_presenter.cpp
    src/vfo_tuner.cpp
    src/wavegen.cpp
    src/wave_out.cpp
    src/async_morse.cpp
    src/async_rtty.cpp
    
    # Mock Arduino implementations
    native/mock_arduino.cpp
    native/mock_wire.cpp
    native/mock_eeprom.cpp
    
    # Library source (we'll need to handle this)
    lib/HT16K33Disp/HT16K33Disp.cpp
)

# Test executable (we'll create a simple test main later)
add_executable(fluxtune_test
    ${SOURCES}
    native/test_main.cpp
)

# Main executable (will need modification of main.cpp)
add_executable(fluxtune_native
    ${SOURCES}
    src/main.cpp
)

# Link libraries
target_link_libraries(fluxtune_test)
target_link_libraries(fluxtune_native)
