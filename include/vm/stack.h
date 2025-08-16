#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include "vm/values.h"

struct Stack{
  struct Value *stack_pointer;
  size_t used;
  size_t size;
};

struct Stack *vm_init_stack();
void vm_push_stack(struct Stack *stack, struct Value value);
struct Value vm_pop_stack(struct Stack *stack);
void vm_free_stack(struct Stack *stack);

// Stack setup

#define STACK_SIZE 255

#endif
