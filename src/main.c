#include <stdio.h>
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/opcodes.h"

int main() {
  struct VM *vm = vm_init();

  INSERT_CONST_VALUES(
    vm,
    NUMBER(10),
    NUMBER(5)
  );

  INSERT_INSTRUCTIONS(
    vm->instruction_buffer,
    LOAD_CONST, 0,
    LOAD_CONST, 1,
    SUB,
    PRINT,
    HALT
  );

  vm_exec(vm);
  vm_free(vm);

  printf("\n");

  return 0;
}
