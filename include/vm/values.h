#ifndef VALUES_H
#define VALUES_H

#include <stdbool.h>
#include "objects/string.h"
#include "objects/function.h"


enum Type {
  TYPE_NUMBER,
  TYPE_BOOLEAN,
  TYPE_STRING,
  TYPE_NIL,
  TYPE_RETURN_ADDRESS,
  TYPE_FUNCTION
};

struct Value {
  enum Type type;
  union {
    double number;
    bool boolean;
    struct String *string;
    struct Function *function;
    uint8_t *return_address;
  };
};

// Constructors

#define NUMBER(value) ((struct Value){.type = TYPE_NUMBER, .number = value}) 
#define BOOLEAN(value) ((struct Value){.type = TYPE_BOOLEAN, .boolean = value})
#define STRING(value, length) ((struct Value){.type = TYPE_STRING, .string = string_init(value, length)})
#define NIL() ((struct Value){.type = TYPE_NIL})
#define FUNCTION(num_args, num_locals) ((struct Value) \
    {.type = TYPE_FUNCTION, .function = function_init(num_args, num_locals)}) 
#define RETURN_ADDRESS(value) ((struct Value){.type = TYPE_RETURN_ADDRESS, .return_address = value})

// Utils

#define VALUES_ARE_SAME_TYPE(type1, type2) (((type1).type) == ((type2).type))

// Macro for quickly inserting values into const pool
// Should only be used for testing
#define INSERT_CONST_VALUES(vm, ...)            \
  do {                                          \
    struct Value temp[] = {__VA_ARGS__};        \
    for (size_t i = 0; i < sizeof(temp)/sizeof(temp[0]); ++i) { \
     vm_add_const(vm, temp[i]);                \
    }                                           \
  } while (0)

#endif
