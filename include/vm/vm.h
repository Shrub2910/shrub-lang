#include <stdint.h>
#include "vm/instruction_buffer.h"

#ifndef VM_H
#define VM_H

struct VM {
  struct InstructionBuffer *instruction_buffer;
  uint8_t *program_counter;
};

struct VM *vm_init();
void vm_exec(struct VM *vm);
void vm_free(struct VM *vm);

#endif // !VM_H
