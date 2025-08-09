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

  vm_exec(vm);
  vm_free(vm);

  printf("\n");

  return 0;
}
