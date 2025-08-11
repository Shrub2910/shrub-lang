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
Currently the program supports only simple programs comprised of:
* `LOAD_CONST <index>` - Pushes a constant to the stack 
* `PRINT` - Pops then prints the top of the stack
* `ADD` - Pops two numbers of the stack, adds them, and then pushes the result,
* `SUB` - Pops two numbers of the stack, subtracts them, and then pushes the result,
* `MUL` - Pops two numbers of the stack, multiplies them, and then pushes the result,
* `DIV` - Pops two numbers of the stack, divides them, and then pushes the result,
* `STORE_VAR <offset>` - Pops a value from the stack, stores it in the current scope + offset
* `LOAD_VAR <depth> <offset>` - Pushes onto the stack the value at (current scope - depth) + offset
* `PUSH_SCOPE <size>` - Pushes a new scope onto the environment, size determines number of variables 
* `POP_SCOPE` - Pops the current scope off the environment 
* `HALT` - Halts execution of the program 

Example usage see (src/main.c):
```c 
INSERT_CONST_VALUES(
    vm,
    NUMBER(3),
    NUMBER(7),
    NUMBER(20)
  );

  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    PUSH_SCOPE, 1,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
    ADD,
    STORE_VAR, 0,
    PUSH_SCOPE, 1,
    LOAD_CONST, 2,
    STORE_VAR, 0,
    LOAD_VAR, 0, 0,
    PRINT,
    POP_SCOPE,
    LOAD_VAR, 0, 0,
    PRINT,
    POP_SCOPE,    
    HALT
  ); 
```

## Planned Features 
* Support for more data types
* Logic operations
* Control flow constructs
* A front end
* Error handling and debugging tools

## Contributing
Contributions and suggestions are welcome. 
Feel free to open issues or pull requests.

