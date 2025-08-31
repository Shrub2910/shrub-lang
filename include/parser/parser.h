#ifndef PARSER_H
#define PARSER_H

#include "token_vector.h"

#include "parser/statement_vector.h"

struct Parser {
    struct TokenVector *token_vector;
    struct Token *current_token;
    size_t index;

    struct StatementVector *statement_vector;
};

struct Parser *parser_init(struct TokenVector *token_vector);
void parser_parse(struct Parser *parser);
void parser_free(struct Parser *parser);

#define MAX_FUNCTION_ARGS 256

#endif