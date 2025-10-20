// This file is just for testing. When grading, we will not
// use main.c, instead, we will only use vector.c and vector.h
// with our own driver code.

// Use the navigation "files" feature on the left of this
// editor to change files.
#include <stdio.h>
#include <string.h>

#include "vector.h"

int main(void)
{
    Vector *v;
    char cmd[33];
    size_t idx;
    int64_t value;

    v = vector_new();
    do {
        printf("Enter command: ");
        if (scanf("%32s", cmd) <= 0) {
            putchar('\n');
            break;
        }
        else if (!strcmp(cmd, "quit")) {
            break;
        }
        else if (!strcmp(cmd, "size")) {
            printf("%lu\n", vector_size(v));
        }
        else if (!strcmp(cmd, "cap")) {
            printf("%lu\n", vector_capacity(v));
        }
        else if (!strcmp(cmd, "get")) {
            if (scanf("%lu", &idx) != 1 || !vector_get(v, idx, &value)) {
                printf("Invalid index.\n");
            }
            else {
                printf("[%lu] -> %ld\n", idx, value);
            }
        }
        else if (!strcmp(cmd, "set")) {
            if (scanf("%lu %ld", &idx, &value) != 2 || !vector_set(v, idx, value)) {
                printf("Invalid index or value.\n");
            }
        }
        else if (!strcmp(cmd, "sort")) {
            vector_sort(v);
        }
        else if (!strcmp(cmd, "find")) {
            if (scanf("%ld", &value) != 1) {
                printf("Invalid value.\n");
            }
            ssize_t ret = vector_find(v, value);
            if (ret < 0) {
                printf("Value %ld not found.\n", value);
            }
            else {
                printf("Value %ld found at index %ld\n", value, ret);
            }
        }
        else if (!strcmp(cmd, "bsearch")) {
            if (scanf("%ld", &value) != 1) {
                printf("Invalid value.\n");
            }
            ssize_t ret = vector_bsearch(v, value);
            if (ret < 0) {
                printf("Value %ld not found.\n", value);
            }
            else {
                printf("Value %ld found at index %ld\n", value, ret);
            }
        }
        else if (!strcmp(cmd, "push")) {
            if (scanf("%ld", &value) != 1) {
                printf("Invalid value to push.\n");
            }
            else {
                vector_push(v, value);
            }
        }
        else if (!strcmp(cmd, "insert")) {
            if (scanf("%lu %ld", &idx, &value) != 2) {
                printf("Invalid index or value to insert.\n");
            }
            else {
                vector_insert(v, idx, value);
            }
        }
        else if (!strcmp(cmd, "clear")) {
            vector_clear(v);
        }
        else if (!strcmp(cmd, "remove")) {
            if (scanf("%lu", &idx) != 1) {
                printf("Invalid index specified.\n");
            }
            else if (!vector_remove(v, idx)) {
                printf("Unable to remove index %lu.\n", idx);
            }
        }
        else if (!strcmp(cmd, "resize")) {
            if (scanf("%lu", &idx) != 1) {
                printf("Invalid size.\n");
            }
            else {
                vector_resize(v, idx);
            }
        }
        else if (!strcmp(cmd, "print") || !strcmp(cmd, "list")) {
            for (idx = 0; idx < vector_size(v); idx++) {
                vector_get(v, idx, &value);
                printf("[%lu] = %ld\n", idx, value);
            }
        }
        else if (!strcmp(cmd, "shrink")) {
            vector_shrink(v);
        }
        else {
            printf("Unknown command '%s'\n", cmd);
        }
    } while (1);

    vector_free(v);

    return 0;
}
