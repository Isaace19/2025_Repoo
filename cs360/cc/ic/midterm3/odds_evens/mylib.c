#include <stdlib.h>
#include <pthread.h>

// process the array to determine even or odd. 

void worker(void * arg) {
    // the worker process that gets doled out to check over or odd
    // cast to int
    int *num = (int *)arg; 
    // check or even or odd
    if (*num % 2 == 0) {
        printf("EVEN!");
    }else {
        printf("ODD!");
    }
}

// create a function that reads in everything 

