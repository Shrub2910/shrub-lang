# Shrub Lang
Shrub lang is a stack-based programming language designed for learning and experimentation.
The project implements a virtual machine (VM) that runs shrub lang bytecode.

## Current Features
* Loading constants onto the VM stack 
* Printing numberic values from the stack to the console
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
* `HALT` - Halts execution of the program 

Example usage see (src/main.c):
```c 
INSERT_CONST_VALUES(
    vm,
    NUMBER(69),
    NUMBER(420),
  );

  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 0,
    PRINT,
    LOAD_CONST, 1,
    PRINT,
    HALT
  );
```

## Planned Features 
* Support for more data types
* Arithmetic and logic operations
* Control flow constructs
* A front end
* Error handling and debuggin tools

## Contributing
Contributions and suggestions are welcome. 
Feel free to open issues or pull requests.

