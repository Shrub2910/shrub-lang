#include <stdlib.h>
#include <stdio.h>
#include "vm/variables/scope.h"
#include "error/error.h"
#include "error/error_types.h"
#include "objects/string.h"

struct Scope vm_init_scope(size_t size) {
  struct Scope scope;
  scope.values = malloc(size * sizeof(struct Value));

  if (!scope.values) {
    error_throw(MALLOC_ERROR, "Failed to allocate values array for scope");
  }
  
  scope.size = size;

  return scope;
}

void vm_insert_scope(struct Scope scope, struct Value value, size_t index) {
  if (index >= scope.size) {
    error_throw(INDEX_ERROR, "Index out of range in variable scope (insert)");
  }

  scope.values[index] = value;
}

struct Value vm_index_scope(struct Scope scope, size_t index) {
  if (index >= scope.size) {
    error_throw(INDEX_ERROR, "Index out of range in variable scope (index)");
  }

  return scope.values[index];
}

void vm_free_scope(struct Scope scope) {
  for (size_t i = 0; i < scope.size; ++i) {
    struct Value value = scope.values[i];
    if (value.type == TYPE_STRING) {
      string_release(value.string);
    } 
  } 

  free(scope.values);
}
