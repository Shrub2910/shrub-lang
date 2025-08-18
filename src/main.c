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
  struct VM *vm = vm_init();

  const struct Value factorial_function = FUNCTION(NUM_ARGS_FACTORIAL, NUM_LOCALS_FACTORIAL);
  const struct Value fibonacci_function = FUNCTION(NUM_ARGS_FIBONACCI, NUM_LOCALS_FIBONACCI);

  // Inserts constant values to be used by the whole program
  INSERT_CONST_VALUES(
    vm,
    NUMBER(12),
    factorial_function,
    NUMBER(1),
    NUMBER(2),
    fibonacci_function
  );
  
  // Insert instructions into recursive function that generates 10!
  INSERT_INSTRUCTIONS(
    factorial_function.function->instruction_buffer,
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
  
  // Insert instructions into top level scope
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
    CALL,
    PRINT,
    LOAD_CONST, 0,
    LOAD_CONST, 4,
    CALL,
    PRINT,
    HALT
  ); 
  
  vm_exec(vm);

  // Clean up
  vm_free(vm);

  printf("\n");

  return 0;
}
