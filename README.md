# Shrub Lang
Shrub lang is a stack-based programming language designed for learning and experimentation.
The project implements a virtual machine (VM) that runs shrub lang bytecode.

## Current Features
* Loading constants onto the VM stack (numbers, strings, booleans, functions)
* Basic reference counting for strings and functions
* Printing values from the stack to the console
* Storing variables
* Calling functions 
* Performing arithmetic operations
* Halting program execution 

## Getting Started
### Build
```bash
make
```
### Running a program 
For list of instructions see include/vm/opcodes.h
All operands are 1 byte unless specified
```
HALT (0x00)
PRINT (0x01)
LOAD_CONST (0x02) <index>
ADD (0x03)
SUB (0x04)
MUL (0x05)
DIV (0x06)
STORE_VAR (0x07) <offset>
LOAD_VAR (0x08) <offset>
CALL (0x09) 
RETURN (0x0A)
JUMP (0x0B) <signed word offset>
JUMP_IF_TRUE (0x0C) <signed word offset>
JUMP_IF_FALSE (0x0D) <signed word offset>
EQUAL (0x0E)
NOT_EQUAL (0x0F)
GREATER (0x10)
LESS (0x11)
GREATER_EQUAL (0x12)
LESS_EQUAL (0x13)
LOAD_ARG (0x14) <offset>
```

Example usage see (src/main.c):
```c 
  INSERT_CONST_VALUES(
    vm,
    NUMBER(10),
    function_value,
    NUMBER(1)
  );
  
  // Insert instructions into recursive function that generates 10!
  INSERT_INSTRUCTIONS(
    function_value.function->instruction_buffer,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    EQUAL,
    JUMP_IF_FALSE, FROM_SIGNED_WORD(3),
    LOAD_ARG, 1,
    RETURN,
    LOAD_ARG, 1,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    SUB,
    LOAD_CONST, 1,
    CALL,
    MUL,
    RETURN
  );
  
  // Insert instructions into top level scope
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
    CALL,
    PRINT,
    HALT
  ); 
```

## Planned Features 
* Support for more data types
* Logic operations
* A front end / compiler
* Error handling and debugging tools

## Contributing
Contributions and suggestions are welcome. 
Feel free to open issues or pull requests.

