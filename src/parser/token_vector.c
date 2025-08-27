#include <stdlib.h>
#include "parser/token_vector.h"

#include "error/error.h"

struct TokenVector *parser_token_vector_init(void) {
    struct TokenVector *token_vector = malloc(sizeof(struct TokenVector));

    if (!token_vector) {
        error_throw(MALLOC_ERROR, "Failed to allocate token vector");
    }

    token_vector->used = 0;
    token_vector->size = TOKEN_VECTOR_INITIAL_SIZE;

    token_vector->tokens = malloc(TOKEN_VECTOR_INITIAL_SIZE * sizeof(struct Token));

    if (!token_vector->tokens) {
        free(token_vector);
        error_throw(MALLOC_ERROR, "Failed to allocate token vector");
    }

    return token_vector;
}

void parser_token_vector_insert(struct TokenVector *token_vector, const struct Token token) {
    if (token_vector->used == token_vector->size) {
        token_vector->size *= 2;
        struct Token *tokens = realloc(token_vector->tokens, token_vector->size * sizeof(struct Token));

        if (!tokens) {
            error_throw(MALLOC_ERROR, "Failed to reallocate token vector");
        }

        token_vector->tokens = tokens;
    }
    token_vector->tokens[token_vector->used++] = token;
}

void parser_token_vector_free(struct TokenVector *token_vector) {
    for (size_t i = 0; i < token_vector->used; i++) {
        const struct Token *token = token_vector->tokens + i;
        if (token->type == IDENTIFIER_TOKEN || token->type == STRING_TOKEN) {
            free(token->string);
        }
    }

    free(token_vector->tokens);
    token_vector->tokens = NULL;
    token_vector->used = token_vector->size = 0;
    free(token_vector);
}
