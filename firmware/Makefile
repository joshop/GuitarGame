# Project Configuration
PROJECT_NAME = guitar-game
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0
BUILD_NUMBER = 1

# Build Configuration
BUILD_TYPE ?= Debug
VERBOSE ?= 0
PARALLEL_JOBS ?= 4

# Toolchain Configuration
ARM_TOOLCHAIN_PREFIX = arm-none-eabi-
CC = $(ARM_TOOLCHAIN_PREFIX)gcc
CXX = $(ARM_TOOLCHAIN_PREFIX)g++
AS = $(ARM_TOOLCHAIN_PREFIX)as
LD = $(ARM_TOOLCHAIN_PREFIX)ld
OBJCOPY = $(ARM_TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(ARM_TOOLCHAIN_PREFIX)objdump
SIZE = $(ARM_TOOLCHAIN_PREFIX)size
GDB = $(ARM_TOOLCHAIN_PREFIX)gdb

# Host tools
PYTHON = python3
STLINK = st-flash
OPENOCD = openocd

# Directory Structure
BUILD_DIR = build
SRC_DIR = src
DRIVERS_DIR = drivers
INCLUDE_DIR = include
DOCS_DIR = docs


CFLAGS = -Wall -Wextra -Werror -Wno-analyzer-infinite-loop -Wno-unused-parameter -Wno-analyzer-null-dereference -std=c23 -fdata-sections -ffunction-sections -fanalyzer -mcpu=cortex-m0plus -mthumb
CPPFLAGS = -DSTM32C071xx=1 -I$(DRIVERS_DIR)/stm32c0xx -I$(DRIVERS_DIR)/common -I$(INCLUDE_DIR)


# Debug/Release specific flags
ifeq ($(BUILD_TYPE),Debug)
    OPT_FLAGS = -O2 -g3 -DDEBUG=1
else
    OPT_FLAGS = -O2 -g1 -DNDEBUG=1
endif

# Linker Flags
LDFLAGS = -T linker/STM32C071FBPX_FLASH.ld -Wl,--gc-sections -Wl,--print-memory-usage

STARTUP = $(SRC_DIR)/target/startup.s

# Source Files
SOURCES = $(wildcard $(SRC_DIR)/*/*.c) $(wildcard $(SRC_DIR)/*/*.s)
DRIVER_SOURCES = $(wildcard $(DRIVERS_DIR)/stm32c0xx/*.c)

# Object Files
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o) $(DRIVER_SOURCES:%.c=%.o)

# Target Binaries
TARGET_ELF = $(BUILD_DIR)/guitar_game.elf

# All targets
ALL_TARGETS = guitar_game

# Default target
.PHONY: all
all: $(ALL_TARGETS)

# Main Board Target
.PHONY: guitar_game
guitar_game: $(TARGET_ELF)

$(TARGET_ELF): $(OBJECTS) | $(BUILD_DIR)
	@echo "Linking ELF..,"
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/guitar_game.bin
	$(OBJCOPY) -O ihex $@ $(BUILD_DIR)/guitar_game.hex
	$(SIZE) $@
	@echo "Successfully linked firmware."

# Main board object compilation
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(CPPFLAGS) -I$(SRC_DIR)/inc $(OPT_FLAGS) -c $< -o $@

# Create build directories
$(BUILD_DIR):
	@mkdir -p $@

# Flash targets
.PHONY: flash
flash: $(TARGET_ELF)
	$(STLINK)  --reset --connect-under-reset --mass-erase write $(BUILD_DIR)/guitar_game.bin 0x08000000

# Debug targets
.PHONY: debug
debug: $(TARGET_ELF)
	$(GDB) $< -ex "target extended-remote localhost:3333"

# Clean targets
.PHONY: clean clean_all
clean:
	rm -rf $(BUILD_DIR)
drivers_clean:
	rm -rf $(DRIVERS_DIR)/*/*.o

clean_all: clean drivers_clean

# Verbose output control
ifeq ($(VERBOSE),1)
    Q =
else
    Q = @
endif

# Default goal
.DEFAULT_GOAL := all 
