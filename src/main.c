#include <stdio.h>

#include "parser/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"
#include "compiler/compiler.h"
#include "utils/print_statements.h"

int main(void) {
  char input[] = "print (2 + 3) * (4 - 1) + 5 * (6 / 2) - 7;";

  struct Lexer *lexer = lexer_init(input, sizeof(input) - 1);
  lexer_tokenize(lexer);

  struct Parser *parser = parser_init(lexer->token_vector);
  parser_parse(parser);

  print_statements(parser->statement_vector);

  struct VM *vm = vm_init();
  struct Compiler compiler = compiler_init(parser->statement_vector, vm);

  compiler_compile_statements(&compiler);

  vm_exec(vm);

  lexer_free(lexer);
  parser_free(parser);
  vm_free(vm);

  return 0;
}
