#include <stdio.h>

#include "parser/token.h"
#include "parser/token_vector.h"

static void print_token(const struct Token token) {
    switch (token.type) {
        case NUMBER_TOKEN: {
            const double number = token.number;
            printf("NUMBER: %lf\n", number);
            break;
        }
        case PLUS_TOKEN: {
            printf("PLUS\n");
            break;
        }
        case MINUS_TOKEN: {
            printf("MINUS\n");
            break;
        }
        case TIMES_TOKEN: {
            printf("TIMES\n");
            break;
        }
        case DIVIDE_TOKEN: {
            printf("DIVIDE\n");
            break;
        }
        case L_BRACKET_TOKEN: {
            printf("L_BRACKET\n");
            break;
        }
        case R_BRACKET_TOKEN: {
            printf("R_BRACKET\n");
            break;
        }
        case PRINT_TOKEN: {
            printf("PRINT\n");
            break;
        }
        case SEMI_COLON_TOKEN: {
            printf("SEMI_COLON\n");
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