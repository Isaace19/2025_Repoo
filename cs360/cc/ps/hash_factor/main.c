#include "hfactor.h"
#include <stdio.h>

// WARNING: Grading is slow due to large numbers being tested!
// Test manually first before grading!

// The RUN button in Zylabs will compile as:
// gcc -o /tmp/Reporter reporter.c
// gcc -o /tmp/Hfactor hfactor.c main.c -lrt

// It will then run as follows:
// /tmp/Hfactor 1778 4 | sort -n
// 2
// 7
// 14
// 127
// 254
// 889

int hfactor_init(mqd_t *q, sem_t **sem, int *shm);
void hfactor_deinit(mqd_t q, sem_t *sem, int shm);
int hfactor_report(const char*reporter, int shm);
int hfactor_calculate(const char* reporter, uint64_t target, uint64_t num_workers);


int main(int argc, char *argv[]) {
    uint64_t number_to_test;
    uint64_t workers;

    if (argc < 3) {
        printf("Usage: %s <number to test> <workers>\n", argv[0]);
        return -1;
    }
    if (1 != sscanf(argv[1], "%lu", &number_to_test)) {
        printf("Unable to scan number.\n");
        return -1;
    }
    if (1 != sscanf(argv[2], "%lu", &workers)) {
        printf("Unable to scan workers.\n");
        return -1;
    }
    printf("TESTER RUNNING /tmp/Reporter\n");
    // return hfactor_calculate("/tmp/Reporter", number_to_test, workers);

	// write some testing functions
	
	/* TESTING FOR HFACTOR INITIALIZATION FUNCTION*/
	
	mqd_t q;
	sem_t *sem;
	int shm_fd;

	if(hfactor_init(&q, &sem, &shm_fd) == -1){
		fprintf(stderr, "Error with initialization of IPC\n");
		return 1;
	}

	fprintf(stdout, "INITIALIZATION SUCCESS\n");
	printf("memq output: %d\n", q);
	printf("semaphore output: %p\n", (void *)sem);
	printf("shared memory output: %d\n", shm_fd);

	// testing locking and waiting conditions of the IPC functions
	
	sem_wait(sem);
	printf("Confirmation of semaphore functions working properly\n");
	sem_post(sem);


	/* TESTING THE HFACTOR REPORTING OF CHILD AND PARENT FORKING */
	uint64_t num_workers = 100;
	
		




	hfactor_deinit(q, sem, shm_fd);
	
}


