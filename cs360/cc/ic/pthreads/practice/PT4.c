// Writing a basic work queue

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *worker(void *arg) {
    int *v = (int *)arg;
    *v = 1234;
    return NULL;
};


int main(int argc, char * argv[]) {
    int num_threads;

    // perform a basic command line check:
    if (argc != 2) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    if (1 != sscanf(argv[1], "%d", &num_threads)) {
        fprintf(stderr, "Could not convert the input number of workers to int.\n");
        return 1;
    }

    // create the actual pthread object as an array of however many workers needed
    pthread_t tids[num_threads];
    // create an index value for the array
    int tids_value[num_threads];

    int i;
    for (i = 0; i < num_threads; i+=1) {
        int result = pthread_create(&tids[i], NULL, worker, &tids_value[i]);
        if (result == -1) {
            perror("pthread_create");
            return 1;
        }
    }

    // then we want to output whatever the workers did.
    // for this class we usually set the status field to NULL 
    for (i = 0; i < num_threads; i+=1) {
        // use the pthread_join
        int printout = pthread_join(tids[i], NULL);
        printf("%d", tids_value[i]);
        printf("\n");
    }

    return 0;
}
