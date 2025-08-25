#include <stdio.h>
#include <stdlib.h>
#include "error/error.h"
#include "error/error_types.h"
#include "file/file_read.h"

#define CHUNK 1024

struct ShrubFile file_read(const char *file_name) {
    FILE *file = fopen(file_name, "r");

    if (!file) {
        error_throw(ARGUMENT_ERROR, "File provided doesn't exist");
    }

    fseek(file, 0L, SEEK_END);
    const long file_size = ftell(file);
    rewind(file);

    char *buffer = malloc(file_size * sizeof(char) + 1);
    buffer[file_size] = '\0';

    int current_char;
    size_t index = 0;

    while ((current_char = fgetc(file)) != EOF) {
        buffer[index++] = (char)current_char;
    }

    buffer[index] = '\0';
    fclose(file);

    struct ShrubFile shrub_file;
    shrub_file.contents = buffer;
    shrub_file.size = index;

    return shrub_file;
}