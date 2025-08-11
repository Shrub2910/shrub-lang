#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"

int main() {
  struct VM *vm = vm_init();

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

  vm_exec(vm);
  vm_free(vm);

  printf("\n");

  return 0;
}
