#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <stdbool.h>

#include "expressions.h"
#include "statement_vector.h"

enum StatementType {
    PRINT_STATEMENT,
    BLOCK_STATEMENT,
    EXPRESSION_STATEMENT,
    LET_STATEMENT,
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
};

struct ExpressionStatement {
    struct Statement statement;
    struct Expression *expression;
};

#endif