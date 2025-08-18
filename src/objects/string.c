#include <string.h>
#include <stdlib.h>
#include "objects/string.h"
#include "error/error.h"
#include "error/error_types.h"

// Initialises string object copying the contents of the buffer provided into the string object 
// Adds a null byte to the string so it is known where it terminates
struct String *string_init(const char *buffer, const size_t length) {
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

// Insures the string object will continue to live
void string_retain(struct String *string) {
  string->references++;
}

// Insures the string object will be freed if necessary 
void string_release(struct String *string) {
  if (--(string->references) == 0) {
    string_free(string);
  }
}

// Clean up
void string_free(struct String *string) {
  free(string->buffer);
  string->buffer = NULL;

  string->length = string->references = 0;

  free(string);
}



