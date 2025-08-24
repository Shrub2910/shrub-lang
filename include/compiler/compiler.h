#ifndef COMPILER_H
#define COMPILER_H

#include "vm/vm.h"
#include "parser/statement_vector.h"

struct Compiler {
    struct StatementVector *statement_vector;
    struct VM *vm;
};

struct Compiler compiler_init(struct StatementVector *statement_vector, struct VM *vm);
void compiler_compile_statements(struct Compiler *compiler);

#endif
