#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

struct Lexer {
    char *input;
    char *current;
    size_t size;
};

struct Lexer *lexer_init(char *input, size_t size);
void lexer_free(struct Lexer *lexer);

#endif