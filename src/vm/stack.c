#include <stdlib.h>
#include <stdio.h>
#include "vm/stack.h"
#include "vm/values.h"
#include "error/error.h"
#include "error/error_types.h"
#include "objects/string.h"
#include "objects/function.h"

struct Stack *vm_init_stack() {
  struct Stack *stack = (struct Stack *) malloc(sizeof(struct Stack));

  if (!stack) {
    error_throw(MALLOC_ERROR, "Failed to allocate stack");
  }

  stack->stack_pointer = (struct Value *) malloc(STACK_SIZE * sizeof(struct Value));

  if (!stack->stack_pointer) {
    error_throw(MALLOC_ERROR, "Failed to allocate value array");
  }

  stack->used = 0;
  stack->size = STACK_SIZE;

  return stack;
}

void vm_push_stack(struct Stack *stack, struct Value value) {
  if (stack->used == stack->size) {
    error_throw(STACK_ERROR, "Stack Overflow");
  }

  stack->stack_pointer[stack->used++] = value;

  if (value.type == TYPE_STRING) {
    string_retain(value.string);
  } else if (value.type == TYPE_FUNCTION) {
    function_retain(value.function);
  }
}

struct Value vm_pop_stack(struct Stack *stack) {
  if (stack->used == 0) {
    error_throw(STACK_ERROR, "Stack underflow");
  }

  struct Value value = stack->stack_pointer[--stack->used];
  
  if (value.type == TYPE_STRING) {
    string_release(value.string);
  } else if (value.type == TYPE_FUNCTION) {
    function_retain(value.function);
  }

  return value;
}

void vm_free_stack(struct Stack *stack) {
  stack->stack_pointer = NULL;
  stack->used = stack->size = 0;
  free(stack);
}
