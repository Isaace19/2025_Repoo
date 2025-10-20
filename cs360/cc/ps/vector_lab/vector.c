#include "vector.h"
#include <stdlib.h>
#include <search.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>


// Write your vector functions here.
// int main() is written in main.c
// You can navigate files to the left of this window.

// change: put functions in order

struct Vector{
	size_t size;     // Amount of things stored into the vector object
	size_t capacity; // Actual memory allocated to the vector object
	int64_t *values; // pointer value in a struct to avoid infinite values.
};


Vector *vector_new(void) {
	return vector_new_with_capacity(0);
}

Vector *vector_new_with_capacity(size_t capacity) {
	Vector *vec = malloc(sizeof *vec);

	// check if the pointer is null before usage
	if (!vec) {
		return NULL;
	}

    vec->size = 0;
    vec->capacity = capacity;
	vec->values = NULL;

	if (capacity > 0) {
		vec->values = (int64_t *)malloc(capacity * sizeof(int64_t));
		if (vec->values == NULL) {
			free(vec);
			return NULL;
		}
	}
	return vec;
}

void vector_push(Vector *vec, int64_t value) {
	if (vec->size == vec->capacity) {
		size_t  new_buff;
		size_t old_cap = vec->capacity;
		if (vec->capacity == 0) {
			new_buff = 1;
		}else {
			new_buff = vec->capacity * 2;
		}
		vector_reserve(vec, new_buff);
		if (vec->capacity == old_cap) {
			return;
		}
	}
	vec->values[vec->size] = value;
	vec->size++;

}

void vector_insert(Vector *vec, size_t index, int64_t value) {
	if (index > vec->size) {
		return;
	}

	if (vec->size == vec->capacity) {
		size_t old_cap = vec->capacity;
		size_t new_cap;

		if(vec->capacity == 0) {
			new_cap = 1;
		}else {
			new_cap = vec->capacity * 2;
		}
		vector_reserve(vec, new_cap);
		if(vec->capacity == old_cap) {
			return;
		}
	}

	memmove(&vec->values[index + 1], &vec->values[index], (vec->size - index) * sizeof(int64_t));
	vec->values[index] = value;
	vec->size++;
}

bool vector_get(const Vector *vec, size_t index, int64_t *value) {
	if (index >= vec->size){
		return false;
	}
	*value = vec->values[index];
	return true;
}

int64_t vector_get_unchecked(const Vector *vec, size_t index){
	return vec->values[index];
}

bool vector_set(Vector *vec, size_t index, int64_t value) {
	// return out immediately if the index is already the same as the value we want
	if (index >= vec->size){
		return false;
	}
	vec->values[index] = value;
	return true;
}

bool vector_remove (Vector * vec, size_t index) {
	// basing this one off of the darn std::vector erase function.
	if (index >= vec->size) {
		return false;
	}

	if (index < vec->size - 1) {
		memmove(&vec->values[index], &vec->values[index + 1], (vec->size - index - 1) * sizeof(int64_t));
	}
    vec->size--;
	return true;
}

void vector_shrink (Vector *vec) {
   if (!vec) {
	return;
   }
	if (vec->size == 0) {
		free(vec->values);
		vec->values = NULL;
		vec->capacity = 0;
		return;
	}else if (vec->size < vec->capacity) {
		vec->values = realloc(vec->values, vec->size * sizeof(int64_t));
		vec->capacity = vec->size;
	}

	/*int64_t *tmp = realloc(vec->values, vec->size * sizeof *tmp);
	if(!tmp){
		return;
	}
	vec->values = tmp;
	vec->capacity = vec->size;*/
}

int comp_ascending(const void *lp, const void *rp) {
	int64_t left = *(const int64_t *)lp;
	int64_t right = *(const int64_t *)rp;
	if (left < right) return -1;
	if (left > right) return 1;
	return 0;
}

void vector_sort(Vector *vec) {
	vector_sort_by(vec, comp_ascending);
}

void vector_sort_by(Vector *vec, SortFunc comp) {
	// move initial implementation of qsort function from vector_bsearch into here.
	qsort(vec->values, vec->size, sizeof(int64_t), comp);
}

ssize_t vector_find(const Vector *vec, int64_t value) {
	// attempt without lfind function

	/*for(int i = 0; i < vec->size; i++) {
		if (vec->values[i] == value) {
			return *vec->values;
		}else{
			return -1;
		}
	}*/

	// this function for find: set a key as the value and set a pointer to result.

	// result should be equal to the lfind of the key value searched through our dynamic array, feeding in the size, the type, and the compare of ints.

	// set the key, the vec array, num of array elements, the width of array elements int64_t, and a compare function of the key to the current value ur at
	if (vec == NULL) {
		return -1;
	}

	size_t n_member = vec->size;
	int64_t key = value;
	int64_t *result = (int64_t *)lfind(&key, vec->values, &n_member, sizeof(int64_t), comp_ascending);
	if(result){
		return (ssize_t)(result - vec->values);
	}
	return -1;
}


ssize_t vector_bsearch(const Vector *vec, int64_t value) {
	if (vec == NULL){
		return -1;
	}

	//vector_sort ((Vector *)vec);

	int64_t key = value;
	int64_t *i = bsearch(&key, vec->values, vec->size, sizeof(int64_t), comp_ascending);
	if(i) {
		return i - (vec->values);
	}
	return -1;
}

void vector_resize(Vector *vec, size_t new_size) {
    if (new_size > vec->capacity) {
        // grow by doubling until we're big enough
        size_t newcap = vec->capacity ? vec->capacity * 2 : 1;
        while (newcap < new_size) {
            newcap *= 2;
        }
        // use your reserve logic
        vec->values = realloc(vec->values,
                              newcap * sizeof *vec->values);
        vec->capacity = newcap;
    }
    // always set the logical size last
    vec->size = new_size;
}


void vector_reserve(Vector *vec, size_t new_capacity) {
	if (new_capacity <= vec->capacity){
		return;
	}
	vec->values = realloc(vec->values, new_capacity * sizeof(int64_t));
	vec->capacity = new_capacity;
}

void vector_clear(Vector *vec) {
	vec->size = 0;
}

void vector_free(Vector *vec) {
	if (vec == NULL){
		return;
	}
	free(vec->values);
    free(vec);
}

size_t vector_capacity(const Vector *vec) {
	return vec->capacity;
}

size_t vector_size(const Vector *vec) {
	return vec->size;
}
