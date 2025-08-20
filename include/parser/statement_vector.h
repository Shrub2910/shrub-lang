#ifndef STATEMENT_VECTOR_H
#define STATEMENT_VECTOR_H
#include <stddef.h>

struct StatementVector {
    struct Statement **statements;
    size_t used;
    size_t size;
};

struct StatementVector *parser_statement_vector_init();
void parser_statement_vector_insert(struct StatementVector *statement_vector, struct Statement *statement);
void parser_statement_vector_free(struct StatementVector *statement_vector);

#define STATEMENT_VECTOR_INITIAL_SIZE 8

#endif