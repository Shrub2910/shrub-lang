#include <stdlib.h>

#include "parser/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"
#include "compiler/compiler.h"
#include "utils/print_statements.h"
#include "error/error.h"
#include "error/error_types.h"
#include "file/file_read.h"

int main(const int argc, char **argv) {
  if (argc != 2) {
    error_throw(ARGUMENT_ERROR, "Expects one 1 argument (File Path)");
  }

  const struct ShrubFile shrub_file = file_read(argv[1]);

  struct Lexer *lexer = lexer_init(shrub_file.contents, shrub_file.size);
  lexer_tokenize(lexer);

  struct Parser *parser = parser_init(lexer->token_vector);
  parser_parse(parser);

  print_statements(parser->statement_vector, 0);

  struct VM *vm = vm_init();

  struct CompilerContext compiler_context = {.vm = vm, .instruction_buffer = vm->instruction_buffer};
  compiler_compile_statements(&compiler_context, parser->statement_vector);

  vm_exec(vm);

  lexer_free(lexer);
  parser_free(parser);
  vm_free(vm);
  free(shrub_file.contents);

  return 0;
}
