#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#include "vm/values.h"

void object_release(struct Value value);
void object_retain(struct Value value);

#endif