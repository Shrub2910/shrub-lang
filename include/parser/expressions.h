#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <stdbool.h>
#include "token.h"

enum ExpressionType {
    LITERAL_EXPRESSION,
    BINARY_EXPRESSION,
    ASSIGNMENT_EXPRESSION,
    UNARY_EXPRESSION,
};

enum LiteralType {
    NUMBER_LITERAL,
    IDENTIFIER_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    NIL_LITERAL,
};

struct Expression {
    enum ExpressionType type;
};

struct LiteralExpression {
    struct Expression expression;
    enum LiteralType literal_type;
    union {
        double number;
        struct {
            char *identifier;
            size_t offset;
        };
        char *string;
        bool boolean;
    };
};

struct BinaryExpression {
    struct Expression expression;
    struct Expression *left;
    struct Expression *right;
    enum TokenType operator;
};

struct UnaryExpression {
    struct Expression expression;
    struct Expression *operand;
    enum TokenType operator;
};

struct AssignmentExpression {
    struct Expression expression;
    char *identifier_name;
    struct Expression *right;

    size_t offset;
};

#endif