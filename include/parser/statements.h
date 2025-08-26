#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "expressions.h"
#include "statement_vector.h"

enum StatementType {
    PRINT_STATEMENT,
    BLOCK_STATEMENT,
    EXPRESSION_STATEMENT,
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

struct ExpressionStatement {
    struct Statement statement;
    struct Expression *expression;
};

#endif