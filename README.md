# Shrub Lang
Shrub lang is a stack-based programming language designed for learning and experimentation.
The project implements a virtual machine (VM) that runs shrub lang bytecode.

## Current Features
* Loading constants onto the VM stack (numbers, strings)
* Basic reference counting for strings 
* Printing values to the stack from the console
* Storing variables with scoping
* Performing arithmetic operations
* Halting program execution 

## Getting Started
### Build
```bash
make
```
### Running a program 
For list of instructions see include/vm/opcodes.h
```
HALT 0x00
PRINT 0x01
LOAD_CONST 0x02 <index>
ADD 0x03
SUB 0x04
MUL 0x05
DIV 0x06
STORE_VAR 0x07 <depth> <offset>
LOAD_VAR 0x08 <depth> <offset>
PUSH_SCOPE 0x09 <size>
POP_SCOPE 0x0A
JUMP 0x0B <offset>
JUMP_IF_TRUE 0x0C <offset>
JUMP_IF_FALSE 0x0D <offset>
EQUAL 0x0E
NOT_EQUAL 0x0F
GREATER 0x10
LESS 0x11
GREATER_EQUAL 0x12
LESS_EQUAL 0x13
```

Example usage see (src/main.c):
```c 
// Prints numbers from 0 to 10
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    PUSH_SCOPE, 1,
    LOAD_CONST, 3,
    PRINT,
    LOAD_CONST, 0,
    STORE_VAR, 0, 0,
    LOAD_VAR, 0, 0,
    PRINT,
    LOAD_VAR, 0, 0,
    LOAD_CONST, 1,
    ADD,
    STORE_VAR, 0, 0,
    LOAD_VAR, 0, 0,
    LOAD_CONST, 2,
    LESS_EQUAL,
    JUMP_IF_TRUE, FROM_SIGNED_WORD(-22),
    POP_SCOPE, 
  ); 
  
  // Fibonacci sequence 
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    PUSH_SCOPE, 2,
    LOAD_CONST, 4,
    PRINT,
    LOAD_CONST, 1, 
    LOAD_CONST, 0, 
    STORE_VAR, 0, 0, // x = 1
    STORE_VAR, 0, 1, // y = 1
    PUSH_SCOPE, 2,
    LOAD_CONST, 0,
    STORE_VAR, 0, 0, // i = 0
    LOAD_VAR, 1, 0,
    STORE_VAR, 0, 1, // temp = x
    LOAD_VAR, 1, 1,
    STORE_VAR, 1, 0, // a = b 
    LOAD_VAR, 0, 1,
    LOAD_VAR, 1, 1,
    ADD,
    STORE_VAR, 1, 1, // b = a + b 
    LOAD_CONST, 1, 
    LOAD_VAR, 0, 0,
    ADD,
    STORE_VAR, 0, 0, // i += 1
    LOAD_VAR, 1, 0,
    PRINT,
    LOAD_VAR, 0, 0,
    LOAD_CONST, 2,
    LESS,
    JUMP_IF_TRUE, FROM_SIGNED_WORD(-44),
    POP_SCOPE,
    POP_SCOPE,
    HALT
  ); 
```

## Planned Features 
* Support for more data types
* Logic operations
* A front end
* Error handling and debugging tools

## Contributing
Contributions and suggestions are welcome. 
Feel free to open issues or pull requests.

