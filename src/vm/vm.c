#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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

#define COMPARISON() \

// Helper function to be called for all comparison operations 
bool compare(struct Value operand_1, struct Value operand_2, uint8_t opcode) {
  if (operand_1.type != operand_2.type) {
    // NOT_EQUAL is the only instruction that can return true
    return opcode == NOT_EQUAL;
  }

  bool result;

  if (operand_1.type == TYPE_NUMBER) {
    switch (opcode) {
      case EQUAL: result = operand_1.number == operand_2.number; break;
      case NOT_EQUAL: result = operand_1.number != operand_2.number; break;
      case GREATER: result = operand_1.number > operand_2.number; break;
      case LESS: result = operand_1.number < operand_2.number; break;
      case GREATER_EQUAL: result = operand_1.number >= operand_2.number; break;
      case LESS_EQUAL: result = operand_1.number <= operand_2.number; break;
    }
  }

  if (operand_1.type == TYPE_BOOLEAN) {
    switch (opcode) {
      case EQUAL: result = operand_1.boolean == operand_2.boolean; break;
      case NOT_EQUAL: result = operand_1.boolean != operand_2.boolean; break;
      case GREATER: result = operand_1.boolean > operand_2.boolean; break;
      case LESS: result = operand_1.boolean < operand_2.boolean; break;
      case GREATER_EQUAL: result = operand_1.boolean >= operand_2.boolean; break;
      case LESS_EQUAL: result = operand_1.boolean <= operand_2.boolean; break;
    }
  }
  
  // Uses strcmp to evalutate equality, inequality and lexicographical ordering 
  if (operand_1.type == TYPE_STRING) {
    switch (opcode) {
      case EQUAL: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) == 0;
        break;
      case NOT_EQUAL: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) != 0;
				break;
      case GREATER: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) > 0;
				break;
      case LESS: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) < 0;
				break;
      case GREATER_EQUAL: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) >= 0;
				break;
      case LESS_EQUAL: result = strcmp(operand_1.string->buffer, operand_2.string->buffer) <= 0;
				break;
    }
  }

  return result;
}

// Returns a preconfigured vm struct to be used in the main function 
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
  vm->program_counter = vm->instruction_buffer->buffer;
  // Program only finishes execution when it should
  for (;;) {
    uint8_t current_instruction = *(vm->program_counter++);
    switch (current_instruction) {
      // Must be used to terminate the program
      case HALT:
        return;
      // Useful for debugging the vm 
      case PRINT:
        struct Value value_to_print = vm_pop_stack(vm->stack);
        switch (value_to_print.type) {
          case TYPE_NUMBER:
            printf("%f\n", value_to_print.number);
            break;
          case TYPE_STRING:
            printf("%s\n", value_to_print.string->buffer);
            break;
          case TYPE_BOOLEAN:
            printf("%s\n", value_to_print.boolean ? "true" : "false");
            break;
          default:
            error_throw(TYPE_ERROR, "Object not printable");
        }
        break;
      // Push constants onto the stack 
      case LOAD_CONST: {
        uint8_t const_index = *(vm->program_counter++);
        struct Value value = vm->constants[const_index];

        vm_push_stack(vm->stack, value);

        break;
      }
      // These 4 instructions will pop 2 values of the top of the stack and push the result
      // of the calculation
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
      // Will store a variable at an offset to the current scope address
      case STORE_VAR: {
        struct Value value = vm_pop_stack(vm->stack);
        size_t depth = *(vm->program_counter++);
        size_t offset = *(vm->program_counter++);
        
        vm_set_variable_environment(vm->environment, value, depth, offset); 
        break;
      }
      // Will load a variable at an offset to the specified scope address 
      case LOAD_VAR: {
        size_t depth = *(vm->program_counter++);
        size_t offset = *(vm->program_counter++);
        vm_push_stack(vm->stack, vm_get_variable_environment(vm->environment, depth, offset));
        break;
      }
      // Entering scope 
      case PUSH_SCOPE: {
        size_t size = *(vm->program_counter++);
        vm_push_environment(vm->environment, size);
        break;
      }
      // Leaving scope 
      case POP_SCOPE: {
        vm_pop_environment(vm->environment);
        break;
      }
      // Useful for infinite loops or goto statements
      case JUMP: {
        uint8_t lo = *(vm->program_counter++);
        uint8_t hi = *(vm->program_counter++);

        int16_t offset = TO_SIGNED_WORD(lo, hi);

        vm->program_counter += offset;
        break;
      }
      // Performs a jump depending on the boolean at the top of the stack
      case JUMP_IF_TRUE: {
        uint8_t lo = *(vm->program_counter++);
        uint8_t hi = *(vm->program_counter++);
        
        int16_t offset = TO_SIGNED_WORD(lo, hi);

        struct Value value = vm_pop_stack(vm->stack);

        if (value.type != TYPE_BOOLEAN) {
          error_throw(TYPE_ERROR, "Jumps must use boolean types");
        }

        if (value.boolean) {
          vm->program_counter += offset;
        }
        break;
      }
      case JUMP_IF_FALSE: {
        uint8_t lo = *(vm->program_counter++);
        uint8_t hi = *(vm->program_counter++);

        int16_t offset = TO_SIGNED_WORD(lo, hi);

        struct Value value = vm_pop_stack(vm->stack);

        if (value.type != TYPE_BOOLEAN) {
          error_throw(TYPE_ERROR, "Jumps must use boolean types");
        }

        if (!value.boolean) {
          vm->program_counter += offset;
        }
        break;
      }
      // Compares values and pushes a boolean to the top of the vm_pop_stack
      // Useful for branching
      case EQUAL:
      case NOT_EQUAL:
      case GREATER:
      case LESS:
      case GREATER_EQUAL:
      case LESS_EQUAL: {
        struct Value operand_2 = vm_pop_stack(vm->stack);
        struct Value operand_1 = vm_pop_stack(vm->stack);
        
        vm_push_stack(vm->stack, BOOLEAN(compare(operand_1, operand_2, current_instruction)));
        break;
      } 
    }
  } 
}

// Helper function for adding constants to the constant pool 
void vm_add_const(struct VM *vm, const struct Value value) {
  if (vm->constant_count == CONST_POOL_SIZE) {
    printf("Error Too Many Constants!"); // TEMPORARY
  }
  
  vm->constants[vm->constant_count++] = value; 
}

// Handles clean up of constants 
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

// Cleans up the virtual machine when no longer needed
void vm_free(struct VM *vm) {
  vm_free_instruction_buffer(vm->instruction_buffer);
  vm->instruction_buffer = NULL;

  vm->program_counter = NULL;

  vm_free_consts(vm);

  vm_free_stack(vm->stack);
  vm->stack = NULL;

  free(vm);
}
