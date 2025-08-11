#include <stdint.h>
#include <stddef.h>

#ifndef INSTRUCTION_BUFFER_H
#define INSTRUCTION_BUFFER_H

struct InstructionBuffer {
  uint8_t *buffer;
  size_t used;
  size_t size;
};

struct InstructionBuffer *vm_init_instruction_buffer();
void vm_insert_instruction_buffer(struct InstructionBuffer *instruction_buffer, uint8_t instruction);
void vm_free_instruction_buffer(struct InstructionBuffer *instruction_buffer);

// Instruction Buffer Setup

#define INSTRUCTION_BUFFER_INITIAL_CAPACITY 64

// Instruction Buffer Utils

// Macro for quickly inserting instructions into the vm
// Should only be used for testing
#define INSERT_INSTRUCTIONS(buf, ...)             \
  do {                                            \
    uint8_t temp[] = {__VA_ARGS__};               \
    for (size_t i = 0; i < sizeof(temp); ++i) {   \
      vm_insert_instruction_buffer(buf, temp[i]); \
    }                                             \
  } while(0)


#endif  


