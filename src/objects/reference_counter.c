#include "vm/values.h"
#include "objects/string.h"
#include "objects/function.h"

void object_release(const struct Value value) {
    switch (value.type) {
        case TYPE_STRING: string_release(value.string); break;
        case TYPE_CLOSURE: closure_release(value.closure); break;
        default: break;
    }
}

void object_retain(const struct Value value) {
    switch (value.type) {
        case TYPE_STRING: string_retain(value.string); break;
        case TYPE_CLOSURE: closure_retain(value.closure); break;
        default: break;
    }
}