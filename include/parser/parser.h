#ifndef PARSER_H
#define PARSER_H

#include "token_vector.h"

#include "parser/statement_vector.h"

struct Parser {
    struct TokenVector *token_vector;
    struct Token *current_token;
    struct Token *next_token;

    struct StatementVector *statement_vector;
};

struct Parser *parser_init(struct TokenVector *token_vector);

#endif