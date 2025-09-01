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
#include "objects/function.h"

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

  struct Environment environment = compiler_init_environment();

  struct Value *constants = malloc(sizeof(struct Value) * CONST_POOL_SIZE);
  struct Function **functions = malloc(sizeof(struct Function *) * FUNCTION_POOL_SIZE);

  struct CompilerContext compiler_context = {
    .environment = &environment,
    .function = function_init()
  };

  compiler_resolve_statements(&compiler_context, parser->statement_vector);
  compiler_compile_statements(&compiler_context, parser->statement_vector);

  compiler_context.function->num_locals = compiler_context.environment->variable_count;

  struct Closure *main_closure = closure_init(compiler_context.function);

  struct VM *vm = vm_init(main_closure);

  INSERT_INSTRUCTIONS(compiler_context.function->instruction_buffer, HALT);

  lexer_free(lexer);
  parser_free(parser);
  compiler_free_environment(&environment);

  vm_exec(vm);

  free(constants);
  free(functions);
  vm_free(vm);
  free(shrub_file.contents);

  return 0;
}
