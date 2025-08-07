#ifndef INSTRUCTION_BUFFER_H
#define INSTRUCTION_BUFFER_H

#include <stdint.h>

struct InstructionBuffer {
  uint8_t *buffer;
  size_t used;
  size_t capacity;
}

void vm_init_instruction_buffer(struct instruction*)
void vm_insert_instruction_buffer(struct InstructionBuffer*, uint8_t instruction);
void vm_free_instruction_buffer(struct InstructionBuffer*)

#endif // !INSTRUCTION_BUFFER_H 


