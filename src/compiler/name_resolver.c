#include <stddef.h>

#include "compiler/name_resolver.h"

#include "compiler/compiler.h"
#include "compiler/scope.h"
#include "compiler/environment.h"

#include "parser/statements.h"
#include "parser/statement_vector.h"

#include "error/error.h"
#include "error/error_types.h"

void compiler_resolve_expression(struct CompilerContext *compiler_context, struct Expression *expression) {
    switch (expression->type) {
        case LITERAL_EXPRESSION: {
            struct LiteralExpression *literal_expression = (struct LiteralExpression *)expression;
            if (literal_expression->literal_type == IDENTIFIER_LITERAL) {
                struct ResolveResult resolve_result =
                compiler_search_environment(compiler_context, literal_expression->identifier);

                if (resolve_result.kind == RESOLVE_NOT_FOUND) {
                    error_throw(NAME_ERROR, "Variable not defined");
                }
                literal_expression->resolve_result = resolve_result;
            }
            break;
        }
        case BINARY_EXPRESSION: {
            struct BinaryExpression *binary_expression = (struct BinaryExpression *)expression;
            compiler_resolve_expression(compiler_context, binary_expression->left);
            compiler_resolve_expression(compiler_context, binary_expression->right);
            break;
        }
        case ASSIGNMENT_EXPRESSION: {
            struct AssignmentExpression *assignment_expression = (struct AssignmentExpression *)expression;

            compiler_resolve_expression(compiler_context, assignment_expression->right);

            struct ResolveResult resolve_result = compiler_search_environment(
                 compiler_context,
                 assignment_expression->identifier_name
             );

            if (resolve_result.kind == RESOLVE_NOT_FOUND) {
                error_throw(NAME_ERROR, "Variable undefined");
            }
            assignment_expression->resolve_result = resolve_result;
            break;
        }
        case UNARY_EXPRESSION: {
            struct UnaryExpression *unary_expression = (struct UnaryExpression *)expression;
            compiler_resolve_expression(compiler_context, unary_expression->operand);
            break;
        }
        case CALL_EXPRESSION: {
            struct CallExpression *call_expression = (struct CallExpression *)expression;

            struct ResolveResult resolve_result = compiler_search_environment(
                compiler_context,
                call_expression->function_name
            );

            if (resolve_result.kind == RESOLVE_NOT_FOUND) {
                error_throw(NAME_ERROR, "Variable not defined");
            }

            for (size_t i = 0; i < call_expression->arguments_count; i++) {
                compiler_resolve_expression(compiler_context, call_expression->arguments[i]);
            }

            call_expression->resolve_result = resolve_result;
            break;
        }
    }
}

static void check_duplicate_declaration(const struct Environment *environment, const char *name) {
    if (compiler_search_scope(
        compiler_get_top_scope(environment),
        name
    )) {
        error_throw(NAME_ERROR, "Can only define variable once per scope");
    }
}

void compiler_resolve_statement(struct CompilerContext *compiler_context, struct Statement *statement) {
    switch (statement->type) {
        case PRINT_STATEMENT: {
            struct PrintStatement *print_statement = (struct PrintStatement *)statement;
            compiler_resolve_expression(compiler_context, print_statement->expression);
            break;
        }
        case BLOCK_STATEMENT: {
            struct BlockStatement *block_statement = (struct BlockStatement *)statement;
            compiler_push_scope(compiler_context->environment);
            compiler_resolve_statements(compiler_context, block_statement->statement_vector);
            compiler_pop_scope(compiler_context->environment);
            break;
        }
        case EXPRESSION_STATEMENT: {
            struct ExpressionStatement *expression_statement = (struct ExpressionStatement *)statement;
            compiler_resolve_expression(compiler_context, expression_statement->expression);
            break;
        }
        case LET_STATEMENT: {
            struct LetStatement *let_statement = (struct LetStatement *)statement;

            compiler_resolve_expression(compiler_context, let_statement->expression);

            check_duplicate_declaration(compiler_context->environment, let_statement->identifier_name);

            compiler_insert_environment(compiler_context->environment, let_statement->identifier_name);
            let_statement->offset = compiler_context->environment->variable_count++;

            break;
        }
        case IF_STATEMENT: {
            struct IfStatement *if_statement = (struct IfStatement *)statement;
            compiler_resolve_expression(compiler_context, if_statement->condition);
            compiler_resolve_statement(compiler_context, (struct Statement *)if_statement->then_block);
            if (if_statement->else_block) {
                compiler_resolve_statement(compiler_context, if_statement->else_block);
            }
            break;
        }
        case WHILE_STATEMENT: {
            struct WhileStatement *while_statement = (struct WhileStatement *)statement;
            compiler_resolve_expression(compiler_context, while_statement->condition);
            compiler_resolve_statement(compiler_context, (struct Statement *)while_statement->body);
            break;
        }
        case FUNCTION_STATEMENT: {
            struct FunctionStatement *function_statement = (struct FunctionStatement *)statement;

            check_duplicate_declaration(compiler_context->environment, function_statement->name);

            compiler_insert_environment(compiler_context->environment, function_statement->name);

            struct Environment new_environment = compiler_init_environment();

            compiler_insert_environment(&new_environment, function_statement->name);
            new_environment.variable_count++;

            for (size_t i = 0; i < function_statement->num_parameters; i++) {
                check_duplicate_declaration(&new_environment, function_statement->parameters[i]);
                compiler_insert_environment(&new_environment, function_statement->parameters[i]);
                new_environment.variable_count++;
            }

            struct Function *function = function_init();

            struct CompilerContext new_function_context = {
                .environment = &new_environment,
                .function = function,
                .previous = compiler_context
            };

            compiler_resolve_statements(&new_function_context, function_statement->body->statement_vector);
            function_statement->offset = compiler_context->environment->variable_count++;
            function_statement->function = function;
            function->num_locals = new_environment.variable_count - function_statement->num_parameters;
            function->num_args = function_statement->num_parameters;
            break;
        }
        case RETURN_STATEMENT: {
            struct ReturnStatement *return_statement = (struct ReturnStatement *)statement;
            compiler_resolve_expression(compiler_context, return_statement->expression);
        }
    }
}

void compiler_resolve_statements
(
    struct CompilerContext *compiler_context,
    struct StatementVector *statement_vector
)
{
    for (size_t i = 0; i < statement_vector->used; ++i) {
        struct Statement *statement = statement_vector->statements[i];
        compiler_resolve_statement(compiler_context, statement);
    }
}