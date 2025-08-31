#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>
#include "vm/vm.h"
#include "parser/statement_vector.h"
#include "vm/instruction_buffer.h"

struct CompilerContext {
    struct InstructionBuffer *instruction_buffer;

    struct Environment *environment;

    struct Value *constants;
    size_t constant_count;

    struct Function **functions;
    size_t function_count;
};

void compiler_compile_statements(
    struct CompilerContext *compiler_context,
    const struct StatementVector *statement_vector)
;

#endif
