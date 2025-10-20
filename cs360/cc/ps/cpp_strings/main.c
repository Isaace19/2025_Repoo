#include <stdio.h>
#include "stringer.h"

static void print_string(const struct string *st)
{
    printf("%s\nLength: %zu\nCapacity: %zu\n", st->buffer, st->length, st->capacity);
}

int main()
{
    struct string *st;
    
    st = string_new_from("Hello World");
    if (st == NULL) {
        printf("I'm assuming you haven't written anything yet.\n");
        return -1;
    }
    print_string(st);
    string_append(st, ", instead goodbye.");
    print_string(st);
    string_prepend(st, "Ignore what I'm about to say: ");
    print_string(st);
    string_free(st);

    st = string_new();
    print_string(st);
    string_free(st);
    st = string_new();
    string_push(st, 'A');
    string_push(st, 'B');
    string_push(st, 'C');
    print_string(st);
    string_prepend(st, "The three characters I pushed were: ");
    print_string(st);
    string_clear(st);
    print_string(st);
    string_exactly(st, "Ok, last test...");
    print_string(st);
    string_free(st);
    
    return 0;
}
