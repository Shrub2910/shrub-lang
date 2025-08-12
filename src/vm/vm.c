#include <stdio.h>
#include <stdlib.h>
#include "vm/instruction_buffer.h"
#include "vm/opcodes.h"
#include "vm/vm.h"
#include "vm/values.h"
#include "vm/stack.h"
#include "vm/variables/environment.h"
#include "error/error.h"
#include "error/error_types.h"
#include "objects/string.h"
#include "utils/operand_conversion.h"

struct VM *vm_init() {
   
  struct VM *vm = (struct VM *) malloc(sizeof(struct VM));

  if (!vm) {
    error_throw(MALLOC_ERROR, "Failed to allocate vm");
  }

  vm->instruction_buffer = vm_init_instruction_buffer();

  if (!vm->instruction_buffer) {
    vm_free(vm);
    error_throw(MALLOC_ERROR, "Failed to allocate instruction buffer");
  }

  vm->program_counter = vm->instruction_buffer->buffer;
  
  vm->constant_count = 0;
  vm->constants = (struct Value *) malloc(CONST_POOL_SIZE * sizeof(struct Value));

  if (!vm->constants) {
    vm_free(vm);
    error_throw(MALLOC_ERROR, "Failed to allocate constants pool");
  }

  vm->stack = vm_init_stack();

  if (!vm->stack) {
    vm_free(vm);
    error_throw(MALLOC_ERROR, "Failed to allocate stack");
  }

  vm->environment = vm_init_environment();

  if (!vm->environment) {
    vm_free(vm);
    error_throw(MALLOC_ERROR, "Failed to allocate environment");
  }

  return vm;
}

void vm_exec(struct VM *vm) {
  
  // Program only finishes execution when it should
  for (;;) {
    uint8_t current_instruction = *(vm->program_counter++);
    switch (current_instruction) {
      // Must be used to terminate the program
      case HALT:
        return;
      case PRINT:
        struct Value value_to_print = vm_pop_stack(vm->stack);
        switch (value_to_print.type) {
          case TYPE_NUMBER:
            printf("Value: %f\n", value_to_print.number);
            break;
          case TYPE_STRING:
            printf("Value: %s\n", value_to_print.string->buffer);
            break;
          default:
            error_throw(TYPE_ERROR, "Object not printable");
        }
        break;
      case LOAD_CONST: {
        uint8_t const_index = *(vm->program_counter++);
        struct Value value = vm->constants[const_index];

        vm_push_stack(vm->stack, value);

        break;
      }
      case ADD: {
        struct Value operand_2 = vm_pop_stack(vm->stack);
        struct Value operand_1 = vm_pop_stack(vm->stack);

        if (!VALUES_ARE_SAME_TYPE(operand_1, operand_2)){
          error_throw(TYPE_ERROR, "Attempt to add values of different types");
        }

        if (operand_1.type == TYPE_NUMBER) {
          double result = operand_1.number + operand_2.number;
          vm_push_stack(vm->stack, NUMBER(result));
        }
        break;
      }
      case SUB: {
        struct Value operand_2 = vm_pop_stack(vm->stack);
        struct Value operand_1 = vm_pop_stack(vm->stack);

        if (!VALUES_ARE_SAME_TYPE(operand_1, operand_2)) {
          error_throw(TYPE_ERROR, "Attempt to subtract values of different types");
        }
        
        if (operand_1.type == TYPE_NUMBER) {
          double result = operand_1.number - operand_2.number;
          vm_push_stack(vm->stack, NUMBER(result));
        }
        break;
      }
      case MUL: {
        struct Value operand_2 = vm_pop_stack(vm->stack);
        struct Value operand_1 = vm_pop_stack(vm->stack);

        if (!VALUES_ARE_SAME_TYPE(operand_1, operand_2)) {
          error_throw(TYPE_ERROR, "Attempt to multiply values of different types");
        }

        if (operand_1.type == TYPE_NUMBER) {
          double result = operand_1.number * operand_2.number;
          vm_push_stack(vm->stack, NUMBER(result));
        }
        break;
      }
      case DIV: {
        struct Value operand_2 = vm_pop_stack(vm->stack);
        struct Value operand_1 = vm_pop_stack(vm->stack);

        if (!VALUES_ARE_SAME_TYPE(operand_1, operand_2)) {
          error_throw(TYPE_ERROR, "Attempt to divide values of different types");
        }

        if (operand_1.type == TYPE_NUMBER) {
          double result = operand_1.number / operand_2.number;
          vm_push_stack(vm->stack, NUMBER(result));
        }
        break;
      }
      case STORE_VAR: {
        struct Value value = vm_pop_stack(vm->stack);
        size_t offset = *(vm->program_counter++);
        
        vm_set_variable_environment(vm->environment, value, offset); 
        break;
      }
      case LOAD_VAR: {
        size_t depth = *(vm->program_counter++);
        size_t offset = *(vm->program_counter++);
        vm_push_stack(vm->stack, vm_get_variable_environment(vm->environment, depth, offset));
        break;
      }
      case PUSH_SCOPE: {
        size_t size = *(vm->program_counter++);
        vm_push_environment(vm->environment, size);
        break;
      }
      case POP_SCOPE: {
        vm_pop_environment(vm->environment);
        break;
      }
      case JUMP: {
        uint8_t lo = *(vm->program_counter++);
        uint8_t hi = *(vm->program_counter++);

        int16_t offset = TO_SIGNED_WORD(lo, hi);

        vm->program_counter += offset;
        break;
      }
    }
 
  } 
}

void vm_add_const(struct VM *vm, const struct Value value) {
  if (vm->constant_count == CONST_POOL_SIZE) {
    printf("Error Too Many Constants!"); // TEMPORARY
  }
  
  vm->constants[vm->constant_count++] = value; 
}

void vm_free_consts(struct VM *vm) {
  for (size_t i = 0; i < vm->constant_count; ++i) {
    struct Value value = vm->constants[i];

    if (value.type == TYPE_STRING) {
      string_free(value.string);
    }
  }

  free(vm->constants);
  vm->constants = NULL;
}

void vm_free(struct VM *vm) {
  vm_free_instruction_buffer(vm->instruction_buffer);
  vm->instruction_buffer = NULL;

  vm->program_counter = NULL;

  vm_free_consts(vm);

  vm_free_stack(vm->stack);
  vm->stack = NULL;

  free(vm);
}
