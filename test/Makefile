# Makefile for Unity-based tests

# Compiler settings
CC = g++
CFLAGS = -DNATIVE_BUILD -Iinclude -Inative -Iunity -std=c++11 -Wall

# Directories
SRC_DIR = src
TEST_DIR = test
UNITY_DIR = $(TEST_DIR)/unity
NATIVE_DIR = native
BUILD_DIR = build/test

# Unity source (assumes Unity is downloaded to test/unity/)
UNITY_SRC = $(UNITY_DIR)/unity.c

# Common source files needed for tests
COMMON_SOURCES = $(SRC_DIR)/utils.cpp $(SRC_DIR)/buffers.cpp $(SRC_DIR)/saved_data.cpp
MOCK_SOURCES = $(NATIVE_DIR)/mock_arduino.cpp $(NATIVE_DIR)/mock_wire.cpp $(NATIVE_DIR)/mock_eeprom.cpp $(NATIVE_DIR)/mock_ht16k33disp.cpp

# Test executables
TEST_UTILS = $(BUILD_DIR)/test_utils.exe
TEST_MORSE = $(BUILD_DIR)/test_morse.exe
TEST_VFO = $(BUILD_DIR)/test_vfo.exe

.PHONY: all test clean setup

all: setup test

# Create build directory
setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

# Run all tests
test: test-utils

# Individual test targets
test-utils: $(TEST_UTILS)
	@echo Running utils tests...
	@$(TEST_UTILS)

$(TEST_UTILS): $(TEST_DIR)/test_utils.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	$(CC) $(CFLAGS) -o $@ $^

# Future test targets (templates)
test-morse: $(TEST_MORSE)
	@echo Running morse tests...
	@$(TEST_MORSE)

$(TEST_MORSE): $(TEST_DIR)/test_morse.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	$(CC) $(CFLAGS) -o $@ $^

test-vfo: $(TEST_VFO)
	@echo Running VFO tests...
	@$(TEST_VFO)

$(TEST_VFO): $(TEST_DIR)/test_vfo.cpp $(UNITY_SRC) $(COMMON_SOURCES) $(MOCK_SOURCES) | setup
	$(CC) $(CFLAGS) -o $@ $^

# Clean build artifacts
clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"

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
	@echo Available targets:
	@echo   test        - Run all tests
	@echo   test-utils  - Run utility function tests
	@echo   test-morse  - Run morse code tests
	@echo   test-vfo    - Run VFO tests
	@echo   clean       - Clean build artifacts
	@echo   setup       - Create build directory
	@echo   unity-setup - Check for Unity framework
	@echo   unity-install - Download and install Unity framework
