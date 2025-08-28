#ifndef STRING_H
#define STRING_H

#include <stddef.h>

struct String {
  char *buffer;
  size_t length;
  size_t references;
};

struct String *string_init(const char *buffer, size_t length);
void string_retain(struct String *string);
void string_release(struct String *string);
void string_free(struct String *string);

#endif
