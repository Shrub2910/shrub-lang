#ifndef PRINT_STATEMENT_H
#define PRINT_STATEMENT_H

#include <stddef.h>
#include "parser/statement_vector.h"

void print_statements(const struct StatementVector *statement_vector, size_t indent_count);

#endif