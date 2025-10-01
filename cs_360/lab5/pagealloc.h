#pragma once
#include <stdbool.h> // bool
#include <stddef.h>

bool page_init(size_t pages);
void page_deinit(void);

void *page_alloc(size_t pages);
void page_free(void *addr);

size_t pages_taken(void);
size_t pages_free(void);
