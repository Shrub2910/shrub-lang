#include <stdlib.h>

#include "vm/vm.h"
#include "compiler/compiler.h"
#include "parser/statement_vector.h"
#include "parser/statements.h"
#include "parser/expressions.h"
#include "parser/token.h"
#include "vm/opcodes.h"
#include "vm/values.h"
#include "vm/instruction_buffer.h"

static void compiler_compile_expression(
    struct Compiler *compiler,
    const struct Expression *expression,
    struct InstructionBuffer *instruction_buffer
);
static void compiler_compile_binary_expression(
    struct Compiler *compiler,
    const struct BinaryExpression *expression,
    struct InstructionBuffer *instruction_buffer
);
static void compiler_compile_literal_expression(
    const struct Compiler *compiler,
    const struct LiteralExpression *expression,
    struct InstructionBuffer *instruction_buffer
);
static void compiler_compile_statement(
    struct Compiler *compiler,
    const struct Statement *statement,
    struct InstructionBuffer *instruction_buffer
);
static void compiler_compile_print_statement(
    struct Compiler *compiler,
    const struct PrintStatement *print_statement,
    struct InstructionBuffer *instruction_buffer
);

struct Compiler compiler_init(struct StatementVector *statement_vector, struct VM *vm) {
    const struct Compiler compiler = {.statement_vector =  statement_vector, .vm = vm};

    return compiler;
}

void compiler_compile_statements(
    struct Compiler *compiler,
    struct InstructionBuffer *instruction_buffer
) {
    for (size_t i = 0; i < compiler->statement_vector->used; ++i) {
        compiler_compile_statement(compiler, compiler->statement_vector->statements[i], instruction_buffer);
    }
    INSERT_INSTRUCTIONS(instruction_buffer, HALT);
}

static void compiler_compile_statement(
    struct Compiler *compiler,
    const struct Statement *statement,
    struct InstructionBuffer *instruction_buffer
) {
    switch (statement->type) {
        case PRINT_STATEMENT:
            compiler_compile_print_statement(
                compiler,
                (struct PrintStatement *)statement, instruction_buffer
            );
            break;
        case BLOCK_STATEMENT: {
            const struct BlockStatement *block_statement = (struct BlockStatement *)statement;
            for (size_t i = 0; i < block_statement->statement_vector->used; ++i) {
                compiler_compile_statement(
                    compiler,
                    block_statement->statement_vector->statements[i],
                    instruction_buffer
                );
            }
            break;
        }
        case EXPRESSION_STATEMENT: {
            const struct ExpressionStatement *expression_statement = (const struct ExpressionStatement *)statement;
            compiler_compile_expression(compiler, expression_statement->expression, instruction_buffer);
            INSERT_INSTRUCTIONS(instruction_buffer, POP_TOP);
            break;
        }
    }
}

static void compiler_compile_print_statement(
    struct Compiler *compiler,
    const struct PrintStatement *print_statement,
    struct InstructionBuffer *instruction_buffer
) {
    compiler_compile_expression(compiler, print_statement->expression, instruction_buffer);
    INSERT_INSTRUCTIONS(instruction_buffer, PRINT);
}

static void compiler_compile_expression(
    struct Compiler *compiler,
    const struct Expression *expression,
    struct InstructionBuffer *instruction_buffer
) {
    switch (expression->type) {
        case BINARY_EXPRESSION:
            compiler_compile_binary_expression(
                compiler,
                (struct BinaryExpression *) expression,
                instruction_buffer
            );
            break;
        case LITERAL_EXPRESSION:
            compiler_compile_literal_expression(
                compiler,
                (struct LiteralExpression *) expression,
                instruction_buffer
            );
            break;
        default: break;
    }
}

static void compiler_compile_binary_expression(
    struct Compiler *compiler,
    const struct BinaryExpression *expression,
    struct InstructionBuffer *instruction_buffer
) {
    compiler_compile_expression(compiler, expression->left, instruction_buffer);
    compiler_compile_expression(compiler, expression->right, instruction_buffer);

    switch (expression->operator) {
        case PLUS_TOKEN: {
            INSERT_INSTRUCTIONS(instruction_buffer, ADD);
            break;
        }
        case MINUS_TOKEN: {
            INSERT_INSTRUCTIONS(instruction_buffer, SUB);
            break;
        }
        case TIMES_TOKEN: {
            INSERT_INSTRUCTIONS(instruction_buffer, MUL);
            break;
        }
        case DIVIDE_TOKEN: {
            INSERT_INSTRUCTIONS(instruction_buffer, DIV);
            break;
        }
        default: break;
    }
}

static void compiler_compile_literal_expression
(
    const struct Compiler *compiler,
    const struct LiteralExpression *expression,
    struct InstructionBuffer *instruction_buffer
) {
    vm_add_const(compiler->vm, (struct Value) {.type = TYPE_NUMBER, .number = expression->value});
    INSERT_INSTRUCTIONS(instruction_buffer, LOAD_CONST, compiler->vm->constant_count - 1);
}