#ifndef TOKEN_H
#define TOKEN_H

enum TokenType {
    NUMBER,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    L_BRACKET,
    R_BRACKET,
    SEMI_COLON,
    PRINT,
};

struct Token {
    enum TokenType type;
    union {
        double number;
    };
};

#endif
