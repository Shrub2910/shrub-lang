#include "parser/statement_vector.h"
#include "parser/statements.h"
#include "parser/expressions.h"

#include <stdlib.h>

#include "error/error.h"
#include "error/error_types.h"

static void parser_statement_free(struct Statement *statement);
static void parser_expression_free(struct Expression *expression);

struct StatementVector *parser_statement_vector_init(void) {
    struct StatementVector *statement_vector = malloc(sizeof(struct StatementVector));

    if (!statement_vector) {
        error_throw(MALLOC_ERROR, "Failed to allocate statement vector");
    }

    statement_vector->used = 0;
    statement_vector->size = STATEMENT_VECTOR_INITIAL_SIZE;

    statement_vector->statements = malloc(STATEMENT_VECTOR_INITIAL_SIZE * sizeof(struct Statement *));

    if (!statement_vector->statements) {
        free(statement_vector);
        error_throw(MALLOC_ERROR, "Failed to allocate statement vector");
    }

    return statement_vector;
}

void parser_statement_vector_insert(struct StatementVector *statement_vector, struct Statement *statement) {
    if (statement_vector->used == statement_vector->size) {
        statement_vector->size *= 2;
        struct Statement **statements = realloc(statement_vector->statements, sizeof(struct Statement*) * statement_vector->size);

        if (!statements) {
            error_throw(MALLOC_ERROR, "Failed to reallocate statement vector");
        }

        statement_vector->statements = statements;
    }

    statement_vector->statements[statement_vector->used++] = statement;
}

void parser_statement_vector_free(struct StatementVector *statement_vector) {
    for (size_t i = 0; i < statement_vector->used; i++) {
        parser_statement_free(statement_vector->statements[i]);
    }
    free(statement_vector->statements);
    statement_vector->statements = NULL;

    statement_vector->used = statement_vector->size = 0;

    free(statement_vector);
}

static void parser_statement_free(struct Statement *statement) {
    switch (statement->type) {
        case PRINT_STATEMENT: {
            struct PrintStatement *print_statement = (struct PrintStatement *)statement;
            parser_expression_free(print_statement->expression);
            free(print_statement);
            break;
        }
        case BLOCK_STATEMENT: {
            struct BlockStatement *block_statement = (struct BlockStatement *)statement;
            parser_statement_vector_free(block_statement->statement_vector);
            free(block_statement);
            break;
        }
        case EXPRESSION_STATEMENT: {
            struct ExpressionStatement *expression_statement = (struct ExpressionStatement *)statement;
            parser_expression_free(expression_statement->expression);
            free(expression_statement);
            break;
        }
        case LET_STATEMENT: {
            struct LetStatement *let_statement = (struct LetStatement *)statement;
            parser_expression_free(let_statement->expression);
            free(let_statement);
            break;
        }
        case IF_STATEMENT: {
            struct IfStatement *if_statement = (struct IfStatement *)statement;
            parser_expression_free(if_statement->condition);
            parser_statement_free((struct Statement *)if_statement->then_block);
            if (if_statement->else_block) {
                parser_statement_free((struct Statement *)if_statement->else_block);
            }
            free(if_statement);
            break;
        }
        case WHILE_STATEMENT: {
            struct WhileStatement *while_statement = (struct WhileStatement *)statement;
            parser_expression_free(while_statement->condition);
            parser_statement_free((struct Statement *)while_statement->body);
            free(while_statement);
            break;
        }
        case FUNCTION_STATEMENT: {
            struct FunctionStatement *function_statement = (struct FunctionStatement *)statement;
            parser_statement_free((struct Statement *)function_statement->body);
            free(function_statement);
            break;
        }
        case RETURN_STATEMENT: {
            struct ReturnStatement *return_statement = (struct ReturnStatement *)statement;
            parser_expression_free(return_statement->expression);
            free(return_statement);
        }
    }
}

static void parser_expression_free(struct Expression *expression) {
    switch (expression->type) {
        case BINARY_EXPRESSION: {
            struct BinaryExpression *binary_expression = (struct BinaryExpression *)expression;
            parser_expression_free(binary_expression->left);
            parser_expression_free(binary_expression->right);
            free(binary_expression);
            break;
        }
        case LITERAL_EXPRESSION: {
            struct LiteralExpression *literal_expression = (struct LiteralExpression *)expression;
            free(literal_expression);
            break;
        }
        case ASSIGNMENT_EXPRESSION: {
            struct AssignmentExpression *assignment_expression = (struct AssignmentExpression *)expression;
            parser_expression_free(assignment_expression->right);
            free(assignment_expression);
            break;
        }
        case UNARY_EXPRESSION: {
            struct UnaryExpression *unary_expression = (struct UnaryExpression *)expression;
            parser_expression_free(unary_expression->operand);
            free(unary_expression);
            break;
        }
        case CALL_EXPRESSION: {
            struct CallExpression *call_expression = (struct CallExpression *)expression;
            for (size_t i = 0; i < call_expression->arguments_count; i++) {
                parser_expression_free(call_expression->arguments[i]);
            }
            free(call_expression);
            break;
        }
    }
}
