#ifndef INSTRUCTION_BUFFER_H
#define INSTRUCTION_BUFFER_H

#include <stdint.h>

struct InstructionBuffer {
  uint8_t *buffer;
  size_t used;
  size_t size;
};

void vm_init_instruction_buffer(struct InstructionBuffer *instruction_buffer, size_t initial_size);
void vm_insert_instruction_buffer(struct InstructionBuffer *instruction_buffer, uint8_t instruction);
void vm_free_instruction_buffer(struct InstructionBuffer *instruction_buffer);

#endif // !INSTRUCTION_BUFFER_H 


