#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stddef.h>



// The goal is to recognize the basic pthread syntax and be able to write it naturally and fast come exam time. 
//
void *worker (void *arg) {
	int *v = (int *)arg;
	*v = 1234;
	return NULL;
}

int main(int argc, char *argv[]) {

	// given a number of threads, or perhaps we can even practice having the user input the threads directly. 
	int num_threads;
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <num threads>\n", argv[0]);
	    return 1;
	}
    if (1 != sscanf(argv[1], "%d", &num_threads) || num_threads <= 0 || num_threads > 1000) {
		fprintf(stderr, "Invalid number of threads: '%s'\n", argv[1]);
        return 1;
	}	

	// write the actual pthread object, then we have to have an integer for the actual value at each index of the array
	pthread_t tids[num_threads];
	int tids_val[num_threads];
	// create an array of pthreads 
	num_threads = atoi(argv[1]); // gets the int of the amount of numthreads to make

	int i;
	for (i = 0; i < num_threads; i+=1) {
		pthread_create(&tids[i], NULL, worker, &tids_val[i]);
	}	

	// print out the worker function:
	for (i = 0; i < num_threads; i+=1) {
		if (pthread_join(tids[i], NULL) != 0) {
			perror("pthread_join");
			return 1;
		}
		printf("%d", tids_val[i]);
		printf("\n");
	}
	return 0;
}
