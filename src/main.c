#include <stdio.h>
#include "vm/vm.h"

int main() {
  struct VM *vm = vm_init();
  vm_exec(vm);
  vm_free(vm);

  return 0;
}
