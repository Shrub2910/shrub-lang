#include <stdlib.h>
#include <stddef.h>

#include "objects/function.h"

#include "error/error.h"
#include "objects/reference_counter.h"

#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/stack.h"
#include "vm/variables/stack_frame.h"
#include "vm/values.h"

// Initialise the function object and make it the right size
struct Function *function_init(void) {
  struct Function *function = malloc(sizeof(struct Function));
  function->instruction_buffer = vm_init_instruction_buffer();
  function->num_args = 0;
  function->num_locals = 0;
  function->constants = malloc(sizeof(struct Value) * CONST_POOL_SIZE);
  function->constant_count = 0;
  function->functions = malloc(sizeof(struct Function *) * FUNCTION_POOL_SIZE);
  function->function_count = 0;

  return function;
}

struct Closure *closure_init(struct Function *function) {
  struct Closure *closure = malloc(sizeof(struct Closure));
  closure->function = function;
  closure->references = 1;

  return closure;
}

// Used by the call instruction to push a new stack frame, update program counter, and load arguments
void closure_call(struct Closure *closure, struct VM *vm) {
  vm_push_frame(
    &vm->stack_frame,
    closure->function->num_locals,
    RETURN_ADDRESS(vm->program_counter) // Return address is instruction after call
  ); 

  vm->stack_frame->closure = closure;

  // Program-counter points to called function's instructions
  vm->program_counter = closure->function->instruction_buffer->buffer;

  struct Value value = {.type = TYPE_CLOSURE, .closure = closure};
  vm_set_local(vm->stack_frame, 0, value);
  object_retain(value);
  
  // Loads the arguments in reverse order to simplify bytecode
  for (size_t i = closure->function->num_args; i > 0; --i) {
    vm_set_local(vm->stack_frame, i, vm_pop_stack(vm->stack));
  }
}

// Pops the top level frame and updates the program counter
void closure_return(struct VM *vm) {
  vm_pop_frame(&vm->stack_frame, &vm->program_counter);
}

// Insures the function object will live 
void closure_retain(struct Closure *closure) {
  closure->references++;
}

// Insures the function object will be freed
void closure_release(struct Closure *closure) {
  if (--closure->references <= 0) {
    free(closure);
  }
}
