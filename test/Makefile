# Makefile for Unity-based tests

# Compiler settings
CC = g++
CFLAGS = -DNATIVE_BUILD -DPLATFORM_NATIVE -I../include -I../native -Iunity/src -std=c++11 -Wall
LDFLAGS = 

# Directories (relative to test directory)
SRC_DIR = ../src
TEST_DIR = .
UNITY_DIR = unity/src
NATIVE_DIR = ../native
BUILD_DIR = build

# Unity source
UNITY_SRC = $(UNITY_DIR)/unity.c

# Common source files needed for tests
COMMON_SOURCES = $(SRC_DIR)/utils.cpp $(SRC_DIR)/buffers.cpp $(SRC_DIR)/saved_data.cpp $(SRC_DIR)/async_morse.cpp
MOCK_SOURCES = $(NATIVE_DIR)/mock_arduino.cpp $(NATIVE_DIR)/mock_wire.cpp $(NATIVE_DIR)/mock_eeprom.cpp $(NATIVE_DIR)/mock_ht16k33disp.cpp

# Test executables
TEST_UTILS = $(BUILD_DIR)/test_utils.exe
TEST_MORSE = $(BUILD_DIR)/test_morse.exe
TEST_ASYNC_MORSE = $(BUILD_DIR)/test_async_morse.exe
TEST_VFO = $(BUILD_DIR)/test_vfo.exe

.PHONY: all test clean setup help
.DEFAULT_GOAL := all

# Main targets
all: setup test

# Create build directory
setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

# Run all tests
test: test-async-morse

# Quick test - just compile and run AsyncMorse tests
quick: $(TEST_ASYNC_MORSE)
	@echo === Running AsyncMorse Tests ===
	@$(TEST_ASYNC_MORSE)

# Individual test targets
test-utils: $(TEST_UTILS)
	@echo === Running utils tests ===
	@$(TEST_UTILS)

test-async-morse: $(TEST_ASYNC_MORSE)
	@echo === Running AsyncMorse tests ===
	@$(TEST_ASYNC_MORSE)

# Build rules
$(TEST_UTILS): test_utils.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	@echo Compiling utils tests...
	@$(CC) $(CFLAGS) -o $@ $^

$(TEST_ASYNC_MORSE): test_async_morse.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	@echo Compiling AsyncMorse tests...
	@$(CC) $(CFLAGS) -o $@ $^

$(TEST_MORSE): test_morse.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	@echo Compiling morse tests...
	@$(CC) $(CFLAGS) -o $@ $^

$(TEST_VFO): test_vfo.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	@echo Compiling VFO tests...
	@$(CC) $(CFLAGS) -o $@ $^

# Development targets
compile-only: $(TEST_ASYNC_MORSE)
	@echo Compilation successful!

# Clean build artifacts
clean:
	@echo Cleaning build artifacts...
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@if exist "*.exe" del /q "*.exe"

# Setup Unity (download if needed)
unity-setup:
	@echo Checking for Unity framework...
	@if not exist "$(UNITY_DIR)" (echo Unity not found. Please download from https://github.com/ThrowTheSwitch/Unity && echo Extract to $(UNITY_DIR)/ directory && exit 1)
	@echo Unity framework found!

# Install Unity automatically (requires curl or wget)
unity-install:
	@echo Downloading Unity framework...
	@if not exist "$(TEST_DIR)" mkdir "$(TEST_DIR)"
	@powershell -Command "Invoke-WebRequest -Uri 'https://github.com/ThrowTheSwitch/Unity/archive/refs/heads/master.zip' -OutFile 'unity.zip'"
	@powershell -Command "Expand-Archive -Path 'unity.zip' -DestinationPath '$(TEST_DIR)'"
	@move "$(TEST_DIR)\Unity-master" "$(UNITY_DIR)"
	@del unity.zip
	@echo Unity framework installed!

help:
	@echo FluxTune Test Build System
	@echo ========================
	@echo Available targets:
	@echo.
	@echo   all           - Setup and run all tests (default)
	@echo   test          - Run all tests
	@echo   quick         - Quick compile and run AsyncMorse tests
	@echo   compile-only  - Compile tests without running
	@echo.
	@echo   test-utils       - Run utility function tests
	@echo   test-async-morse - Run AsyncMorse tests
	@echo   test-morse       - Run morse code tests  
	@echo   test-vfo         - Run VFO tests
	@echo.
	@echo   clean         - Clean build artifacts
	@echo   setup         - Create build directory
	@echo   unity-setup   - Check for Unity framework
	@echo   unity-install - Download and install Unity framework
	@echo.
	@echo Example usage:
	@echo   make quick           # Fast development cycle
	@echo   make test-async-morse # Test specific module
