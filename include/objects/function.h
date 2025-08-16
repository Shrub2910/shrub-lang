#ifndef FUNCTION_H
#define FUNCTION_H

#include <stddef.h>

#include "vm/instruction_buffer.h"

struct VM;
struct Value;

struct Function {
  struct InstructionBuffer *instruction_buffer;
  size_t num_args;
  size_t num_locals;
  size_t references;
};

struct Function *function_init(size_t num_args, size_t num_locals);
void function_call(struct Function *function, struct VM *vm);
void function_return(struct VM *vm);
void function_retain(struct Function *function);
void function_release(struct Function *function);

#endif 
