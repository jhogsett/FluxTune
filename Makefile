# Makefile for FluxTune native compilation
# Use: make native

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -DPLATFORM_NATIVE=1
INCLUDES = -Iinclude -Ilib/HT16K33Disp -Ilib/Randomizer -Inative

# Source files
SOURCES = src/buffers.cpp \
          src/chime.cpp \
          src/contrast.cpp \
          src/contrast_handler.cpp \
          src/displays.cpp \
          src/event_dispatcher.cpp \
          src/leds.cpp \
          src/led_handler.cpp \
          src/mode.cpp \
          src/mode_handler.cpp \
          src/morse.cpp \
          src/option.cpp \
          src/options_presenter.cpp \
          src/option_handler.cpp \
          src/realization.cpp \
          src/realization_pool.cpp \
          src/realizer.cpp \
          src/realizer_pool.cpp \
          src/saved_data.cpp \
          src/seeding.cpp \
          src/sim_rtty.cpp \
          src/sim_signal.cpp \
          src/sim_station.cpp \
          src/sleep_mode.cpp \
          src/speaker.cpp \
          src/utils.cpp \
          src/vfo.cpp \
          src/vfo_presenter.cpp \
          src/vfo_tuner.cpp \
          src/wavegen.cpp \
          src/wave_out.cpp \
          src/async_morse.cpp \
          src/async_rtty.cpp

# Mock implementations
MOCK_SOURCES = native/mock_arduino.cpp \
               native/mock_wire.cpp \
               native/mock_eeprom.cpp

# Library sources
LIB_SOURCES = lib/HT16K33Disp/HT16K33Disp.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o) $(MOCK_SOURCES:.cpp=.o) $(LIB_SOURCES:.cpp=.o)
TEST_OBJECTS = $(OBJECTS) native/test_main.o

# Default target
all: native

# Native test build
native: $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o fluxtune_test $(TEST_OBJECTS)
	@echo "Native build complete: fluxtune_test"

# Test run
test: native
	./fluxtune_test

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) native/test_main.o fluxtune_test
	@echo "Cleaned build files"

.PHONY: all native test clean
