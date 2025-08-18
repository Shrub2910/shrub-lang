#include <stdlib.h>
#include <stddef.h>

#include "objects/function.h"
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/stack.h"
#include "vm/variables/stack_frame.h"

// Initialise the function object and make it the right size
struct Function *function_init(const size_t num_args, const size_t num_locals) {
  struct Function *function = malloc(sizeof(struct Function));
  function->instruction_buffer = vm_init_instruction_buffer();
  function->num_args = num_args;
  function->num_locals = num_locals;
  function->references = 1;

  return function;
}

// Used by the call instruction to push a new stack frame, update program counter, and load arguments
void function_call(const struct Function *function, struct VM *vm) {
  vm_push_frame(
    &vm->stack_frame,
    function->num_args,
    function->num_locals,
    RETURN_ADDRESS(vm->program_counter) // Return address is instruction after call
  ); 

  // Program-counter points to called function's instructions
  vm->program_counter = function->instruction_buffer->buffer;
  
  // Loads the arguments in reverse order to simplify bytecode
  for (size_t i = function->num_args; i > 0; --i) {
    vm_set_arg(vm->stack_frame, i, vm_pop_stack(vm->stack));
  }
}

// Pops the top level frame and updates the program counter
void function_return(struct VM *vm) {
  vm_pop_frame(&vm->stack_frame, &vm->program_counter);
}

// Insures the function object will live 
void function_retain(struct Function *function) {
  function->references++;
}

// Insures the function object will be freed
void function_release(struct Function *function) {
  if (--function->references <= 0) {
    free(function);
  }
}
