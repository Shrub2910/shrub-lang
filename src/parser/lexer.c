#include "parser/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "error/error.h"
#include "parser/token_vector.h"
#include "parser/hash_tables/keywords.h"

#define CHAR_ALPHA 1
#define CHAR_DIGIT 2
#define CHAR_ALPHA_NUMERIC 4
#define CHAR_VECTOR_INITIAL_SIZE 16

static struct Token create_number(struct Lexer *lexer, char previous_char);
static struct Token create_keyword(struct Lexer *lexer, char previous_char);
static struct Token create_string(struct Lexer *lexer);

struct CharVector {
    char *string;
    size_t used;
    size_t size;
};

static inline bool lexer_has_char(const struct Lexer *lexer) {
    return (size_t)(lexer->current - lexer->input) < lexer->size;
}

static inline int lexer_peek(const struct Lexer *lexer) {
    return lexer_has_char(lexer) ? *(lexer->current) : '\0';
}

struct CharVector *char_vector_init(void) {
    struct CharVector *char_vector = malloc(sizeof(struct CharVector));

    if (!char_vector)
        error_throw(MALLOC_ERROR, "Failed to allocate char vector");

    char_vector->used = 0;
    char_vector->size = CHAR_VECTOR_INITIAL_SIZE;
    char_vector->string = malloc(char_vector->size * sizeof(char));

    if (!char_vector->string) {
        free(char_vector);
        error_throw(MALLOC_ERROR, "Failed to allocate char vector string");
    }

    return char_vector;
}

static void char_vector_insert(struct CharVector *char_vector, const char character) {
    if (char_vector->used == char_vector->size) {
        char_vector->size *= 2;
        char *new_string = realloc(char_vector->string, char_vector->size * sizeof(char));

        if (!new_string) {
            free(char_vector);
            error_throw(MALLOC_ERROR, "Failed to allocate char vector string");
        }

        char_vector->string = new_string;
    }

    char_vector->string[char_vector->used++] = character;
}

static void char_vector_free(struct CharVector *char_vector) {
    free(char_vector->string);
    char_vector->string = NULL;
    char_vector->used = char_vector->size = 0;
    free(char_vector);
}

struct Lexer *lexer_init(char *input, const size_t size) {
    struct Lexer *lexer = malloc(sizeof(struct Lexer));

    if (!lexer)
        error_throw(MALLOC_ERROR, "Failed to allocate lexer");

    lexer->input = input;
    lexer->current = input;
    lexer->size = size;

    lexer->token_vector = parser_token_vector_init();

    for (int i = 0; i < 255; ++i)
        lexer->char_table[i] = 0;

    for (int i = 'A'; i <= 'Z'; ++i)
        lexer->char_table[i] |= CHAR_ALPHA | CHAR_ALPHA_NUMERIC;

    for (int i = 'a'; i <= 'z'; ++i)
        lexer->char_table[i] |= CHAR_ALPHA | CHAR_ALPHA_NUMERIC;

    for (int i = '0'; i <= '9'; ++i)
        lexer->char_table[i] |= CHAR_DIGIT | CHAR_ALPHA_NUMERIC;

    keywords_init(&lexer->keywords);

    keywords_insert(&lexer->keywords, "print", PRINT_TOKEN);
    keywords_insert(&lexer->keywords, "do", DO_TOKEN);
    keywords_insert(&lexer->keywords, "end", END_TOKEN);
    keywords_insert(&lexer->keywords, "let", LET_TOKEN);
    keywords_insert(&lexer->keywords, "true", BOOLEAN_TOKEN);
    keywords_insert(&lexer->keywords, "false", BOOLEAN_TOKEN);
    keywords_insert(&lexer->keywords, "and", AND_TOKEN);
    keywords_insert(&lexer->keywords, "or", OR_TOKEN);
    keywords_insert(&lexer->keywords, "null", NIL_TOKEN);
    keywords_insert(&lexer->keywords, "if", IF_TOKEN);
    keywords_insert(&lexer->keywords, "else", ELSE_TOKEN);
    keywords_insert(&lexer->keywords, "elseif", ELSE_IF_TOKEN);
    keywords_insert(&lexer->keywords, "then", THEN_TOKEN);
    keywords_insert(&lexer->keywords, "while", WHILE_TOKEN);
    keywords_insert(&lexer->keywords, "fn", FUNCTION_TOKEN);
    keywords_insert(&lexer->keywords, "return", RETURN_TOKEN);

    return lexer;
}

static struct Token lexer_get_next_token(struct Lexer *lexer) {
    const char previous_char = *(lexer->current++);

