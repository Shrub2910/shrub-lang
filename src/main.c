#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"
#include "utils/operand_conversion.h"
int main() {
  struct VM *vm = vm_init();
  
  // All the constants the program will use
  INSERT_CONST_VALUES(
    vm,
    NUMBER(0),
    NUMBER(1),
    NUMBER(20),
    STRING("\nCOUNT TO 20: \n", 15),
    STRING("\nFIBONACCI SEQUENCE: \n", 22)
  );
  
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

  vm_exec(vm);

  // Clean up
  vm_free(vm);

  printf("\n");

  return 0;
}
