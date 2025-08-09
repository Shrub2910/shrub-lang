#include <stdlib.h>
#include <stdio.h>
#include "vm/stack.h"
#include "vm/values.h"

struct Stack *vm_init_stack() {
  struct Stack *stack = (struct Stack *) malloc(sizeof(struct Stack));

  stack->stack_pointer = (struct Value *) malloc(STACK_SIZE * sizeof(struct Value));
  stack->used = 0;
  stack->size = STACK_SIZE;

  return stack;
}

void vm_push_stack(struct Stack *stack, struct Value value) {
  if (stack->used == stack->size) {
    printf("Stack overflow"); // TEMPORARY
    return;
  }

  stack->stack_pointer[stack->used++] = value;
}

struct Value vm_pop_stack(struct Stack *stack) {
  return stack->stack_pointer[--stack->used];
}

void vm_free_stack(struct Stack *stack) {
  stack->stack_pointer = NULL;
  stack->used = stack->size = 0;
  free(stack);
}
