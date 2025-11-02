#include <stdio.h>
#include "tpool.h"

// This file is NOT graded. Only your tpool.c
// will be tested.

void *thread_pool_open(int num_threads);


int main()
{
    void *handle;
    handle = thread_pool_open(30); // open 30 jobs
    if (handle == NULL) {
        printf()
    }

    printf("Hello World.\n");
}

