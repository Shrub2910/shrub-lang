#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdnoreturn.h>
#include "error/error.h"
#include "error/error_types.h"

// Associates the error enum with an error message 
// Joins it with an additional message that could be passed in by any part of the program
// Example: Stack Error: Stack Overflow
noreturn void error_throw(const enum ErrorType error, const char *error_message) {
  char buffer[100] = "";
  switch (error) {
    case TYPE_ERROR:
      strcat(buffer, "Type Error: ");
      break;
    case STACK_ERROR:
      strcat(buffer, "Stack Error: ");
      break;
    case MALLOC_ERROR:
      strcat(buffer, "Malloc Error: ");
      break;
    case INDEX_ERROR:
      strcat(buffer, "Index Error: ");
      break;
    case INSTRUCTION_ERROR:
      strcat(buffer, "Instruction Error: ");
      break;
    case LEXICAL_ERROR:
      strcat(buffer, "Lexical Error: ");
    case SYNTAX_ERROR:
      strcat(buffer, "Syntax Error: ");
    case HASH_TABLE_ERROR:
      strcat(buffer, "Hash Table Error: ");
    default:
      strcat(buffer, "Unknown Error: ");
      break;
  }

  strcat(buffer, error_message);

  printf("%s\n", buffer);
  
  // 1 means failure
  // Program can't continue after an error, program must be exited from
  exit(1);
}
