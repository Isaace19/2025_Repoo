/**
 * vector.h
 * C-style vectors
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
struct Vector;
typedef struct Vector Vector;

typedef int (SortFunc)(const void *, const void*);

Vector *vector_new(void);
Vector *vector_new_with_capacity(size_t capacity);
void vector_push(Vector *vec, int64_t value);
void vector_insert(Vector *vec, size_t index, int64_t value);
bool vector_get(const Vector *vec, size_t index, int64_t *value);
int64_t vector_get_unchecked(const Vector *vec, size_t index);
bool vector_set(Vector *vec, size_t index, int64_t value);
bool vector_remove(Vector *vec, size_t index);
void vector_shrink(Vector *vec);
void vector_sort(Vector *vec);
void vector_sort_by(Vector *vec, SortFunc comp);
ssize_t vector_find(const Vector *vec, int64_t value);
ssize_t vector_bsearch(const Vector *vec, int64_t value);
void vector_resize(Vector *vec, size_t new_size);
void vector_reserve(Vector *vec, size_t new_capacity);
void vector_clear(Vector *vec);
void vector_free(Vector *vec);
size_t vector_capacity(const Vector *vec);
size_t vector_size(const Vector *vec);

#define vector_push_ptr(vec, ptr)        vector_push(vec, (int64_t)ptr)
#define vector_find_ptr(vec, ptr)        vector_find(vec, (int64_t)ptr)
#define vector_insert_ptr(vec, idx, ptr) vector_insert(vec, idx, (int64_t)ptr)
#define vector_set_ptr(vec, idx, ptr)    vector_set(vec, idx, (int64_t)ptr)
#define vector_get_unchecked_ptr(vec, idx) (void*)vector_get_unchecked(vec, idx)
#define vector_bsearch_ptr(vec, ptr)     vector_bsearch(vec, (int64_t)ptr)
