#ifndef COMPILER_H
#define COMPILER_H

#include <stddef.h>
#include "vm/vm.h"
#include "parser/statement_vector.h"
#include "vm/instruction_buffer.h"

struct CompilerContext {
    struct VM *vm;
    struct InstructionBuffer *instruction_buffer;

    struct Environment *environment;
};

void compiler_compile_statements(
    struct CompilerContext *compiler_context,
    const struct StatementVector *statement_vector)
;

#endif
