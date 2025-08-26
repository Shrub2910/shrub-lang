#include <stdlib.h>
#include "parser/parser.h"

#include "error/error.h"
#include "parser/statement_vector.h"
#include "stdbool.h"

#include "parser/statements.h"
#include "parser/expressions.h"
#include "parser/token.h"

static struct Statement *parser_statement(struct Parser *parser);
static struct ExpressionStatement *parser_expression_statement(struct Parser *parser);
static struct PrintStatement *parser_print_statement(struct Parser *parser);
static struct BlockStatement *parser_block_statement(struct Parser *parser);
static struct Token parser_previous(const struct Parser *parser);
static void parser_consume(struct Parser *parser, enum TokenType type, const char *error_message);
static bool parser_match(struct Parser *parser, const enum TokenType *types, size_t amount);
static struct Token parser_peek(const struct Parser *parser);
static struct Expression *parser_expression(struct Parser *parser);
static struct Expression *parser_add(struct Parser *parser);
static struct Expression *parser_multiply(struct Parser *parser);
static struct Expression *parser_literal(struct Parser *parser);

struct Parser *parser_init(struct TokenVector *token_vector) {
    struct Parser *parser = malloc(sizeof(struct Parser));

    if (!parser) {
        error_throw(MALLOC_ERROR, "Failed to allocate parser");
    }

    parser->token_vector = token_vector;

    parser->current_token = parser->token_vector->tokens;
    parser->index = 0;

    parser->statement_vector = parser_statement_vector_init();

    if (!parser->statement_vector) {
        free(parser);
        error_throw(MALLOC_ERROR, "Failed to allocate parser");
    }

    return parser;
}

void parser_parse(struct Parser *parser) {
    while (parser->index < parser->token_vector->used) {
        parser_statement_vector_insert(parser->statement_vector, parser_statement(parser));
    }
}

static struct Statement *parser_statement(struct Parser *parser) {
    if (parser_match(parser, (enum TokenType[]) {PRINT_TOKEN}, 1)) {
        return (struct Statement *)parser_print_statement(parser);
    }

    if (parser_match(parser, (enum TokenType[]) {DO_TOKEN}, 1)) {
        return (struct Statement *)parser_block_statement(parser);
    }

    return (struct Statement *)parser_expression_statement(parser);
}

static struct ExpressionStatement *parser_expression_statement(struct Parser *parser) {
    struct Expression *expression = parser_expression(parser);
    parser_consume(parser, SEMI_COLON_TOKEN, "Expected ;");

    struct ExpressionStatement *expression_statement = malloc(sizeof(struct ExpressionStatement));

    if (!expression_statement) {
        error_throw(MALLOC_ERROR, "Failed to allocate memory for expression_statement");
    }

    expression_statement->statement.type = EXPRESSION_STATEMENT;
    expression_statement->expression = expression;
    return expression_statement;
}

static struct PrintStatement *parser_print_statement(struct Parser *parser) {
    struct Expression *expression = parser_expression(parser);

    parser_consume(parser, SEMI_COLON_TOKEN, "Expected ;");

    struct PrintStatement *print_statement = malloc(sizeof(struct PrintStatement));

    if (!print_statement) {
        error_throw(MALLOC_ERROR, "Failed to allocate memory for print_statement");
    }

    print_statement->statement.type = PRINT_STATEMENT;
    print_statement->expression = expression;

    return print_statement;
}

static struct BlockStatement *parser_block_statement(struct Parser *parser) {
    struct StatementVector *statement_vector = parser_statement_vector_init();
    while (parser->index < parser->token_vector->used
        && parser_peek(parser).type != END_TOKEN) {
        parser_statement_vector_insert(statement_vector, parser_statement(parser));
    }
    parser_consume(parser, END_TOKEN, "Expected end");

    struct BlockStatement *block_statement = malloc(sizeof(struct BlockStatement));

    if (!block_statement) {
        error_throw(MALLOC_ERROR, "Failed to allocate memory for block_statement");
    }

