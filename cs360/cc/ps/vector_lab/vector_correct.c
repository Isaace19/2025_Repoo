#include "vector.h"
#include <stdlib.h>
#include <search.h>
#include <sys/types.h>

struct Vector{
    size_t size;
    size_t capacity;
    int64_t *values;
};

// Write your vector functions here.
// int main() is written in main.c
// You can navigate files to the left of this window.

//need to allocate memory for the new vecotr element
Vector *vector_new(void)
{
  Vector *v = malloc(sizeof(Vector));
  v->size = 0; 
  v->capacity = 0;
  v->values = NULL;
  return (v);
}

//making a new vector taking into account capacity now
Vector *vector_new_with_capacity(size_t capacity)
{
  Vector *v = malloc(sizeof(Vector));
  v->size = 0; 
  v->capacity = capacity;
  v->values = malloc(sizeof(int64_t) * capacity);
  return (v);
}

// we need to make sure we are freeing all of values
void vector_free(Vector *vec)
{
  free(vec->values);
  vec->size = 0;
  vec->capacity = 0;
  free(vec);
}

//
void vector_push(Vector *vec, int64_t value)
{
  if(vec->size + 1 > vec->capacity)
  {
    vector_resize(vec, vec->capacity + 1);
  }
  vec->values[vec->size] = value;
  vec->size = vec->size + 1;
}

//
void vector_insert(Vector *vec, size_t index, int64_t value)
{
  if(vec->size + 1 > vec->capacity) //checking to see if we need to resize
  {
    vector_resize(vec, vec->capacity + 1);
  }

  if(index >= vec->size) //checking to see if we need to add it at the end
  {
    vec->values[vec->size] = value;
  }
  else //need to shift everything over to make space for the thing we are inserting
  {
    for(size_t i = vec->size; i > index; i--)
    {
      vec->values[i] = vec->values[i-1];
    }
    vec->values[index] = value;
  }
  vec->size = vec->size + 1;
}

//
bool vector_remove(Vector *vec, size_t index)
{
  if(index > vec->size - 1)
  {
    return(false);
  }
  else
  {
    for(size_t i = index; i < vec->size - 1; i++)
    {
      vec->values[i] = vec->values[i+1];
    }
    vec->size = vec->size -1;
    return(true);
  }
}

//
void vector_shrink(Vector *vec)
{
  vec->values = realloc(vec->values, sizeof(int64_t) * vec->size);
  vec->capacity = vec->size;
}

//
bool vector_get(const Vector *vec, size_t index, int64_t *value)
{
  if(index >= vec->size)
  {
    return(false);
  }
  else
  {
    *value = vec->values[index];
    return(true);
  }
}

//
int64_t vector_get_unchecked(const Vector *vec, size_t index)
{
  return(vec->values[index]);
}

//
bool vector_set(Vector *vec, size_t index, int64_t value)
{
  if(index >= vec->size)
  {
    return(false);
  }
  else
  {
    vec->values[index] = value;
    return(true);
  }
}

//
int comp_ascending(const void *left, const void *right)
{
  int64_t l = *(const int64_t *)left;
  int64_t r = *(const int64_t *)right;
  if(l < r)
  {
    return (-1);
  }
  else if(l>r)
  {
    return (1);
  }
  else
  {
    return (0);
  }
}
void vector_sort(Vector *vec)
{
  vector_sort_by(vec, comp_ascending);
}

//
void vector_sort_by(Vector *vec, SortFunc comp)
{
  qsort(vec->values, vec->size, sizeof(int64_t), comp);
}

//
ssize_t vector_bsearch(const Vector *vec, int64_t value)
{
  int64_t *spot = bsearch(&value, vec->values, vec->size, sizeof(int64_t), comp_ascending);

  if(spot == NULL)
  {
    return (-1);
  }
  return (spot - vec->values);  
}

//
ssize_t vector_find(const Vector *vec, int64_t value)
{
  size_t n = vec->size;
  int64_t *result = lfind(&value, vec->values, &n, sizeof(int64_t), comp_ascending);
  if(result != NULL)
  {
    return(result - vec->values);
  }
  else
  {
    return (-1);
  }
}

void vector_resize(Vector *vec, size_t new_size)
{
  if(new_size > vec->capacity)
  {
   vec->values = realloc(vec->values, new_size * sizeof(int64_t));
   vec->capacity = new_size;
  }
  vec->size = new_size;
}

//
void vector_reserve(Vector *vec, size_t new_capacity)
{
  if(new_capacity < vec->size)
  {
    new_capacity = vec->size;
  }
  if(new_capacity > vec->capacity)
  {
    vec->values = realloc(vec->values, sizeof(int64_t) * new_capacity);
    vec->capacity = new_capacity; 
  }
}

//
void vector_clear(Vector *vec)
{
  vec->size = 0;
}

//
size_t vector_capacity(const Vector *vec)
{
  return (vec->capacity);
}

//
size_t vector_size(const Vector *vec)
{
  return (vec->size);
}
