#ifndef STATEMENTS_H
#define STATEMENTS_H

#include "expressions.h"

enum StatementType {
    PRINT_STATEMENT,
    EXPRESSION_STATEMENT,
};

struct Statement {
    enum StatementType type;
};

struct PrintStatement {
    struct Statement statement;
    struct Expression *expression;
};

struct ExpressionStatement {
    struct Statement statement;
    struct Expression *expression;
};

#endif