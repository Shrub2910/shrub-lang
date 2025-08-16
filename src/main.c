#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"
#include "utils/operand_conversion.h"
int main() {
  struct VM *vm = vm_init();
  
  struct Value function_value = FUNCTION(1, 0);

  // All the constants the program will use
  INSERT_CONST_VALUES(
    vm,
    NUMBER(10),
    function_value,
    NUMBER(1)
  );

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
  
  // Prints numbers from 0 to 10
  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
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
