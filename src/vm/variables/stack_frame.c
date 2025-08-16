#include <stddef.h>
#include <stdlib.h>

#include "vm/variables/stack_frame.h"
#include "vm/values.h"
#include "vm/objects/string.h";

struct StackFrame *vm_init_stack_frame
(size_t num_args, size_t num_locals, struct StackFrame *previous_stack_frame) {
  struct StackFrame *stack_frame = 
    malloc(sizeof(struct StackFrame) + sizeof(struct Value) * (num_args + num_locals + 1));

  stack_frame->previous_stack_frame = previous_stack_frame;

  stack_frame->num_args = num_args;
  stack_frame->num_locals = num_locals;
  
  // TODO: replace NULL with a null value once implemented
  stack_frame->data[0] = NULL;
}

struct Value vm_get_local(struct StackFrame *stack_frame, size_t offset) {
  return stack_frame->data[stack_frame->num_args + offset];
}

void vm_set_local(struct StackFrame *stack_frame, size_t offset, struct Value value) {
  // Ensure heap allocated objects continue to live
  if (value.type == TYPE_STRING) {
    string_retain(value.string);
  }

  stack_frame->data[stack_frame->num_args + offset] = value;
}

struct Value vm_get_arg(struct StackFrame *stack_frame, size_t offset) {
  return stack_frame->data[offset];
}

void vm_set_arg(struct StackFrame *stack_frame, size_t offset, struct Value value) {
  // Ensure heap allocated objects continue to live
  if (value.type == TYPE_STRING) {
    string_retain(value.string);
  }

  stack_frame->data[offset] = value;
}

void vm_push_frame
(
  struct StackFrame **current_stack_frame,
  size_t num_args,
  size_t num_locals,
  uint8_t *return_address
) 
{
  struct StackFrame *new_stack_frame = 
    vm_init_stack_frame(num_args, num_locals, *current_stack_frame); 
  
  new_stack_frame->data[0] = return_address;
  *current_stack_frame = new_stack_frame;
}

void vm_pop_frame(
  struct StackFrame **current_stack_frame, 
  uint8_t **current_program_counter
)
{
  struct StackFrame *old_stack_frame = *current_stack_frame;

  *current_stack_frame = *current_stack_frame->previous_stack_frame;
  *current_program_counter = old_stack_frame->data[0];

  vm_free_frame(old_stack_frame);
}

void vm_free_frame(struct StackFrame *stack_frame) {
  for (size_t i = 1; i < (stack_frame->num_locals + stack_frame->num_args); ++i) {
    struct Value value = stack_frame->data[i];
    if (value.type == TYPE_STRING) {
      string_retain(value.string);
    }
  }

  stack_frame->previous_stack_frame = NULL;
  stack_frame->num_args = 0;
  stack_frame->num_locals = 0;
  stack_frame->data = NULL;

  free(stack_frame);
}

