#include <stdlib.h>
#include "vm/instruction_buffer.h"

struct InstructionBuffer *vm_init_instruction_buffer() {
  struct InstructionBuffer *instruction_buffer = malloc(sizeof(struct InstructionBuffer));

  instruction_buffer->buffer = 
    malloc(INSTRUCTION_BUFFER_INITIAL_CAPACITY * sizeof(uint8_t));
  instruction_buffer->used = 0;
  instruction_buffer->size = INSTRUCTION_BUFFER_INITIAL_CAPACITY;

  return instruction_buffer;
}

void vm_insert_instruction_buffer(struct InstructionBuffer *instruction_buffer, uint8_t instruction) {
  if (instruction_buffer->used == instruction_buffer->size) {
    instruction_buffer->size *= 2;
    instruction_buffer->buffer = realloc(
      instruction_buffer->buffer,
      instruction_buffer->size * sizeof(uint8_t)
    );
  }

  instruction_buffer->buffer[instruction_buffer->used++] = instruction;
}

void vm_free_instruction_buffer(struct InstructionBuffer *instruction_buffer) {
  instruction_buffer->buffer = NULL;
  instruction_buffer->size = instruction_buffer->used = 0;
  free(instruction_buffer);
}
