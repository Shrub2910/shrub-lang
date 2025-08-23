#include <stdio.h>

#include "parser/lexer.h"
#include "parser/parser.h"
#include "utils/print_token.h"
#include "utils/print_statements.h"

int main(void) {
  char input[] = "? (1 + 2) * (3 - 4 / (5 + 6)) + 7;";
  struct Lexer *lexer = lexer_init(input, sizeof(input) - 1);
  lexer_tokenize(lexer);

  struct Parser *parser = parser_init(lexer->token_vector);
  parser_parse(parser);

  printf("TOKENS:\n\n");
  print_tokens(lexer->token_vector);
  printf("\nAST:\n\n");
  print_statements(parser->statement_vector);

  lexer_free(lexer);
  parser_free(parser);

  return 0;
}
