#include <stdlib.h>

#include "vm/vm.h"
#include "compiler/compiler.h"
#include "parser/statement_vector.h"
#include "parser/statements.h"
#include "parser/expressions.h"
#include "parser/token.h"
#include "vm/opcodes.h"
#include "vm/values.h"

static void compiler_compile_expression(struct Compiler *compiler, const struct Expression *expression);
static void compiler_compile_binary_expression(struct Compiler *compiler, const struct BinaryExpression *expression);
static void compiler_compile_literal_expression
    (const struct Compiler *compiler, const struct LiteralExpression *expression);
static void compiler_compile_statement(struct Compiler *compiler, const struct Statement *statement);
static void compiler_compile_print_statement(struct Compiler *compiler, const struct PrintStatement *print_statement);

struct Compiler compiler_init(struct StatementVector *statement_vector, struct VM *vm) {
    const struct Compiler compiler = {.statement_vector =  statement_vector, .vm = vm};

    return compiler;
}

void compiler_compile_statements(struct Compiler *compiler) {
    for (size_t i = 0; i < compiler->statement_vector->used; ++i) {
        compiler_compile_statement(compiler, compiler->statement_vector->statements[i]);
    }
    INSERT_INSTRUCTIONS(compiler->vm->instruction_buffer, HALT);
}

static void compiler_compile_statement(struct Compiler *compiler, const struct Statement *statement) {
    switch (statement->type) {
        case PRINT_STATEMENT: compiler_compile_print_statement(compiler, (struct PrintStatement *)statement); break;
        case EXPRESSION_STATEMENT: {
            const struct ExpressionStatement *expression_statement = (const struct ExpressionStatement *)statement;
            compiler_compile_expression(compiler, expression_statement->expression);
            INSERT_INSTRUCTIONS(compiler->vm->instruction_buffer, POP_TOP);
        }
    }
}

static void compiler_compile_print_statement(struct Compiler *compiler, const struct PrintStatement *print_statement) {
    compiler_compile_expression(compiler, print_statement->expression);
    INSERT_INSTRUCTIONS(compiler->vm->instruction_buffer, PRINT);
}

static void compiler_compile_expression(struct Compiler *compiler, const struct Expression *expression) {
    switch (expression->type) {
        case BINARY_EXPRESSION: compiler_compile_binary_expression(compiler, (struct BinaryExpression *) expression);
            break;
        case LITERAL_EXPRESSION: compiler_compile_literal_expression(compiler, (struct LiteralExpression *) expression);
            break;
        default: break;
    }
}

static void compiler_compile_binary_expression(struct Compiler *compiler, const struct BinaryExpression *expression) {
    compiler_compile_expression(compiler, expression->left);
    compiler_compile_expression(compiler, expression->right);

    switch (expression->operator) {
        case PLUS_TOKEN: {
            vm_insert_instruction_buffer(compiler->vm->instruction_buffer, ADD);
            break;
        }
        case MINUS_TOKEN: {
            vm_insert_instruction_buffer(compiler->vm->instruction_buffer, SUB);
            break;
        }
        case TIMES_TOKEN: {
            vm_insert_instruction_buffer(compiler->vm->instruction_buffer, MUL);
            break;
        }
        case DIVIDE_TOKEN: {
            vm_insert_instruction_buffer(compiler->vm->instruction_buffer, DIV);
            break;
        }
        default: break;
    }
}

static void compiler_compile_literal_expression
(const struct Compiler *compiler, const struct LiteralExpression *expression) {
    vm_add_const(compiler->vm, (struct Value) {.type = TYPE_NUMBER, .number = expression->value});
    INSERT_INSTRUCTIONS(compiler->vm->instruction_buffer, LOAD_CONST, compiler->vm->constant_count - 1);
}