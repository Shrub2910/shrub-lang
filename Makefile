# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -Iinclude -g

# Directories
SRC_DIR := src
VM_DIR := $(SRC_DIR)/vm
ERROR_DIR := $(SRC_DIR)/error
OBJ_DIR := build/objects
BIN_DIR := build/output

# Source files
SRCS := $(SRC_DIR)/main.c $(VM_DIR)/vm.c $(VM_DIR)/instruction_buffer.c $(VM_DIR)/stack.c $(ERROR_DIR)/error.c
# Object files (replace .c with .o and prefix with OBJ_DIR)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Target executable name
TARGET := $(BIN_DIR)/my_program

# Default target
all: $(TARGET)

# Link objects into final executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files to object files
# This rule handles .c -> .o compilation for both src/ and src/vm/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/vm/%.o: $(VM_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object and binary directories if not present
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

