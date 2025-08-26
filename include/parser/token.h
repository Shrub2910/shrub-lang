#ifndef TOKEN_H
#define TOKEN_H

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
    NOTHING_TOKEN,
};

struct Token {
    enum TokenType type;
    union {
        double number;
    };
};

#endif
