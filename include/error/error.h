#ifndef ERROR_H
#define ERROR_H
#include <stdnoreturn.h>
#include "error/error_types.h"

noreturn void error_throw(enum ErrorType error, const char *error_message);

#endif
