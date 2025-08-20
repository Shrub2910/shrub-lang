#include "parser/statement_vector.h"

#include <stdlib.h>

#include "error/error.h"
#include "error/error_types.h"

struct StatementVector *parser_statement_vector_init() {
    struct StatementVector *statement_vector = malloc(sizeof(struct StatementVector));

    if (!statement_vector) {
        error_throw(MALLOC_ERROR, "Failed to allocate statement vector");
    }

    statement_vector->used = 0;
    statement_vector->size = STATEMENT_VECTOR_INITIAL_SIZE;

    statement_vector->statements = malloc(STATEMENT_VECTOR_INITIAL_SIZE * sizeof(struct Statement *));

    if (!statement_vector->statements) {
        free(statement_vector);
        error_throw(MALLOC_ERROR, "Failed to allocate statement vector");
    }

    return statement_vector;
}

void parser_statement_vector_insert(struct StatementVector *statement_vector, struct Statement *statement) {
    if (statement_vector->used == statement_vector->size) {
        statement_vector->size *= 2;
        struct Statement **statements = realloc(statement_vector->statements, sizeof(struct Statement*) * statement_vector->size);

        if (!statements) {
            error_throw(MALLOC_ERROR, "Failed to reallocate statement vector");
        }

        statement_vector->statements = statements;
    }

    statement_vector->statements[statement_vector->used++] = statement;
}

void parser_statement_vector_free(struct StatementVector *statement_vector) {
    for (size_t i = 0; i < statement_vector->used; i++) {
        free(statement_vector->statements[i]);
    }
    free(statement_vector->statements);
    statement_vector->statements = NULL;

    statement_vector->used = statement_vector->size = 0;

    free(statement_vector);
}
