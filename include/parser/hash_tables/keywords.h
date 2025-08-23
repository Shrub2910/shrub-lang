#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stddef.h>
#include "parser/token.h"

#define KEYWORDS_TABLE_SIZE 32

struct Keyword {
    const char *key;
    enum TokenType type;
};

struct KeywordsTable {
    struct Keyword table[KEYWORDS_TABLE_SIZE];
    size_t used;
};

void keywords_init(struct KeywordsTable *keywords);
void keywords_insert(struct KeywordsTable *keywords, const char *key, enum TokenType token);
enum TokenType keywords_index(const struct KeywordsTable *keywords, const char *key);

#endif