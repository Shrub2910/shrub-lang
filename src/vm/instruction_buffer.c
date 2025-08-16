#include <stdlib.h>
#include "vm/instruction_buffer.h"
#include "error/error.h"
#include "error/error_types.h"

// Initialises the instruction buffer and sets its size to be what is defined in instruction_buffer.h
struct InstructionBuffer *vm_init_instruction_buffer() {
  struct InstructionBuffer *instruction_buffer = malloc(sizeof(struct InstructionBuffer));

  instruction_buffer->buffer = 
    malloc(INSTRUCTION_BUFFER_INITIAL_CAPACITY * sizeof(uint8_t));

  if (!instruction_buffer) {
    error_throw(MALLOC_ERROR, "Failed to allocate instruction buffer");
  }

  instruction_buffer->used = 0;
  instruction_buffer->size = INSTRUCTION_BUFFER_INITIAL_CAPACITY;

  return instruction_buffer;
}

// Pushes an instruction to the buffer and allocates more memory if needed
void vm_insert_instruction_buffer(struct InstructionBuffer *instruction_buffer, uint8_t instruction) {
  if (instruction_buffer->used == instruction_buffer->size) {
    instruction_buffer->size *= 2;
    instruction_buffer->buffer = realloc(
      instruction_buffer->buffer,
      instruction_buffer->size * sizeof(uint8_t)
    );

    if (!instruction_buffer) {
      error_throw(MALLOC_ERROR, "Failed to reallocate instruction buffer");
    }

  }

  instruction_buffer->buffer[instruction_buffer->used++] = instruction;
}

// Clean up 
void vm_free_instruction_buffer(struct InstructionBuffer *instruction_buffer) {
  free(instruction_buffer->buffer);
  instruction_buffer->buffer = NULL;
  instruction_buffer->size = instruction_buffer->used = 0;
  free(instruction_buffer);
}
