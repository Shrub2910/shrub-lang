#include <stdlib.h>
#include <stdbool.h>
#include "parser/parser.h"

#include "error/error.h"
#include "parser/statement_vector.h"

#include "parser/statements.h"
#include "parser/expressions.h"
#include "parser/token.h"

static struct Statement *parser_statement(struct Parser *parser);
static struct ExpressionStatement *parser_expression_statement(struct Parser *parser);
static struct PrintStatement *parser_print_statement(struct Parser *parser);
static struct BlockStatement *parser_block_statement(struct Parser *parser);
static struct LetStatement *parser_let_statement(struct Parser *parser);
static struct Token parser_previous(const struct Parser *parser);
static void parser_consume(struct Parser *parser, enum TokenType type, const char *error_message);
static bool parser_match(struct Parser *parser, const enum TokenType *types, size_t amount);
static struct Token parser_peek(const struct Parser *parser);
static struct Expression *parser_expression(struct Parser *parser);
static struct Expression *parser_comparison(struct Parser *parser);
static struct Expression *parser_assignment(struct Parser *parser);
static struct Expression *parser_add(struct Parser *parser);
static struct Expression *parser_multiply(struct Parser *parser);
static struct Expression *parser_unary(struct Parser *parser);
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

    if (parser_match(parser, (enum TokenType[]) {LET_TOKEN}, 1)) {
        return (struct Statement *)parser_let_statement(parser);
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

static struct LetStatement *parser_let_statement(struct Parser *parser) {
    struct LetStatement *let_statement = malloc(sizeof(struct LetStatement));

    if (!let_statement) {
        error_throw(MALLOC_ERROR, "Failed to allocate memory for let_statement");
    }

    parser_consume(parser, IDENTIFIER_TOKEN, "Expected identifier");
    char *identifier_name = parser_previous(parser).string;

    if (!parser_match(parser, (enum TokenType[]){EQUAL_TOKEN}, 1)) {
        parser_consume(parser, SEMI_COLON_TOKEN, "Expected ;");

        let_statement->statement.type = LET_STATEMENT;
        let_statement->identifier_name = identifier_name;
        let_statement->is_nil = true;
        return let_statement;
    }

    struct Expression *expression = parser_expression(parser);
    parser_consume(parser, SEMI_COLON_TOKEN, "Expected ;");

    let_statement->statement.type = LET_STATEMENT;
    let_statement->expression = expression;
    let_statement->identifier_name = identifier_name;

    return let_statement;
}

static struct Expression *parser_expression(struct Parser *parser) {
    return parser_assignment(parser);
}

static struct Expression *parser_assignment(struct Parser *parser) {
    if (parser_match(parser, (enum TokenType[]){IDENTIFIER_TOKEN}, 1)) {
        char *identifier_name = parser_previous(parser).string;
        if (parser_match(parser, (enum TokenType[]){EQUAL_TOKEN}, 1)) {
            struct AssignmentExpression *assignment_expression =
                malloc(sizeof(struct AssignmentExpression));

            if (!assignment_expression) {
                error_throw(MALLOC_ERROR, "Failed to allocate memory for assignment_expression");
            }

            assignment_expression->expression.type = ASSIGNMENT_EXPRESSION;
            assignment_expression->identifier_name = identifier_name;
            assignment_expression->right = parser_assignment(parser);

            return (struct Expression *)assignment_expression;
        }

        parser->current_token--;
        parser->index--;
    }

    return parser_comparison(parser);
}

static struct Expression *parser_comparison(struct Parser *parser) {
    struct Expression *left = parser_add(parser);

    while (parser_match(parser, (enum TokenType[]){
        DOUBLE_EQUAL_TOKEN,
        NOT_EQUAL_TOKEN,
        LESS_EQUAL_TOKEN,
        GREATER_EQUAL_TOKEN,
        LESS_TOKEN,
        GREATER_TOKEN
    }, 6)) {
        const enum TokenType operator = parser_previous(parser).type;
        struct Expression *right = parser_add(parser);
        struct BinaryExpression *new_left = malloc(sizeof(struct BinaryExpression));

        if (!new_left) {
            error_throw(MALLOC_ERROR, "Failed to allocate memory for new_left");
        }

        new_left->expression.type = BINARY_EXPRESSION;
        new_left->left = left;
        new_left->right = right;
        new_left->operator = operator;

        left = (struct Expression *)new_left;
    }

    return left;
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
    struct Expression *left = parser_unary(parser);

    while (parser_match(parser, (enum TokenType[]){TIMES_TOKEN, DIVIDE_TOKEN}, 2)) {
        const enum TokenType operator = parser_previous(parser).type;
        struct Expression *right = parser_unary(parser);
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

static struct Expression *parser_unary(struct Parser *parser) {
    if (parser_match(parser, (enum TokenType[]){MINUS_TOKEN, BANG_TOKEN}, 2)) {
        const enum TokenType operator = parser_previous(parser).type;
        struct Expression *operand = parser_unary(parser);
        struct UnaryExpression *unary_expression = malloc(sizeof(struct UnaryExpression));

        if (!unary_expression) {
            error_throw(MALLOC_ERROR, "Failed to allocate memory for unary_expression");
        }

        unary_expression->expression.type = UNARY_EXPRESSION;
        unary_expression->operator = operator;
        unary_expression->operand = operand;

        return (struct Expression *)unary_expression;
    }

    return parser_literal(parser);
}

static struct Expression *parser_literal(struct Parser *parser) {
    struct LiteralExpression *literal_expression = malloc(sizeof(struct LiteralExpression));;

    if (!literal_expression) {
        error_throw(MALLOC_ERROR, "Failed to allocate new expression");
    }

    literal_expression->expression.type = LITERAL_EXPRESSION;

    if (parser_match(parser, (enum TokenType[]){NUMBER_TOKEN}, 1)) {
        const double value = parser_previous(parser).number;
        literal_expression->literal_type = NUMBER_LITERAL;
        literal_expression->number = value;

        return (struct Expression *)literal_expression;
    }

    if (parser_match(parser, (enum TokenType[]){IDENTIFIER_TOKEN}, 1)) {
        char *value = parser_previous(parser).string;
        literal_expression->literal_type = IDENTIFIER_LITERAL;
        literal_expression->identifier = value;

        return (struct Expression *)literal_expression;
    }

    if (parser_match(parser, (enum TokenType[]){STRING_TOKEN}, 1)) {
        char *value = parser_previous(parser).string;
        literal_expression->literal_type = STRING_LITERAL;
        literal_expression->string = value;

        return (struct Expression *)literal_expression;
    }

    if (parser_match(parser, (enum TokenType[]){BOOLEAN_TOKEN}, 1)) {
        const bool value = parser_previous(parser).boolean;
        literal_expression->literal_type = BOOLEAN_LITERAL;
        literal_expression->boolean = value;

        return (struct Expression *)literal_expression;
    }

    if (parser_match(parser, (enum TokenType[]){NIL_TOKEN}, 1)) {
        literal_expression->literal_type = NIL_LITERAL;

        return (struct Expression *)literal_expression;
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
