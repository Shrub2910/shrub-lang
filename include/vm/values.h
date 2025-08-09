#ifndef VALUES_H
#define VALUES_H

enum Type {
  TYPE_NUMBER
};

struct Value {
  enum Type type;
  union {
    double number;
  };
};

// Constructors

#define NUMBER(value) ((struct Value){.type = TYPE_NUMBER, .number = value}) 

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
