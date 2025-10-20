#pragma once
#include <stddef.h> // size_t

enum ItemType {
    File,
    Directory
};

struct List {
    enum ItemType type;
    char *name;
    union {
        size_t size; // for File
        struct List *contents; // for Directory
    };
    struct List *next;
};

struct List *build_tree(const char starting_directory[], size_t max_depth);
void free_tree(struct List *tree);

size_t disk_usage(const struct List *tree);
size_t file_size(const struct List *item);
