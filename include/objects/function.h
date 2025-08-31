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
};

struct Closure {
  struct Function *function;
  size_t references;
};

struct Function *function_init(size_t num_args, size_t num_locals);
struct Closure *closure_init(struct Function *function);
void closure_call(const struct Closure *closure, struct VM *vm);
void closure_return(struct VM *vm);
void closure_retain(struct Closure *closure);
void closure_release(struct Closure *closure);

#endif 
