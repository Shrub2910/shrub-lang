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
    LOAD_CONST, 3,
    PRINT,
    LOAD_CONST, 0,
    STORE_VAR, 0,
    LOAD_VAR, 0,
    PRINT,
    LOAD_VAR, 0,
    LOAD_CONST, 1,
    ADD,
    STORE_VAR, 0, 
    LOAD_VAR, 0, 
    LOAD_CONST, 2,
    LESS_EQUAL,
    JUMP_IF_TRUE, FROM_SIGNED_WORD(-18),
    HALT
  ); 
  
  vm_exec(vm);

  // Clean up
  vm_free(vm);

  printf("\n");

  return 0;
}
