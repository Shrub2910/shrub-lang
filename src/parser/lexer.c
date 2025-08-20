#include "parser/lexer.h"

#include <stdlib.h>

#include "error/error.h"

struct Lexer *lexer_init(char *input, const size_t size) {
    struct Lexer *lexer = malloc(sizeof(struct Lexer));

    if (!lexer) {
        error_throw(MALLOC_ERROR, "Failed to allcoate lexer");
    }

    lexer->input = input;
    lexer->size = size;

    return lexer;
}
