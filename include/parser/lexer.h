#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include "token_vector.h"

struct Lexer {
    char *input;
    char *current;
    size_t size;

    struct TokenVector *token_vector;

    unsigned char char_table[256];
};

struct Lexer *lexer_init(char *input, size_t size);
void lexer_tokenize(struct Lexer *lexer);
void lexer_free(struct Lexer *lexer);

#endif