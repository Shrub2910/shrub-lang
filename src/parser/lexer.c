#include "parser/lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "error/error.h"
#include "parser/token_vector.h"

#define CHAR_ALPHA 1
#define CHAR_DIGIT 2

#define CHAR_VECTOR_INITIAL_SIZE 16

struct CharVector {
    char *string;
    size_t used;
    size_t size;
};

struct CharVector *char_vector_init() {
    struct CharVector *char_vector = malloc(sizeof(struct CharVector));

    if (!char_vector) {
        error_throw(MALLOC_ERROR, "Failed to allocate char vector");
    }

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

    if (!lexer) {
        error_throw(MALLOC_ERROR, "Failed to allocate lexer");
    }

    lexer->input = input;
    lexer->current = input;
    lexer->size = size;

    lexer->token_vector = parser_token_vector_init();

    for (int i = 0; i < 255; ++i) {
        lexer->char_table[i] = 0;
    }

    for (int i = 'A'; i <= 'Z'; ++i) {
        lexer->char_table[i] |= CHAR_ALPHA;
    }

    for (int i = 'a'; i <= 'z'; ++i) {
        lexer->char_table[i] |= CHAR_ALPHA;
    }

    for (int i = '0'; i <= '9'; ++i) {
        lexer->char_table[i] |= CHAR_DIGIT;
    }


    return lexer;
}

static struct Token lexer_get_next_token(struct Lexer *lexer) {
    const char previous_char = *(lexer->current++);
    switch (previous_char) {
        case '+': {
            return (struct Token){.type = PLUS};
        }
        case '*': {
            return (struct Token){.type=TIMES};
        }
        case '-': {
            return (struct Token){.type=MINUS};
        }
        case '/': {
            return (struct Token){.type=DIVIDE};
        }
        case '(': {
            return (struct Token){.type=L_BRACKET};
        }
        case ')': {
            return (struct Token){.type=R_BRACKET};
        }
        default: {
            if ((lexer->char_table[previous_char] & CHAR_DIGIT) == CHAR_DIGIT) {
                struct CharVector *char_vector = char_vector_init();
                char_vector_insert(char_vector, previous_char);

                while ((lexer->char_table[*(lexer->current)] & CHAR_DIGIT) == CHAR_DIGIT
                    && lexer->current - lexer->input < lexer->size) {
                    char_vector_insert(char_vector, *(lexer->current++));
                }

                if (*(lexer->current) == '.') {
                    char_vector_insert(char_vector, '.');
                    lexer->current++;
                    while ((lexer->char_table[*(lexer->current)] & CHAR_DIGIT) == CHAR_DIGIT
                        && lexer->current - lexer->input < lexer->size) {
                        char_vector_insert(char_vector, *(lexer->current++));
                    }
                }

                char_vector_insert(char_vector, '\0');
                const double number = strtod(char_vector->string, NULL);
                char_vector_free(char_vector);

                return (struct Token){.type = NUMBER, .number=number};
            }

            char buffer[100];
            sprintf(buffer, "Character %c is not supported", previous_char);
            error_throw(LEXICAL_ERROR, buffer);
        }
    }
}

void lexer_tokenize(struct Lexer *lexer) {
    while (lexer->current - lexer->input < lexer->size) {
        while (isspace(*(lexer->current))) {
            lexer->current++;
        }
        parser_token_vector_insert(lexer->token_vector, lexer_get_next_token(lexer));
    }
}

void lexer_free(struct Lexer *lexer) {
    lexer->input = NULL;
    lexer->size = 0;

    free(lexer);
}
