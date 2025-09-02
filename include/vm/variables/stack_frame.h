#ifndef STACK_FRAME_H
#define STACK_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "vm/values.h"

struct StackFrame {
    struct StackFrame *previous_stack_frame;
    size_t num_locals;

    struct Closure *closure;

    struct Upvalue *upvalue;

    struct Value data[];
};

struct StackFrame *vm_init_stack_frame(
  size_t num_locals,
  struct StackFrame *previous_stack_frame,
  struct Value return_address
);

void vm_add_upvalues(struct StackFrame *stack_frame, struct Closure *new_closure);

struct Value vm_get_local(const struct StackFrame *stack_frame, size_t offset);

void vm_set_local(struct StackFrame *stack_frame, size_t offset, struct Value value);

struct Value vm_get_arg(const struct StackFrame *stack_frame, size_t offset);

void vm_set_arg(struct StackFrame *stack_frame, size_t offset, struct Value value);

void vm_push_frame
(
 struct StackFrame **current_stack_frame,
 size_t num_locals,
 struct Value return_address

);

void vm_pop_frame(struct StackFrame **current_stack_frame, uint8_t **current_program_counter);

void vm_free_frame(struct StackFrame *stack_frame);

#endif
