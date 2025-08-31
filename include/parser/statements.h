#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <stdbool.h>
#include <stddef.h>

#include "expressions.h"
#include "statement_vector.h"

enum StatementType {
    PRINT_STATEMENT,
    BLOCK_STATEMENT,
    EXPRESSION_STATEMENT,
    LET_STATEMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FUNCTION_STATEMENT,
    RETURN_STATEMENT,
};

struct Statement {
    enum StatementType type;
};

struct PrintStatement {
    struct Statement statement;
    struct Expression *expression;
};

struct BlockStatement {
    struct Statement statement;
    struct StatementVector *statement_vector;
};

struct LetStatement {
    struct Statement statement;
    char *identifier_name;
    struct Expression *expression;

    bool is_nil;
    size_t offset;
};

struct IfStatement {
    struct Statement statement;
    struct Expression *condition;
    struct BlockStatement *then_block;
    struct Statement *else_block;
};

struct WhileStatement {
    struct Statement statement;
    struct Expression *condition;
    struct BlockStatement *body;
};

struct FunctionStatement {
    struct Statement statement;
    char *name;
    struct BlockStatement *body;
    size_t num_parameters;
    char **parameters;

    size_t offset;
    size_t num_locals;
};

struct ExpressionStatement {
    struct Statement statement;
    struct Expression *expression;
};

struct ReturnStatement {
    struct Statement statement;
    struct Expression *expression;
};

#endif