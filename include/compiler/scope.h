#ifndef SCOPE_H
#define SCOPE_H

#include <stddef.h>

struct ScopeVariable {
    char *name;
    size_t offset;
};

struct Scope {
    size_t used;
    size_t size;
    struct ScopeVariable *variables;
};

struct Scope compiler_init_scope(void);
void compiler_insert_scope(struct Scope *scope, char *name, size_t offset);
struct ScopeVariable *compiler_search_scope(const struct Scope *scope, const char *name);
void compiler_free_scope(const struct Scope *scope);

#define SCOPE_INITIAL_SIZE 16

#endif