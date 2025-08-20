#include <stdio.h>

#include "parser/token.h"
#include "parser/token_vector.h"

static void print_token(const struct Token token) {
    switch (token.type) {
        case NUMBER: {
            const double number = token.number;
            printf("NUMBER: %lf\n", number);
            break;
        }
        case PLUS: {
            printf("PLUS\n");
            break;
        }
        case MINUS: {
            printf("MINUS\n");
            break;
        }
        case TIMES: {
            printf("TIMES\n");
            break;
        }
        case DIVIDE: {
            printf("DIVIDE\n");
            break;
        }
        case L_BRACKET: {
            printf("L_BRACKET\n");
            break;
        }
        case R_BRACKET: {
            printf("R_BRACKET\n");
            break;
        }
        case PRINT: {
            printf("PRINT\n");
            break;
        }
        default:
            printf("UNKNOWN TOKEN");
    }
}

void print_tokens(const struct TokenVector *token_vector) {
    for (size_t i = 0; i < token_vector->used; ++i) {
        print_token(token_vector->tokens[i]);
    }
}