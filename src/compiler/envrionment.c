#include <stdlib.h>
#include "compiler/compiler.h"
#include "compiler/environment.h"
#include "compiler/scope.h"
#include "error/error.h"
#include "error/error_types.h"

struct Environment compiler_init_environment(void) {
    struct Environment environment = {.used = 0, .size = ENVIRONMENT_INITIAL_SIZE};

    struct Scope *scopes = malloc(ENVIRONMENT_INITIAL_SIZE * sizeof(struct Scope));

    if (!scopes) {
        error_throw(MALLOC_ERROR, "Failed to allocate memory for environment");
    }

    environment.variable_count = 0;
    environment.scopes = scopes;

    compiler_push_scope(&environment);

    return environment;
}

void compiler_push_scope(struct Environment *environment) {
    if (environment->used == environment->size) {
        environment->size *= 2;
        struct Scope *new_scopes = realloc(environment->scopes, environment->size * sizeof(struct Scope));

        if (!new_scopes) {
            error_throw(MALLOC_ERROR, "Failed to reallocate memory for environment");
        }

        environment->scopes = new_scopes;
    }

    environment->scopes[environment->used++] = compiler_init_scope();
}

struct ResolveResult compiler_search_environment(struct CompilerContext *compiler_context, const char *name) {
    for (size_t i = compiler_context->environment->used; i-- > 0;) {
        const struct Scope *scope = &compiler_context->environment->scopes[i];
        struct ScopeVariable *scope_variable = compiler_search_scope(scope, name);
        if (scope_variable) {
            return (struct ResolveResult){.index = scope_variable->offset, .kind = RESOLVE_LOCAL};
        }
    }

    if (compiler_context->previous == NULL) {
        return (struct ResolveResult){.index = 0, .kind = RESOLVE_NOT_FOUND};
    }

    struct ResolveResult resolve_result = compiler_search_environment(compiler_context->previous, name);
    struct Function *function = compiler_context->function;

    if (resolve_result.kind == RESOLVE_LOCAL) {
        size_t upvalue_index = function->upvalue_descriptor_count++;
        function->upvalue_descriptors[upvalue_index] =
            (struct UpvalueDesc){.is_local = true, .index = resolve_result.index};
        return (struct ResolveResult){.kind = RESOLVE_UPVALUE, .index = upvalue_index};
    } else if (resolve_result.kind == RESOLVE_UPVALUE) {
        size_t upvalue_index = function->upvalue_descriptor_count++;
        struct Function *previous_function = compiler_context->previous->function;
        function->upvalue_descriptors[upvalue_index] =
            (struct UpvalueDesc){.is_local = false, .index = previous_function->upvalue_descriptor_count - 1};
        return (struct ResolveResult){.kind = RESOLVE_UPVALUE, .index = upvalue_index};
    }

    return (struct ResolveResult) {.kind = RESOLVE_NOT_FOUND};
}

void compiler_insert_environment(const struct Environment *environment, char *name) {
    struct Scope *current_scope = compiler_get_top_scope(environment);
    compiler_insert_scope(current_scope, name, environment->variable_count);
}

struct Scope *compiler_get_top_scope(const struct Environment *environment) {
    return &environment->scopes[environment->used - 1];
}

void compiler_pop_scope(struct Environment *environment) {
    environment->used--;
}

void compiler_free_environment(const struct Environment *environment) {
    for (size_t i = 0; i < environment->used; i++) {
        compiler_free_scope(&environment->scopes[i]);
    }

    free(environment->scopes);
}
