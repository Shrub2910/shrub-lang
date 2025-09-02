#ifndef UPVALUES_H
#define UPVALUES_H

#include <stdbool.h>
#include <stddef.h>
#include "vm/values.h"

struct Upvalue {
    struct Value *v;
    size_t reference_count;
    struct Upvalue *next;
};

struct UpvalueDesc {
    size_t index;
    bool is_local;
};

void close_upvalues(struct Upvalue *upvalue);

#define UPVALUE_POOL_SIZE 255


#endif