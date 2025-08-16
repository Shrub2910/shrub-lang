#include <stdlib.h>
#include <stddef.h>

#include "objects/function.h"
#include "vm/vm.h"
#include "vm/instruction_buffer.h"
#include "vm/stack.h"
#include "vm/variables/stack_frame.h"

struct Function *function_init(size_t num_args, size_t num_locals) {
  struct Function *function = malloc(sizeof(struct Function));
  function->instruction_buffer = vm_init_instruction_buffer();
  function->num_args = num_args;
  function->num_locals = num_locals;
  function->references = 1;

  return function;
}

void function_call(struct Function *function, struct VM *vm) {
  vm_push_frame(
    &vm->stack_frame,
    function->num_args,
    function->num_locals,
    RETURN_ADDRESS(vm->program_counter)
  ); 
  vm->program_counter = function->instruction_buffer->buffer;

  for (size_t i = function->num_args; i > 0; --i) {
    vm_set_arg(vm->stack_frame, i, vm_pop_stack(vm->stack));
  }
}

void function_return(struct VM *vm) {
  vm_pop_frame(&vm->stack_frame, &vm->program_counter);
}

void function_retain(struct Function *function) {
  function->references++;
}

void function_release(struct Function *function) {
  if (--function->references <= 0) {
    free(function);
  }
}
