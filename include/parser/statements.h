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
    IF_STATEMENT,
    WHILE_STATEMENT,
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

struct ExpressionStatement {
    struct Statement statement;
    struct Expression *expression;
};

#endif