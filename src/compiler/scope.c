#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "compiler/scope.h"

#include "error/error.h"

struct Scope compiler_init_scope(void) {
    struct Scope scope = {.used = 0, .size = SCOPE_INITIAL_SIZE};

    struct ScopeVariable *variables = malloc(sizeof(struct ScopeVariable) * SCOPE_INITIAL_SIZE);
    scope.variables = variables;

    return scope;
}

void compiler_insert_scope(struct Scope *scope, char *name, const size_t offset) {
    if (scope->used == scope->size) {
        scope->size *= 2;
        struct ScopeVariable *new_variables = realloc(scope->variables, sizeof(struct ScopeVariable) * scope->size);

        if (!new_variables) {
            error_throw(MALLOC_ERROR, "Failed to allocate new variables for scope");
        }

        scope->variables = new_variables;
    }
    scope->variables[scope->used] = (struct ScopeVariable){.name = name, .offset = offset};

    scope->used++;
}

struct ScopeVariable *compiler_search_scope(const struct Scope *scope, const char *name) {
    for (size_t i = 0; i < scope->used; i++) {
        if (strcmp(scope->variables[i].name, name) == 0) {
            return &scope->variables[i];
        }
    }

    return NULL;
}

void compiler_free_scope(const struct Scope *scope) {
    free(scope->variables);
}