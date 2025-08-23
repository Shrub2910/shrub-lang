#include <stddef.h>
#include <string.h>

#include "parser/hash_tables/keywords.h"

#include "error/error.h"
#include "error/error_types.h"

#include "parser/token.h"

static size_t hash_string(const char *str) {
    size_t hash = 5381;
    int c;

    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void keywords_init(struct KeywordsTable *keywords) {
    for (size_t i = 0; i < KEYWORDS_TABLE_SIZE; ++i) {
        keywords->table[i].key = NULL;
    }

    keywords->used = 0;
}

void keywords_insert(struct KeywordsTable *keywords, const char *key, const enum TokenType token) {
    if (keywords->used == KEYWORDS_TABLE_SIZE) {
        error_throw(HASH_TABLE_ERROR, "Increase keywords size");
    }

    size_t index = hash_string(key) % KEYWORDS_TABLE_SIZE;

    while (keywords->table[index].key != NULL) {
        index = (index + 1) % KEYWORDS_TABLE_SIZE;
    }

    keywords->table[index] = (struct Keyword){.key = key, .type = token};
}

enum TokenType keywords_index(const struct KeywordsTable *keywords, const char *key) {
    const size_t index = hash_string(key) % KEYWORDS_TABLE_SIZE;

    while (keywords->table[index].key != NULL) {
        if (strcmp(keywords->table[index].key , key) == 0) {
            return keywords->table[index].type;
        }
    }

    return NOTHING_TOKEN;
}