#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"
#include "utils/operand_conversion.h"
int main() {
  struct VM *vm = vm_init();

  INSERT_CONST_VALUES(
    vm,
    NUMBER(0),
    NUMBER(1),
    NUMBER(10)
  );

  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    PUSH_SCOPE, 1,
    LOAD_CONST, 0,
    STORE_VAR, 0,
    LOAD_VAR, 0, 0,
    PRINT,
    LOAD_VAR, 0, 0,
    LOAD_CONST, 1,
    ADD,
    STORE_VAR, 0,
    LOAD_VAR, 0, 0,
    LOAD_CONST, 2,
    EQUAL,
    JUMP_IF_FALSE, FROM_SIGNED_WORD(-21),
    POP_SCOPE, 
    HALT
  ); 

  vm_exec(vm);
  vm_free(vm);

  printf("\n");

  return 0;
}
