#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include "token.h"

enum ExpressionType {
    LITERAL_EXPRESSION,
    BINARY_EXPRESSION
};

enum OperatorType {
    PLUS_OPERATOR,
    MINUS_OPERATOR,
    TIMES_OPERATOR,
    DIVIDE_OPERATOR
};

struct Expression {
    enum ExpressionType type;
};

struct LiteralExpression {
    struct Expression expression;
    double value;
};

struct BinaryExpression {
    struct Expression expression;
    struct Expression *left;
    struct Expression *right;
    enum OperatorType operator;
};

#endif