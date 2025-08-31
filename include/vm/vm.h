#ifndef VM_H
#define VM_H

#include <stdint.h>
#include "vm/instruction_buffer.h"
#include "vm/values.h"


struct VM {
  struct Stack *stack;

  struct Value *constants;
  size_t constant_count;

  struct Function **functions;
  size_t function_count;

  struct InstructionBuffer *instruction_buffer;

  struct StackFrame *stack_frame;

  uint8_t *program_counter;
};

// VM methods

struct VM *vm_init(void);
void vm_exec(struct VM *vm);
void vm_free(struct VM *vm);

void vm_add_const(struct VM *vm, struct Value value);
void vm_free_consts(struct VM *vm);

void vm_add_function(struct VM *vm, struct Function *function);
void vm_free_functions(struct VM *vm);

// VM Setup

#define CONST_POOL_SIZE 255
#define FUNCTION_POOL_SIZE 255
#define STACK_SIZE 255

#endif 
