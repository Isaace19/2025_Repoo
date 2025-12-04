// PThread Work Queue Implementation practice

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

struct Data {
	int value;
	bool is_prime;
};

struct Result {
	struct Data data;
	struct Result *next;
};

struct ResultList {
	struct Result *head;
};

struct WorkQueue {
	int size;
	int capacity;
	int at;
	int *data;
};

struct Worker {
	bool die;
	pthread_t tid;
	// add in the other requirements to have a worker queue. 
	// This include the cond_t signals
	
	pthread_cond_t *not_empty;
	pthread_cond_t *not_full;

	pthread_mutex_t *lock;
	struct WorkQueue *queue;
	struct ResultLIst *results;
};

static void *worker(void *arg);
static in wq_pop(struct WorkQueue *wq);
static bool wq_push(struct WorkQueue *wq, int value);
static bool wq_full(const struct WorkQueue *wq);
static bool wq_empty(const struct WorkQueue *wq);
static bool awful_is_prime(int value);

// write the main function

int main (int argc, char *argv[]) {
	int i;
	int start;
	int end;
	int num_workers;
	int work_queue_size;

    // command line parsing
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number of workers> <work queue size> <start prime> <end prime>\n", argv[0]);
        return 1;
    }

    if (1 != sscanf(argv[1], "%d", &num_workers) || num_workers <= 0 || num_workers > 10000) {
        fprintf(stderr, "Unable to convert number of threads '%s'.\n", argv[2]);
        return 1;
    }

    if (1 != sscanf(argv[2], "%d", &work_queue_size)) {
        fprintf(stderr, "Unable to scan work queue size '%s'.\n", argv[2]);
        return 1;    
    }

    if (1 != sscanf(argv[3], "%d", &start)) {
        fprintf(stderr, "Unable to convert starting prime '%s'.\n", argv[3]);
        return 1;
    }

    if (1 != sscanf(argv[4], "%d", &end)) {
        fprintf(stderr, "Unable to convert ending prime '%s'.\n", argv[4]);
        return 1;
    }

    if (start < 2) {
        start = 2;
    }

    if (end > 10000000) {
        end = 10000000;
    }

    // write the marshaller of the pthread setup here

    


}











