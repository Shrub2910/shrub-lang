#include "parser/lexer.h"
#include "utils/print_token.h"

int main(void) {
  struct Lexer *lexer = lexer_init("2 + 3 * 8 / 1.203", 17);
  lexer_tokenize(lexer);
  print_tokens(lexer->token_vector);
  lexer_free(lexer);

  return 0;
}
