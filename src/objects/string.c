#include <string.h>
#include <stdlib.h>
#include "objects/string.h"
#include "error/error.h"
#include "error/error_types.h"

struct String *string_init(const char *buffer, size_t length) {
  struct String *string = malloc(sizeof(struct String));

  if (!string) {
    error_throw(MALLOC_ERROR, "Failed to allocate string object");
  }

  string->length = length;
  string->references = 1;
  
  string->buffer = malloc(string->length + 1);
  if (!string->buffer) {
    free(string);
    error_throw(MALLOC_ERROR, "Failed to allocate string buffer");
  }

  memcpy(string->buffer, buffer, string->length);
  string->buffer[string->length] = '\0';
  
  return string;
}

void string_retain(struct String *string) {
  string->references++;
}

void string_release(struct String *string) {
  if (--(string->references) == 0) {
    string_free(string);
  }
}

void string_free(struct String *string) {
  free(string->buffer);
  string->buffer = NULL;

  string->length = string->references = 0;

  free(string);
}



