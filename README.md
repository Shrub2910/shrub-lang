# Shrub Lang
Shrub lang is a stack-based programming language designed for learning and experimentation.
The project implements a virtual machine (VM) that runs shrub lang bytecode.

## Current Features
* Loading constants onto the VM stack 
* Printing numberic values from the stack to the console
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
* `HALT` - Halts execution of the program 

Example usage see (src/main.c):
```c 
INSERT_CONST_VALUES(
    vm,
    NUMBER(10),
    NUMBER(5),
    NUMBER(17),
    NUMBER(3),
    NUMBER(12)
  );

  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 3,
    LOAD_CONST, 1,
    ADD,
    LOAD_CONST, 2,
    MUL,
    LOAD_CONST, 0,
    DIV,
    LOAD_CONST, 4,
    SUB,
    PRINT,
    HALT
  ); // ((3 + 5) * 17 / 10) - 12 
```

## Planned Features 
* Support for more data types
* Logic operations
* Control flow constructs
* A front end
* Error handling and debuggin tools

## Contributing
Contributions and suggestions are welcome. 
Feel free to open issues or pull requests.

