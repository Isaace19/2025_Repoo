/*
 * Attempt of memorizing and understanding PThreads better from repeating our zybook
 *
 * Attempt 1 had me copying down the zybook, looking at lecture notes, and then asking AI why it was having compilation errors. 
 *
 * Here is Attempt 2:
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <pthread.h>

void *worker(void *arg) {
    int *v = (int *)arg;
    *v = 1234;
    return NULL;
};

int main(int argc, char *argv[]) {
    int num_threads;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    
    if (1 != sscanf(argv[1], "%d", &num_threads) || num_threads <= 0 || num_threads >1000) {
        fprintf(stderr, "Invalid number of threads: '%s'\n", argv[1]);
        return 1;
    }

    pthread_t tcb[num_threads]; 
    // creates the thread control block object based on how many number of threads we want to generate
    int i;
    int tids[num_threads]; 
    // contents of each tcb pointer address is given by the amount of num threads that we want
    void *status;

    for (i = 0; i < num_threads; i+=1) {
        tids[i] = i;
        // fed in address of a thread, NULL for attribute, then the worker function, and then it is stored at the index of thread id for that thread process. 
        int result = pthread_create(&tcb[i], NULL, worker, &tids[i]);

        // so this says create a new thread control block of ith amount, default attributes, running the void * argument above, and storing that result into the ith thread id. which in this case is an array of thread ids for the number of threads 
        if (result != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // join the pthreads and error check and print out the result of the worker function argument
    for (i = 0; i < num_threads; i+=1) {
        if (pthread_join(tcb[i], &status) != 0) {
            perror("pthread_join");
            return 1;
        }
        printf("%d", tids[i]);
        printf("\n");
    }

    return 0;
}
