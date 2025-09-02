#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vm/variables/stack_frame.h"
#include "vm/values.h"
#include "objects/function.h"
#include "objects/reference_counter.h"
#include "vm/variables/upvalues.h"

#define NUMBER_OF_PRE_DEFINED_LOCALS 2

// Allocate and initialise new stack frame for function call or top level scope
// data[0] = return address 
// arguments come first, then locals 
struct StackFrame *vm_init_stack_frame
(
  const size_t num_locals,
  struct StackFrame *previous_stack_frame,
  const struct Value return_address
) 
{
  const size_t data_size = num_locals + NUMBER_OF_PRE_DEFINED_LOCALS;

  struct StackFrame *stack_frame = 
    malloc(sizeof(struct StackFrame) + sizeof(struct Value) * data_size);

  stack_frame->previous_stack_frame = previous_stack_frame;

  stack_frame->num_locals = num_locals + NUMBER_OF_PRE_DEFINED_LOCALS;

  memset(stack_frame->data, 0, data_size * sizeof(struct Value));
  stack_frame->data[0] = return_address;

  stack_frame->upvalue = NULL;

  return stack_frame;
}

void vm_add_upvalues(struct StackFrame *stack_frame, struct Closure *new_closure) {
  for (size_t i = 0; i < new_closure->function->upvalue_descriptor_count; i++) {
    struct UpvalueDesc descriptor = new_closure->function->upvalue_descriptors[i];
    if (descriptor.is_local) {
      struct Upvalue *upvalue = stack_frame->upvalue;
      while (upvalue) {
        if (upvalue->v == &stack_frame->data[descriptor.index + 1] ) {
          new_closure->upvalues[i] = upvalue;
          upvalue->reference_count++;
          break;
        }
        upvalue = upvalue->next;
      }

      if (upvalue) continue;

      struct Upvalue *new_upvalue = malloc(sizeof(struct Upvalue));
      new_closure->upvalues[i] = new_upvalue;
      new_upvalue->v = &stack_frame->data[descriptor.index + 1];
      new_upvalue->next = stack_frame->upvalue;
      new_upvalue->reference_count = 1;
      stack_frame->upvalue = new_upvalue;


    } else {
      struct Upvalue *parent_upvalue = stack_frame->closure->upvalues[descriptor.index];
      parent_upvalue->reference_count++;
      new_closure->upvalues[i] = parent_upvalue;
    }
  }
}

// Retrieve the value of a local variable from the stack frame 
struct Value vm_get_local(const struct StackFrame *stack_frame, const size_t offset) {
  return stack_frame->data[offset + 1];
}

// Set the value of a local variable on the stack frame
void vm_set_local(struct StackFrame *stack_frame, const size_t offset, const struct Value value) {
  struct Value *old_value = &stack_frame->data[offset + 1];

  if (old_value->type != TYPE_UNSET) {
    object_release(*old_value);
  }

  *old_value = value;
}

// Push a new frame and point to it,
// Supply the return address and a pointer to the previous stack to the new stack frame 
void vm_push_frame
(
  struct StackFrame **current_stack_frame,
  const size_t num_locals,
  const struct Value return_address
) 
{
  struct StackFrame *new_stack_frame = 
    vm_init_stack_frame(num_locals, *current_stack_frame, return_address);
  
  *current_stack_frame = new_stack_frame;
}

// Pop the current frame and point to the previous 
// Jump the supplied program counter to the correct frame 
void vm_pop_frame(
  struct StackFrame **current_stack_frame, 
  uint8_t **current_program_counter
)
{
  struct StackFrame *old_stack_frame = *current_stack_frame;

  *current_stack_frame = (*current_stack_frame)->previous_stack_frame;
  *current_program_counter = old_stack_frame->data[0].return_address;

  vm_free_frame(old_stack_frame);
}

// Clean up stack frame once it has been popped
void vm_free_frame(struct StackFrame *stack_frame) {
  close_upvalues(stack_frame->upvalue);

  // Ensure heap allocated objects are released if necessary
  for (size_t i = 1; i < (stack_frame->num_locals); ++i) {
    const struct Value value = stack_frame->data[i];
    object_release(value);
  }

  stack_frame->previous_stack_frame = NULL;
  stack_frame->num_locals = 0;

  free(stack_frame);
}

