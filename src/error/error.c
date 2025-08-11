#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error/error_types.h"

void error_throw(enum ErrorType error, char *error_message) {
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
  }

  strcat(buffer, error_message);

  printf("%s\n", buffer);
  exit(1);
}
