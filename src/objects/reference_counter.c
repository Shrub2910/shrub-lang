#include "vm/values.h"
#include "objects/string.h"
#include "objects/function.h"

void object_release(const struct Value value) {
    switch (value.type) {
        case TYPE_STRING: string_release(value.string); break;
        case TYPE_FUNCTION: function_release(value.function); break;
        default: break;
    }
}

void object_retain(const struct Value value) {
    switch (value.type) {
        case TYPE_STRING: string_retain(value.string); break;
        case TYPE_FUNCTION: function_retain(value.function); break;
        default: break;
    }
}