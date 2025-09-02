#include "vm/variables/upvalues.h"

#include <stdlib.h>

#include "objects/function.h"
#include "objects/reference_counter.h"

static void close_upvalue(struct Upvalue *upvalue) {
    struct Value *new_v = malloc(sizeof(struct Value));
    *new_v = *upvalue->v;
    object_retain(*new_v);
    upvalue->v = new_v;
}

void close_upvalues(struct Upvalue *upvalue) {
    while (upvalue) {
        close_upvalue(upvalue);
        upvalue = upvalue->next;
    }
}

