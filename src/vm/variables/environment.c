#include <stdlib.h>
#include <stdio.h>
#include "vm/variables/scope.h"
#include "vm/variables/environment.h"
#include "error/error.h"
#include "error/error_types.h"

struct Environment *vm_init_environment() {
  struct Environment *environment = malloc(sizeof(struct Environment));
  
  if (!environment) {
    error_throw(MALLOC_ERROR, "Failed to allocate environment");
  }

  environment->scopes = malloc(INITIAL_SCOPE_DEPTH * sizeof(struct Scope));

  if (!environment->scopes) {
    error_throw(MALLOC_ERROR, "Failed to allocate scope array");
  }

  environment->used = 0;
  environment->size = INITIAL_SCOPE_DEPTH;

  return environment;
}

void vm_push_environment(struct Environment *environment, size_t size) {
  if (environment->used == environment->size) {
    environment->size *= 2;
    environment->scopes = 
      realloc(environment->scopes, environment->size * sizeof(struct Scope));
  }

  struct Scope scope = vm_init_scope(size);
  environment->scopes[environment->used++] = scope;
}

void vm_pop_environment(struct Environment *environment) {
  if (environment->used == 0) {
    error_throw(STACK_ERROR, "Stack underflow in environment");
  }

  struct Scope scope = environment->scopes[--environment->used];
  
  vm_free_scope(scope);
}

struct Value vm_get_variable_environment(struct Environment *environment, size_t depth, size_t offset) {
  if (depth >= environment->used) {
    error_throw(STACK_ERROR, "Tried to access variable from too many scopes back");
  }

  return vm_index_scope(environment->scopes[environment->used - depth - 1], offset);
}

void vm_set_variable_environment(struct Environment *environment, struct Value value, size_t offset) {
  vm_insert_scope(environment->scopes[environment->used - 1], value, offset);
}

void vm_free_environment(struct Environment *environment) {
  for (size_t i = 0; i < environment->used; ++i) {
    vm_free_scope(environment->scopes[i]);
  }

  free(environment->scopes);
  environment->scopes = NULL;

  environment->used = environment->size = 0;

  free(environment);
}
