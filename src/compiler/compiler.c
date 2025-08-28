#include <stdlib.h>
#include <string.h>

#include "vm/vm.h"
#include "compiler/compiler.h"

#include "compiler/environment.h"
#include "compiler/scope.h"
#include "error/error.h"
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
static void compiler_compile_assignment_expression(
    struct CompilerContext *compiler_context,
    const struct AssignmentExpression *assignment_expression
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
            compiler_push_scope(compiler_context->environment);
            for (size_t i = 0; i < block_statement->statement_vector->used; ++i) {
                compiler_compile_statement(
                    compiler_context,
                    block_statement->statement_vector->statements[i]
                );
            }
            compiler_pop_scope(compiler_context->environment);
            break;
        }
        case LET_STATEMENT: {
            const struct LetStatement *let_statement = (struct LetStatement *)statement;

            if (compiler_search_scope(
                compiler_get_top_scope(compiler_context->environment),
                let_statement->identifier_name
            )) {
                error_throw(NAME_ERROR, "Can only define variable once per scope");
            }

            if (let_statement->is_nil) {
                INSERT_INSTRUCTIONS(
                    compiler_context->instruction_buffer,
                    PUSH_NIL
                );
            } else {
                compiler_compile_expression(compiler_context, let_statement->expression);
            }

            compiler_insert_environment(compiler_context->environment, let_statement->identifier_name);
            INSERT_INSTRUCTIONS(
                compiler_context->instruction_buffer,
                STORE_VAR,
                compiler_context->environment->variable_count++
            );
            break;
        }
        case EXPRESSION_STATEMENT: {
            const struct ExpressionStatement *expression_statement = (const struct ExpressionStatement *)statement;
            compiler_compile_expression(compiler_context, expression_statement->expression);
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, DISCARD);
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
        case ASSIGNMENT_EXPRESSION: {
            compiler_compile_assignment_expression(
                compiler_context,
                (struct AssignmentExpression *)expression
            );
        }
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

static void compiler_compile_assignment_expression(
    struct CompilerContext *compiler_context,
    const struct AssignmentExpression *assignment_expression
) {
    struct ScopeVariable *variable = compiler_search_environment(
        compiler_context->environment,
        assignment_expression->identifier_name
    );

    if (!variable) {
        error_throw(NAME_ERROR, "Variable undefined");
    }

    compiler_compile_expression(compiler_context, assignment_expression->right);
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, STORE_VAR, variable->offset);
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LOAD_VAR, variable->offset);
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
        case DOUBLE_EQUAL_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, EQUAL);
            break;
        }
        case NOT_EQUAL_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, NOT_EQUAL);
            break;
        }
        case LESS_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LESS);
            break;
        }
        case GREATER_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, GREATER);
            break;
        }
        case LESS_EQUAL_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LESS_EQUAL);
            break;
        }
        case GREATER_EQUAL_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, GREATER_EQUAL);
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
    switch (expression->literal_type) {
        case NUMBER_LITERAL: {
            vm_add_const(compiler_context->vm, NUMBER(expression->number));
            INSERT_INSTRUCTIONS(
                compiler_context->instruction_buffer,
                LOAD_CONST,
                compiler_context->vm->constant_count - 1)
            ;
            break;
        }
        case IDENTIFIER_LITERAL: {
            struct ScopeVariable *scope_variable =
                compiler_search_environment(compiler_context->environment, expression->identifier);

            if (!scope_variable) {
                error_throw(NAME_ERROR, "Variable not defined");
            }

            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LOAD_VAR, scope_variable->offset);
            break;
        }
        case STRING_LITERAL: {
            vm_add_const(compiler_context->vm, STRING(expression->string, strlen(expression->string)));
            INSERT_INSTRUCTIONS(
                compiler_context->instruction_buffer,
                LOAD_CONST,
                compiler_context->vm->constant_count - 1
            );
            break;
        }
        case BOOLEAN_LITERAL: {
            vm_add_const(compiler_context->vm, BOOLEAN(expression->boolean));
            INSERT_INSTRUCTIONS(
                compiler_context->instruction_buffer,
                LOAD_CONST,
                compiler_context->vm->constant_count - 1
            );
            break;
        }
        case NIL_LITERAL: {
            vm_add_const(compiler_context->vm, NIL());
            INSERT_INSTRUCTIONS(
                compiler_context->instruction_buffer,
                LOAD_CONST,
                compiler_context->vm->constant_count - 1
            );
            break;
        }
    }

}