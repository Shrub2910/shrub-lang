#include <stdio.h>
#include <stdlib.h>

#include "parser/statement_vector.h"

#include "parser/expressions.h"
#include "parser/statements.h"

#include "parser/token.h"

#include "utils/print_statements.h"

#include "error/error.h"

static char *make_indent(const size_t count) {
    char *s = malloc(count + 1);
    if (!s)
        error_throw(MALLOC_ERROR, "Could not allocate memory for indent.");

    for (int i = 0; i < count; i++)
        s[i] = '\t';

    s[count] = '\0';
    return s;
}

static void print_expression(struct Expression *expression, size_t indent_count) {
    if (expression->type == BINARY_EXPRESSION){
        char *indent = make_indent(indent_count);
        const struct BinaryExpression *binary_expression = (struct BinaryExpression *)expression;

        switch (binary_expression->operator) {
            case PLUS_TOKEN: printf("%sBin(+)\n", indent); break;
            case MINUS_TOKEN: printf("%sBin(-)\n", indent); break;
            case TIMES_TOKEN: printf("%sBin(*)\n", indent); break;
            case DIVIDE_TOKEN: printf("%sBin(/)\n", indent); break;
            default: break;
        }

        print_expression(binary_expression->left, indent_count + 1);
        print_expression(binary_expression->right, indent_count + 1);
        free(indent);
        return;
    }

    char *indent = make_indent(indent_count);
    const struct LiteralExpression *literal_expression = (struct LiteralExpression *)expression;
    printf("%sNumber(%lf)\n", indent, literal_expression->value);
    free(indent);
}

static void print_statement(struct Statement *statement) {
    if (statement->type == PRINT_STATEMENT) {
        const struct PrintStatement *print_statement = (struct PrintStatement *)statement;
        printf("Print\n");

        print_expression(print_statement->expression, 1);
        return;
    }

    const struct ExpressionStatement *expression_statement = (struct ExpressionStatement *)statement;
    printf("Statement\n");

    print_expression(expression_statement->expression, 1);
}

void print_statements(const struct StatementVector *statement_vector) {
    for (size_t i = 0; i < statement_vector->used; i++) {
        print_statement(statement_vector->statements[i]);
    }
}