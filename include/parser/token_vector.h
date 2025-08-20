#ifndef TOKEN_VECTOR_H
#define TOKEN_VECTOR_H

#include <stddef.h>
#include "token.h"

struct TokenVector {
  struct Token *tokens;
  size_t used;
  size_t size;
};

struct TokenVector *parser_token_vector_init();
void parser_token_vector_insert(struct TokenVector *token_vector, struct Token token);
void parser_token_vector_free(struct TokenVector *token_vector);

#define TOKEN_VECTOR_INITIAL_SIZE 64

#endif
