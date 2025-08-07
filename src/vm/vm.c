#include <stdlib.h>
#include "vm/instruction_buffer.h"
#include "vm/opcodes.h"
#include "vm/vm.h"

struct VM *vm_init() {

  struct InstructionBuffer *instruction_buffer = (struct InstructionBuffer*) malloc(sizeof(struct InstructionBuffer));
  vm_init_instruction_buffer(instruction_buffer, 1);
  vm_insert_instruction_buffer(instruction_buffer, HALT);

  struct VM *vm = (struct VM *) malloc(sizeof(struct VM));
  vm->instruction_buffer = instruction_buffer;
  vm->program_counter = vm->instruction_buffer->buffer;

  return vm;
}

void vm_exec(struct VM *vm) {

  for (;;) {
    uint8_t current_instruction = *(vm->program_counter++);
    switch (current_instruction) {
      case HALT:
      return;
    }
 
  } 
}

void vm_free(struct VM *vm) {
  vm_free_instruction_buffer(vm->instruction_buffer);
  vm->instruction_buffer = NULL;

  vm->program_counter = NULL;

  free(vm);
}
