#pragma once

#include <stddef.h>

struct string {
    char *buffer;    // memory buffer for the string
    size_t length;   // total number of characters (excluding '\0')
    size_t capacity; // total amount of memory allocated for `buffer`
};

// Allocate/Dellocate
struct string *string_new(void);
struct string *string_new_from(const char *cstr);
void string_realloc(struct string *s, size_t capacity);
void string_free(struct string *s);

// Utilities
void string_push(struct string *s, char c);
void string_append(struct string *s, const char *cstr);
void string_prepend(struct string *s, const char *cstr);
void string_clear(struct string *s);
void string_exactly(struct string *s, const char *cstr);


