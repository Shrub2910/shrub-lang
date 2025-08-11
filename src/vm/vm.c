#include <stdio.h>
#include <stdlib.h>
#include "vm/instruction_buffer.h"
#include "vm/opcodes.h"
#include "vm/vm.h"
#include "vm/values.h"
#include "vm/stack.h"
#include "error/error.h"
#include "error/error_types.h"
#include "objects/string.h"

struct VM *vm_init() {
   
  struct VM *vm = (struct VM *) malloc(sizeof(struct VM));
  vm->instruction_buffer = vm_init_instruction_buffer();
  vm->program_counter = vm->instruction_buffer->buffer;
  
  vm->constant_count = 0;
  vm->constants = (struct Value *) malloc(CONST_POOL_SIZE * sizeof(struct Value));

  vm->stack = vm_init_stack();

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
