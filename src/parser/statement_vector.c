#include "parser/statement_vector.h"
#include "parser/statements.h"
#include "parser/expressions.h"

#include <stdlib.h>

#include "error/error.h"
#include "error/error_types.h"

static void parser_statement_free(struct Statement *statement);
static void parser_expression_free(struct Expression *expression);

struct StatementVector *parser_statement_vector_init(void) {
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
        parser_statement_free(statement_vector->statements[i]);
    }
    free(statement_vector->statements);
    statement_vector->statements = NULL;

    statement_vector->used = statement_vector->size = 0;

    free(statement_vector);
}

static void parser_statement_free(struct Statement *statement) {
    switch (statement->type) {
        case PRINT_STATEMENT: {
            struct PrintStatement *print_statement = (struct PrintStatement *)statement;
            parser_expression_free(print_statement->expression);
            free(print_statement);
            break;
        }
        case BLOCK_STATEMENT: {
            struct BlockStatement *block_statement = (struct BlockStatement *)statement;
            parser_statement_vector_free(block_statement->statement_vector);
            free(block_statement);
            break;
        }
        case EXPRESSION_STATEMENT: {
            struct ExpressionStatement *expression_statement = (struct ExpressionStatement *)statement;
            parser_expression_free(expression_statement->expression);
            free(expression_statement);
            break;
        }
    }
}

static void parser_expression_free(struct Expression *expression) {
    switch (expression->type) {
        case BINARY_EXPRESSION: {
            struct BinaryExpression *binary_expression = (struct BinaryExpression *)expression;
            parser_expression_free(binary_expression->left);
            parser_expression_free(binary_expression->right);
            free(binary_expression);
            break;
        }
        case LITERAL_EXPRESSION: {
            struct LiteralExpression *literal_expression = (struct LiteralExpression *)expression;
            free(literal_expression);
            break;
        }
    }
}
