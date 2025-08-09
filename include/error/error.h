#include "error/error_types.h"

#ifndef ERROR_H
#define ERROR_H

void error_throw(enum ErrorType error, char *error_message);

#endif