    switch (previous_char) {
        case '+': return (struct Token){.type = PLUS_TOKEN};
        case '*': return (struct Token){.type = TIMES_TOKEN};
        case '-': return (struct Token){.type = MINUS_TOKEN};
        case '/': return (struct Token){.type = DIVIDE_TOKEN};
        case '%': return (struct Token){.type = MOD_TOKEN};
        case '(': return (struct Token){.type = L_BRACKET_TOKEN};
        case ')': return (struct Token){.type = R_BRACKET_TOKEN};
        case ';': return (struct Token){.type = SEMI_COLON_TOKEN};
        case ',': return (struct Token){.type = COMMA_TOKEN};
        case '=': {
            if (lexer_peek(lexer) == '=') {
                lexer->current++;
                return (struct Token){.type =  DOUBLE_EQUAL_TOKEN};
            }
            return (struct Token){.type = EQUAL_TOKEN};
        }
        case '>': {
            if (lexer_peek(lexer) == '=') {
                lexer->current++;
                return (struct Token){.type = GREATER_EQUAL_TOKEN};
            }
            return (struct Token){.type = GREATER_TOKEN};
        }
        case '<': {
            if (lexer_peek(lexer) == '=') {
                lexer->current++;
                return (struct Token){.type = LESS_EQUAL_TOKEN};
            }
            return (struct Token){.type = LESS_TOKEN};
        }
        case '!': {
            if (lexer_peek(lexer) == '=') {
                lexer->current++;
                return (struct Token){.type = NOT_EQUAL_TOKEN};
            }
            return (struct Token){.type = BANG_TOKEN};
        }
        case '"': return create_string(lexer);

        default: {
            if ((lexer->char_table[(unsigned char)previous_char] & CHAR_DIGIT) == CHAR_DIGIT)
                return create_number(lexer, previous_char);

            if ((lexer->char_table[(unsigned char)previous_char] & CHAR_ALPHA) == CHAR_ALPHA)
                return create_keyword(lexer, previous_char);

            char buffer[100];
            sprintf(buffer, "Character %c is not supported", previous_char);
            error_throw(LEXICAL_ERROR, buffer);
        }
    }
}

static struct Token create_number(struct Lexer *lexer, const char previous_char) {
    struct CharVector *char_vector = char_vector_init();

    char_vector_insert(char_vector, previous_char);

    while (lexer_has_char(lexer) && (lexer->char_table[(unsigned char)lexer_peek(lexer)] & CHAR_DIGIT))
        char_vector_insert(char_vector, *(lexer->current++));

    if (lexer_has_char(lexer) && lexer_peek(lexer) == '.') {
        char_vector_insert(char_vector, '.');
        lexer->current++;

        while (lexer_has_char(lexer) && (lexer->char_table[(unsigned char)lexer_peek(lexer)] & CHAR_DIGIT))
            char_vector_insert(char_vector, *(lexer->current++));
    }

    char_vector_insert(char_vector, '\0');

    const double number = strtod(char_vector->string, NULL);
    char_vector_free(char_vector);

    return (struct Token){.type = NUMBER_TOKEN, .number = number};
}

static struct Token create_keyword(struct Lexer *lexer, const char previous_char) {
    struct CharVector *char_vector = char_vector_init();

    char_vector_insert(char_vector, previous_char);

    while (lexer_has_char(lexer) &&
           ((lexer->char_table[(unsigned char)lexer_peek(lexer)] & CHAR_ALPHA_NUMERIC) || lexer_peek(lexer) == '_'))
        char_vector_insert(char_vector, *(lexer->current++));

    char_vector_insert(char_vector, '\0');

    const enum TokenType token_type = keywords_index(&lexer->keywords, char_vector->string);

    struct Token token = {.type = token_type};

    if (token_type == IDENTIFIER_TOKEN)
        token.string = strdup(char_vector->string);

    if (token_type == BOOLEAN_TOKEN) {
        token.boolean = strcmp(char_vector->string, "true") == 0 ? true : false;
    }

    char_vector_free(char_vector);

    return token;
}

static struct Token create_string(struct Lexer *lexer) {
    struct CharVector *char_vector = char_vector_init();

    while (lexer_has_char(lexer) && lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\n') {
        char_vector_insert(char_vector, *(lexer->current++));
    }

    if (lexer_peek(lexer) != '"') {
        error_throw(LEXICAL_ERROR, "String unterminated");
    }

    lexer->current++;

    char_vector_insert(char_vector, '\0');

    const enum TokenType token_type = STRING_TOKEN;
    const struct Token token = {.type = token_type, .string = strdup(char_vector->string)};

    char_vector_free(char_vector);

    return token;
}

void lexer_tokenize(struct Lexer *lexer) {
    while (lexer_has_char(lexer)) {
        while (lexer_has_char(lexer) && isspace(lexer_peek(lexer)))
            lexer->current++;

        if (!lexer_has_char(lexer))
            break;

        parser_token_vector_insert(lexer->token_vector, lexer_get_next_token(lexer));
    }
}

void lexer_free(struct Lexer *lexer) {
    parser_token_vector_free(lexer->token_vector);

    lexer->input = NULL;
    lexer->size = 0;

    free(lexer);
}
