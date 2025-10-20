#include "stringer.h"
#include <stdlib.h>
#include <string.h>

/**
 * Allocate a new string with an empty string ""
 */
struct string *string_new(void)
{
    struct string *mystring = malloc(sizeof(struct string));
    if (!mystring) {
        return NULL;
    }

    mystring->buffer = calloc(1, sizeof(char));
    if (!mystring->buffer) {
        free(mystring);
        return NULL;
    }

    mystring->capacity = 1;
    mystring->length = 0;
    return mystring;
}
/**
 * Allocate a new string by copying it from `cstr`
 */
struct string *string_new_from(const char *cstr)
{
    struct string *mystring = malloc(sizeof(struct string));
    mystring->buffer = strdup(cstr);
    mystring->capacity = strlen(cstr) + 1;
    mystring->length = strlen(cstr);
    return mystring;
}
/**
 * Reallocate the capacity of a string. Does not change the
 * string itself, but just allocates new memory. If `capacity`
 * is <= length, then length is set to capacity - 1.
 */
void string_realloc(struct string *s, size_t capacity)
{
    if (capacity > s->capacity) {
        s->buffer = realloc(s->buffer, capacity);
        s->capacity = capacity;
    }
    if (s->capacity <= s->length) {
        s->length = s->length - 1;
        s->buffer[s->length - 1] = '\0';
    }
}
/**
 * Frees all allocated memory for a string.
 */
void string_free(struct string *s)
{
    if (s->buffer) {
        free(s->buffer);
        s->buffer = NULL;
    }
    s->length = 0;
    s->capacity = 0;
    free(s);
}


// Utilities
/**
 * Pushes a single charater to the string.
 */
void string_push(struct string *s, char c)
{
    s->buffer = realloc(s->buffer, s->capacity +1);
    s->buffer = strncat(s->buffer, &c, 1);

    s->capacity =strlen(s->buffer) + 1;
    s->length = strlen(s->buffer);
}
/**
 * Appends a C-style string to the string. The new string
 * is a combination of both the old string and `cstr`.
 */
void string_append(struct string *s, const char *cstr)
{
    size_t cstr_length = strlen(cstr);
    size_t new_length = s->length + cstr_length;

    // realloc the buffer size no matter what
    s->buffer = realloc (s->buffer, new_length + 1);

    memcpy(s->buffer + s->length, cstr, cstr_length +1);
    s->length = new_length;
    s->capacity = new_length + 1;
}
/**
 * Prepends a C-style string to the string. The new string
 * is a combination of both the old string and `cstr`.
 */
void string_prepend(struct string *s, const char *cstr)
{
    size_t cstr_length = strlen(cstr);
    size_t new_length = s->length + cstr_length;

    //s->buffer = realloc(s->buffer, new_length + 1);
    char *temp_buff = realloc(s->buffer, new_length + 1);
    if(!temp_buff){
        return;
    }
    s->buffer = temp_buff;

    memmove(s->buffer + cstr_length, s->buffer, s->length +1);
    memcpy(s->buffer, cstr, cstr_length);

    s->length = new_length;
}
/**
 * Clears the string to empty string "". Does not free any memory.
 */
void string_clear(struct string *s)
{
    s->length = 0;
    s->buffer[0] = '\0';

}
/**
 * Sets the string exactly to the given C-string `cstr`.
 */
void string_exactly(struct string *s, const char *cstr)
{
    s->buffer = strdup(cstr);
    s->capacity = strlen(cstr) +1;
    s->length = strlen(cstr);
}
