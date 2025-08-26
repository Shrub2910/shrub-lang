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
    struct CompilerContext *compiler_context,
    const struct Expression *expression
);
static void compiler_compile_binary_expression(
    struct CompilerContext *compiler_context,
    const struct BinaryExpression *expression
);
static void compiler_compile_literal_expression(
    const struct CompilerContext *compiler_context,
    const struct LiteralExpression *expression
);
static void compiler_compile_statement(
    struct CompilerContext *compiler_context,
    const struct Statement *statement
);
static void compiler_compile_print_statement(
    struct CompilerContext *compiler_context,
    const struct PrintStatement *print_statement
);

void compiler_compile_statements(
    struct CompilerContext *compiler_context,
    const struct StatementVector *statement_vector
) {
    for (size_t i = 0; i < statement_vector->used; ++i) {
        compiler_compile_statement(compiler_context, statement_vector->statements[i]);
    }
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, HALT);
}

static void compiler_compile_statement(
    struct CompilerContext *compiler_context,
    const struct Statement *statement
) {
    switch (statement->type) {
        case PRINT_STATEMENT:
            compiler_compile_print_statement(
                compiler_context,
                (struct PrintStatement *)statement
            );
            break;
        case BLOCK_STATEMENT: {
            const struct BlockStatement *block_statement = (struct BlockStatement *)statement;
            for (size_t i = 0; i < block_statement->statement_vector->used; ++i) {
                compiler_compile_statement(
                    compiler_context,
                    block_statement->statement_vector->statements[i]
                );
            }
            break;
        }
        case EXPRESSION_STATEMENT: {
            const struct ExpressionStatement *expression_statement = (const struct ExpressionStatement *)statement;
            compiler_compile_expression(compiler_context, expression_statement->expression);
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, POP_TOP);
            break;
        }
    }
}

static void compiler_compile_print_statement(
    struct CompilerContext *compiler_context,
    const struct PrintStatement *print_statement
) {
    compiler_compile_expression(compiler_context, print_statement->expression);
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, PRINT);
}

static void compiler_compile_expression(
    struct CompilerContext *compiler_context,
    const struct Expression *expression
) {
    switch (expression->type) {
        case BINARY_EXPRESSION:
            compiler_compile_binary_expression(
                compiler_context,
                (struct BinaryExpression *) expression
            );
            break;
        case LITERAL_EXPRESSION:
            compiler_compile_literal_expression(
                compiler_context,
                (struct LiteralExpression *) expression
            );
            break;
        default: break;
    }
}

static void compiler_compile_binary_expression(
    struct CompilerContext *compiler_context,
    const struct BinaryExpression *expression
) {
    compiler_compile_expression(compiler_context, expression->left);
    compiler_compile_expression(compiler_context, expression->right);

    switch (expression->operator) {
        case PLUS_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, ADD);
            break;
        }
        case MINUS_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, SUB);
            break;
        }
        case TIMES_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, MUL);
            break;
        }
        case DIVIDE_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, DIV);
            break;
        }
        default: break;
    }
}

static void compiler_compile_literal_expression
(
    const struct CompilerContext *compiler_context,
    const struct LiteralExpression *expression
) {
    vm_add_const(compiler_context->vm, (struct Value) {.type = TYPE_NUMBER, .number = expression->value});
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LOAD_CONST, compiler_context->vm->constant_count - 1);
}