#include <stdlib.h>
#include "parser/parser.h"

#include "error/error.h"
#include "parser/statement_vector.h"

struct Parser *parser_init(struct TokenVector *token_vector) {
    struct Parser *parser = malloc(sizeof(struct Parser));

    if (!parser) {
        error_throw(MALLOC_ERROR, "Failed to allocate parser");
    }

    parser->token_vector = token_vector;

    parser->current_token = parser->token_vector->tokens;
    parser->next_token = parser->current_token + 1;

    parser->statement_vector = parser_statement_vector_init();

    if (!parser->statement_vector) {
        free(parser);
        error_throw(MALLOC_ERROR, "Failed to allocate parser");
    }

    return parser;
}

