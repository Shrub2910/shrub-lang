#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "vm/instruction_buffer.h"
#include "vm/values.h"


struct VM {
  struct Stack *stack;

  struct StackFrame *stack_frame;

  uint8_t *program_counter;
};

// VM methods

struct VM *vm_init(struct Closure *main_closure);
void vm_exec(struct VM *vm);
void vm_free(struct VM *vm);

// VM Setup

#define CONST_POOL_SIZE 255
#define FUNCTION_POOL_SIZE 255
#define STACK_SIZE 255

#endif 
