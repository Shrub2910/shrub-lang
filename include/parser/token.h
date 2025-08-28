#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

enum TokenType {
    NUMBER_TOKEN,
    PLUS_TOKEN,
    MINUS_TOKEN,
    TIMES_TOKEN,
    DIVIDE_TOKEN,
    L_BRACKET_TOKEN,
    R_BRACKET_TOKEN,
    SEMI_COLON_TOKEN,
    PRINT_TOKEN,
    DO_TOKEN,
    END_TOKEN,
    IDENTIFIER_TOKEN,
    LET_TOKEN,
    EQUAL_TOKEN,
    STRING_TOKEN,
    BOOLEAN_TOKEN,
    DOUBLE_EQUAL_TOKEN,
    NOT_EQUAL_TOKEN,
    GREATER_TOKEN,
    LESS_TOKEN,
    GREATER_EQUAL_TOKEN,
    LESS_EQUAL_TOKEN,
    NIL_TOKEN,
    BANG_TOKEN,
    AND_TOKEN,
    OR_TOKEN,
    IF_TOKEN,
    ELSE_IF_TOKEN,
    ELSE_TOKEN,
    THEN_TOKEN,
    WHILE_TOKEN,
    NOTHING_TOKEN,
};

struct Token {
    enum TokenType type;
    union {
        char *string;
        bool boolean;
        double number;
    };
};

#endif
