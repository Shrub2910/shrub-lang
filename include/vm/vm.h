#include <stdint.h>
#include "vm/instruction_buffer.h"
#include "vm/values.h"
#include "vm/variables/environment.h"

#ifndef VM_H
#define VM_H

struct VM {
  struct Stack *stack;

  struct Value *constants;
  size_t constant_count;

  struct InstructionBuffer *instruction_buffer;

  struct StackFrame *stack_frame;

  uint8_t *program_counter;
};

// VM methods

struct VM *vm_init();
void vm_exec(struct VM *vm);
void vm_free(struct VM *vm);

void vm_add_const(struct VM *vm, const struct Value value);
void vm_free_consts(struct VM *vm);

// VM Setup

#define CONST_POOL_SIZE 255
#define STACK_SIZE 255

#endif 
