#ifndef FUNCTION_H
#define FUNCTION_H

#include <stddef.h>

#include "vm/instruction_buffer.h"
#include "vm/variables/upvalues.h"

struct VM;
struct Value;

struct Function {
  struct InstructionBuffer *instruction_buffer;
  size_t num_args;
  size_t num_locals;

  struct Value *constants;
  size_t constant_count;

  struct Function **functions;
  size_t function_count;

  struct UpvalueDesc *upvalue_descriptors;
  size_t upvalue_descriptor_count;
};

struct Closure {
  struct Function *function;
  size_t references;

  struct Upvalue **upvalues;
  size_t upvalues_count;
};

struct Function *function_init(void);
struct Closure *closure_init(struct Function *function);

void closure_call(struct Closure *closure, struct VM *vm);
void closure_return(struct VM *vm);
void closure_retain(struct Closure *closure);
void closure_release(struct Closure *closure);

#endif 
