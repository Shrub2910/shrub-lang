#ifndef FILE_READ_H
#define FILE_READ_H

struct ShrubFile {
    char *contents;
    size_t size;
};

struct ShrubFile file_read(const char *file_name);

#endif
