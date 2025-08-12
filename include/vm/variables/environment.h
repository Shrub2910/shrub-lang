#include "vm/variables/scope.h"

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

struct Environment {
  struct Scope *scopes;
  size_t used;
  size_t size;
};

struct Environment *vm_init_environment();
void vm_push_environment(struct Environment *environment, size_t size);
void vm_pop_environment(struct Environment *environment);
struct Value vm_get_variable_environment(struct Environment *environment, size_t depth, size_t offset);
void vm_set_variable_environment
(struct Environment *environment, struct Value value, size_t depth, size_t offset); 
void vm_free_environment(struct Environment *environment);

// Setup

#define INITIAL_SCOPE_DEPTH 4

#endif
