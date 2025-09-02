#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stddef.h>
#include "compiler/scope.h"
#include "compiler/compiler.h"

enum ResolveResultType {
    RESOLVE_NOT_FOUND,
    RESOLVE_LOCAL,
    RESOLVE_GLOBAL,
    RESOLVE_UPVALUE
};

struct ResolveResult {
    enum ResolveResultType kind;
    size_t index;
};

struct Environment {
    struct Scope *scopes;
    size_t used;
    size_t size;

    size_t variable_count;
};

struct Environment compiler_init_environment(void);
void compiler_push_scope(struct Environment *environment);
struct ResolveResult compiler_search_environment(struct CompilerContext *compiler_context, const char *name);
void compiler_free_environment(const struct Environment *environment);
struct Scope *compiler_get_top_scope(const struct Environment *environment);
void compiler_insert_environment(const struct Environment *environment, char *name);
void compiler_pop_scope(struct Environment *environment);

#define ENVIRONMENT_INITIAL_SIZE 16

#endif
