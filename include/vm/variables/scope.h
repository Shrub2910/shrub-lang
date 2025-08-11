#include <stddef.h>
#include "vm/values.h"

#ifndef SCOPE_H
#define SCOPE_H

struct Scope {
  struct Value *values;
  size_t size;
};

struct Scope vm_init_scope(size_t size);
void vm_insert_scope(struct Scope scope, struct Value value, size_t index);
struct Value vm_index_scope(struct Scope scope, size_t index);
void vm_free_scope(struct Scope scope);

#endif
