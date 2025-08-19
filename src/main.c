#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"
#include "utils/operand_conversion.h"

#define NUM_ARGS_FACTORIAL 1
#define NUM_LOCALS_FACTORIAL 0
#define NUM_ARGS_FIBONACCI 1
#define NUM_LOCALS_FIBONACCI 0

int main(void) {
  // Function call to get a preconfigured Virtual Machine
  struct VM *vm = vm_init();

  /*
    These function values are stored inside the constants pool of the virtual machine
    Normally they would be pushed to the stack and stored under some sort of global identifier
    In these examples, since they are only used once, are just called immediately
  */
  const struct Value factorial_function = FUNCTION(NUM_ARGS_FACTORIAL, NUM_LOCALS_FACTORIAL);
  const struct Value fibonacci_function = FUNCTION(NUM_ARGS_FIBONACCI, NUM_LOCALS_FIBONACCI);

  /*
   These are all the constants that are used by the program
   Given that we can only read each bite at a time we can't simply load the values as operands
   Through the use of macros the necessary value structs can be easily constructed
   They are indexed via the LOAD_CONST <index> instruction
  */
  INSERT_CONST_VALUES(
    vm,
    NUMBER(170),
    factorial_function,
    NUMBER(1),
    NUMBER(2),
    fibonacci_function,
    // Can't calculate that high of a fibonacci number since recursion is resource intensive
    NUMBER(20),
    STRING("Calculation of 170!:\n", 21),
    STRING("\nCalculation of fibonacci(20):\n", 31)
  );

  INSERT_INSTRUCTIONS(
    // Macro needs to know where to insert instructions
    factorial_function.function->instruction_buffer,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    EQUAL,
    // Jump if false is basically and if statement
    JUMP_IF_FALSE, FROM_SIGNED_WORD(3), // Since it reads a byte at a time this macro simplifies the process
    LOAD_ARG, 1,
    RETURN, // Early return to prevent infinite recursion
    LOAD_ARG, 1,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    SUB,
    LOAD_CONST, 1,
    CALL,
    MUL,
    RETURN
  );

  INSERT_INSTRUCTIONS(
    fibonacci_function.function->instruction_buffer,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    LESS_EQUAL,
    JUMP_IF_FALSE, FROM_SIGNED_WORD(3),
    LOAD_ARG, 1,
    RETURN,
    LOAD_ARG, 1,
    LOAD_CONST, 2,
    SUB,
    LOAD_CONST, 4,
    CALL,
    LOAD_ARG, 1,
    LOAD_CONST, 3,
    SUB,
    LOAD_CONST, 4,
    CALL,
    ADD,
    RETURN
  );
  
  /*
    The virtual machine also contains an address to an instruction buffer
    This is for instructions on the outer scope
    This program simply calls the two functions abover and prints what they return
    There is also a halt instruction so the VM knows to stop executing
  */
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 6,
    PRINT,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
    CALL,
    PRINT,
    LOAD_CONST, 7,
    PRINT,
    LOAD_CONST, 5,
    LOAD_CONST, 4,
    CALL,
    PRINT,
    HALT
  ); 

  // This will execute the program loaded into the VM's instruction buffer
  // The program will not execute until this is called
  vm_exec(vm);

  // Clean up
  vm_free(vm);

  printf("\n");

  return 0;
}