    block_statement->statement.type = BLOCK_STATEMENT;
    block_statement->statement_vector = statement_vector;

    return block_statement;
}

static struct Expression *parser_expression(struct Parser *parser) {
    return parser_add(parser);
}

static struct Expression *parser_add(struct Parser *parser) {
    struct Expression *left = parser_multiply(parser);

    while (parser_match(parser, (enum TokenType[]){PLUS_TOKEN, MINUS_TOKEN}, 2)) {
        const enum TokenType operator = parser_previous(parser).type;
        struct Expression *right = parser_multiply(parser);
        struct BinaryExpression *new_left = malloc(sizeof(struct BinaryExpression));

        if (!new_left) {
            error_throw(MALLOC_ERROR, "Failed to allocate new left expression");
        }

        new_left->expression.type = BINARY_EXPRESSION;
        new_left->left = left;
        new_left->right = right;
        new_left->operator = operator;

        left = (struct Expression *)new_left;
    }

    return left;
}

static struct Expression *parser_multiply(struct Parser *parser) {
    struct Expression *left = parser_literal(parser);

    while (parser_match(parser, (enum TokenType[]){TIMES_TOKEN, DIVIDE_TOKEN}, 2)) {
        const enum TokenType operator = parser_previous(parser).type;
        struct Expression *right = parser_literal(parser);
        struct BinaryExpression *new_left = malloc(sizeof(struct BinaryExpression));

        if (!new_left) {
            error_throw(MALLOC_ERROR, "Failed to allocate new left expression");
        }

        new_left->expression.type = BINARY_EXPRESSION;
        new_left->left = left;
        new_left->right = right;
        new_left->operator = operator;

        left = (struct Expression *)new_left;
    }

    return left;
}

static struct Expression *parser_literal(struct Parser *parser) {
    if (parser_match(parser, (enum TokenType[]){NUMBER_TOKEN}, 1)) {
        struct LiteralExpression *expression = malloc(sizeof(struct LiteralExpression));

        if (!expression) {
            error_throw(MALLOC_ERROR, "Failed to allocate new expression");
        }

        const double value = parser_previous(parser).number;
        expression->expression.type = LITERAL_EXPRESSION;
        expression->value = value;

        return (struct Expression *)expression;
    }

    if (parser_match(parser, (enum TokenType[]){L_BRACKET_TOKEN}, 1)) {
        struct Expression *expression = parser_expression(parser);
        parser_consume(parser, R_BRACKET_TOKEN, "Expected end bracket");

        return expression;
    }

    error_throw(SYNTAX_ERROR, "Expected Expression");
}

static struct Token parser_peek(const struct Parser *parser) {
    if (parser->index < parser->token_vector->used) {
        return *(parser->current_token);
    }
    return (struct Token) {.type = NOTHING_TOKEN};
}

static struct Token parser_previous(const struct Parser *parser) {
    return *(parser->current_token - 1);
}

static void parser_consume(struct Parser *parser, const enum TokenType type, const char *error_message) {
    if (parser->index >= parser->token_vector->used) {
        error_throw(SYNTAX_ERROR, error_message);
    }

    if (parser->current_token->type == type) {
        parser->current_token++;
        parser->index++;
        return;
    }

    error_throw(SYNTAX_ERROR, error_message);
}

static bool parser_match(struct Parser *parser, const enum TokenType *types, const size_t amount) {
    if (parser->index >= parser->token_vector->used) {
        return false;
    }

    const struct Token token = parser_peek(parser);

    for (size_t i = 0; i < amount; i++) {
        if (token.type == types[i] ) {
            parser_consume(parser, types[i], "");
            return true;
        }
    }

    return false;
}

void parser_free(struct Parser *parser) {
    parser_statement_vector_free(parser->statement_vector);
    parser->statement_vector = NULL;
    parser->index = 0;
    parser->token_vector = NULL;
    parser->current_token = NULL;

    free(parser);
}
