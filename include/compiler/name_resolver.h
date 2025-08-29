#ifndef NAME_RESOLVER_H
#define NAME_RESOLVER_H

#include "parser/statement_vector.h"
#include "compiler/compiler.h"

void compiler_resolve_statements
(
    struct CompilerContext *compiler_context,
    struct StatementVector *statement_vector
);

#endif
