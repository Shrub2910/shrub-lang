cmake_minimum_required(VERSION 3.20)
project(shrub_lang C)

# Use C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add the main include directory (project headers)
include_directories(./include)

# Collect all source files
set(SOURCES
        src/main.c

        src/error/error.c

        src/objects/function.c
        src/objects/string.c

        src/parser/lexer.c

        src/vm/instruction_buffer.c
        src/vm/stack.c
        src/vm/vm.c
        src/vm/variables/stack_frame.c
)

# Create executable
add_executable(shrub_lang ${SOURCES})
