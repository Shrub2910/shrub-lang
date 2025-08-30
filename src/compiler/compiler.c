#include <stdlib.h>
#include <string.h>

#include "vm/vm.h"
#include "compiler/compiler.h"

#include <stdio.h>

#include "compiler/environment.h"
#include "compiler/scope.h"
#include "error/error.h"
#include "parser/statement_vector.h"
#include "parser/statements.h"
#include "parser/expressions.h"
#include "parser/token.h"
#include "utils/operand_conversion.h"
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
static void compiler_compile_unary_expression(
    struct CompilerContext *compiler_context,
    const struct UnaryExpression *unary_expression
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

static inline size_t get_current_position(const struct InstructionBuffer *instruction_buffer) {
    return instruction_buffer->used;
}

static inline size_t get_jump_position(const struct InstructionBuffer *instruction_buffer) {
    return instruction_buffer->used - 2;
}

static void patch_jump_position(struct InstructionBuffer *instruction_buffer, size_t start, size_t end) {
    uint8_t *operands = instruction_buffer->buffer + start;
    uint8_t new_operands[] = {FROM_SIGNED_WORD(end - (start + 2))};

    operands[0] = new_operands[0];
    operands[1] = new_operands[1];
}

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
        case LET_STATEMENT: {
            const struct LetStatement *let_statement = (struct LetStatement *)statement;

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
                let_statement->offset
            );
            break;
        }
        case EXPRESSION_STATEMENT: {
            const struct ExpressionStatement *expression_statement = (const struct ExpressionStatement *)statement;
            compiler_compile_expression(compiler_context, expression_statement->expression);
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, DISCARD);
            break;
        }
        case IF_STATEMENT: {
            const struct IfStatement *if_statement = (const struct IfStatement *)statement;
            struct InstructionBuffer *instruction_buffer = compiler_context->instruction_buffer;

            compiler_compile_expression(compiler_context, if_statement->condition);

            INSERT_INSTRUCTIONS(instruction_buffer, JUMP_IF_FALSE, 0, 0);
            size_t jf_position = get_jump_position(instruction_buffer);

            compiler_compile_statement(compiler_context, (struct Statement *)if_statement->then_block);

            if (if_statement->else_block) {
                INSERT_INSTRUCTIONS(instruction_buffer, JUMP, 0, 0);
                size_t j_position = get_jump_position(instruction_buffer);

                patch_jump_position(instruction_buffer, jf_position, get_current_position(instruction_buffer));

                compiler_compile_statement(compiler_context, if_statement->else_block);
                patch_jump_position(instruction_buffer, j_position, get_current_position(instruction_buffer));
            } else {
                patch_jump_position(instruction_buffer, jf_position, get_current_position(instruction_buffer));
            }
            break;
        }
        case WHILE_STATEMENT: {
            const struct WhileStatement *while_statement = (const struct WhileStatement *)statement;
            struct InstructionBuffer *instruction_buffer = compiler_context->instruction_buffer;

            const size_t j_position = get_current_position(instruction_buffer);
            compiler_compile_expression(compiler_context, while_statement->condition);
            INSERT_INSTRUCTIONS(instruction_buffer, JUMP_IF_FALSE, 0, 0);
            const size_t jf_position = get_jump_position(instruction_buffer);
            compiler_compile_statement(compiler_context, (struct Statement *)while_statement->body);
            INSERT_INSTRUCTIONS(
                instruction_buffer,
                JUMP,
                FROM_SIGNED_WORD(j_position - (get_current_position(instruction_buffer) + 3))
            );
            patch_jump_position(instruction_buffer, jf_position, get_current_position(instruction_buffer));
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
            break;
        }
        case BINARY_EXPRESSION:
            compiler_compile_binary_expression(
                compiler_context,
                (struct BinaryExpression *) expression
            );
            break;
        case UNARY_EXPRESSION:
            compiler_compile_unary_expression(
                compiler_context,
                (struct UnaryExpression *) expression
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
    compiler_compile_expression(compiler_context, assignment_expression->right);
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, STORE_VAR, assignment_expression->offset);
    INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LOAD_VAR, assignment_expression->offset);
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
        case AND_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, AND);
            break;
        }
        case OR_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, OR);
            break;
        }
        case MOD_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, MOD);
            break;
        }
        default: break;
    }
}

static void compiler_compile_unary_expression(
    struct CompilerContext *compiler_context,
    const struct UnaryExpression *unary_expression
) {
    compiler_compile_expression(compiler_context, unary_expression->operand);

    switch (unary_expression->operator) {
        case MINUS_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, NEGATE);
            break;
        }
        case BANG_TOKEN: {
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, NOT);
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
            INSERT_INSTRUCTIONS(compiler_context->instruction_buffer, LOAD_VAR, expression->offset);
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