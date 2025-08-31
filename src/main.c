#include <stdlib.h>

#include "parser/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"
#include "compiler/compiler.h"
#include "compiler/environment.h"
#include "compiler/name_resolver.h"
#include "utils/print_statements.h"
#include "error/error.h"
#include "error/error_types.h"
#include "file/file_read.h"
#include "vm/opcodes.h"

#define SHOW_AST

int main(const int argc, char **argv) {
  if (argc != 2) {
    error_throw(ARGUMENT_ERROR, "Expects one 1 argument (File Path)");
  }

  const struct ShrubFile shrub_file = file_read(argv[1]);

  struct Lexer *lexer = lexer_init(shrub_file.contents, shrub_file.size);
  lexer_tokenize(lexer);

  struct Parser *parser = parser_init(lexer->token_vector);
  parser_parse(parser);

#ifdef SHOW_AST
  print_statements(parser->statement_vector, 0);
#endif

  struct VM *vm = vm_init();

  struct Environment environment = compiler_init_environment();

  struct CompilerContext compiler_context = {
    .vm = vm,
    .instruction_buffer = vm->instruction_buffer,
    .environment = &environment
  };

  compiler_resolve_statements(&compiler_context, parser->statement_vector);
  compiler_compile_statements(&compiler_context, parser->statement_vector);

  INSERT_INSTRUCTIONS(compiler_context.instruction_buffer, HALT);

  lexer_free(lexer);
  parser_free(parser);
  compiler_free_environment(&environment);

  vm_exec(vm);

  vm_free(vm);
  free(shrub_file.contents);

  return 0;
}
