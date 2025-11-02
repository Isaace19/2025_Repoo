#pragma once

#include <stdint.h>

typedef uint64_t (*Executor)(int val);

void *thread_pool_open(int num_threads);
uint64_t *thread_pool_execute(void *handle, const int *work_vals, int num_work, const Executor executor);
void thread_pool_close(void *handle);

// Executors
uint64_t hash32(int fd);
uint64_t hash64(int fd);

